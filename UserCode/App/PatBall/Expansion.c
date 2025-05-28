/*
 * @Description: 伸缩机构
 * @Author: Alex
 * @Date: 2025-03-22 10:35:23
 * @LastEditors: Alex
 * @LastEditTime: 2025-05-24 21:58:25
 */
#include "Expansion.h"

/**
 * @brief 线程定义
 * 
 */
osThreadId_t Expansion_Executor_TaskHandle;
const osThreadAttr_t Expansion_Executor_Task_attributes = {
    .name       = "Expansion_Executor_Task",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

/**
 * @brief 接口结构体定义
 * 
 */
Expansion_t Expansion_Up;
Expansion_t Expansion_Down;

float DJI_Output_WithFC_Down[2];
float DJI_Output_WithFC_Up[2];

uint8_t Button[4] = {0};

void m_up_expandhighly()
{
    for (uint8_t i = 0; i < 2; ++i) Expansion_Up.m_Angle[i] = Expansion_Up.m_StartAngle[i] - 360 * UPHIGH_ROTATION;
}

void m_up_expand()
{
    for (uint8_t i = 0; i < 2; ++i) Expansion_Up.m_Angle[i] = Expansion_Up.m_StartAngle[i] - 360 * UP_ROTATION;
}

void m_up_contract()
{
    for (uint8_t i = 0; i < 2; ++i) Expansion_Up.m_Angle[i] = Expansion_Up.m_StartAngle[i];
}

void m_down_expandhighly()
{
    for (uint8_t i = 0; i < 2; ++i) Expansion_Down.m_Angle[i] = Expansion_Down.m_StartAngle[i] - 360 * DOWNHIGH_ROTATION;
}

void m_down_expand()
{
    for (uint8_t i = 0; i < 2; ++i) Expansion_Down.m_Angle[i] = Expansion_Down.m_StartAngle[i] - 360 * DOWN_ROTATION;
}

void m_down_contract()
{
    for (uint8_t i = 0; i < 2; ++i) Expansion_Down.m_Angle[i] = Expansion_Down.m_StartAngle[i];
}

/**
 * @brief 伸缩机构结构体初始化函数
 * 
 */
void Expansion_Init()
{
    hDJI[4].motorType = M3508;
    hDJI[5].motorType = M3508;
    hDJI[6].motorType = M2006;
    hDJI[7].motorType = M2006;
    Expansion_Down.m_DJI = &hDJI[4];
    Expansion_Down.m_Angle[0] = 0;
    Expansion_Down.m_Angle[1] = 0;
    Expansion_Down.m_StartAngle[0] = 0;
    Expansion_Down.m_StartAngle[1] = 0;
    Expansion_Down.expandhighly = m_down_expandhighly;
    Expansion_Down.expand = m_down_expand;
    Expansion_Down.contract = m_down_contract;

    Expansion_Up.m_DJI = &hDJI[6];
    Expansion_Up.m_Angle[0] = 0;
    Expansion_Up.m_Angle[1] = 0;
    Expansion_Up.m_StartAngle[0] = 0;
    Expansion_Up.m_StartAngle[1] = 0;
    Expansion_Up.expandhighly = m_up_expandhighly;
    Expansion_Up.expand = m_up_expand;
    Expansion_Up.contract = m_up_contract;

    for (int i = 4; i < 8; i++)
	{
		// 速度环PID
		hDJI[i].speedPID.KP = 12;
		hDJI[i].speedPID.KI = 0.2;
		hDJI[i].speedPID.KD = 5;
		hDJI[i].speedPID.outputMax = 12000;

		// 位置环PID
		hDJI[i].posPID.KP = 80.0f;
		hDJI[i].posPID.KI = 1.0f;
		hDJI[i].posPID.KD = 0.0f;
		hDJI[i].posPID.outputMax = 12000;
		// hDJI[i].posPID.outputMin = 1500;
		hDJI[i].f_current = 0;
		if (hDJI[i].motorType == M3508)
		{
			hDJI[i].reductionRate = 3591.0f / 187.0f; // 2006减速比为36 3508减速比约为19
		}
		else if (hDJI[i].motorType == M2006)
		{
			hDJI[i].reductionRate = 36.0f;
		}

		hDJI[i].encoder_resolution = 8192.0f;
	}
    for (int i = 6; i < 8; ++i)
    {
        hDJI[i].posPID.outputMax=25000;
        hDJI[i].posPID.KP=135.0f;
        hDJI[i].posPID.KI=5.0f;
        hDJI[i].posPID.KD=0.02f;
        hDJI[i].speedPID.outputMax=25000;
        hDJI[i].speedPID.KP=12.0;
        hDJI[i].speedPID.KI=0.55;
        hDJI[i].speedPID.KD=2.0;
    }
}

/**
 * @brief 创建升降机构执行器线程
 * 
 */
void Expansion_Executor_TaskStart()
{
    Expansion_Executor_TaskHandle = osThreadNew(Expansion_Executor_Task, NULL, &Expansion_Executor_Task_attributes);
}

/**
 * @brief 升降机构执行器线程定义
 * 
 * @param argument 
 */
void Expansion_Executor_Task(void* argument)
{
//STARTUP:
    do
    {
        Button[0] = HAL_GPIO_ReadPin(ButtonLH_GPIO_Port, ButtonLH_Pin);
        Button[1] = HAL_GPIO_ReadPin(ButtonLL_GPIO_Port, ButtonLL_Pin);
        Button[2] = HAL_GPIO_ReadPin(ButtonRH_GPIO_Port, ButtonRH_Pin);
        Button[3] = HAL_GPIO_ReadPin(ButtonRL_GPIO_Port, ButtonRL_Pin);
        if (Button[1] == 1) speedServo(4000, &hDJI[4]);
        else hDJI[4].speedPID.output = 0;
        if (Button[3] == 1) speedServo(4000, &hDJI[5]);
        else hDJI[5].speedPID.output = 0;
        if (Button[0] == 1) speedServo(8000, &hDJI[6]);
        else hDJI[6].speedPID.output = 0;
        if (Button[2] == 1) speedServo(8000, &hDJI[7]);
        else hDJI[7].speedPID.output = 0;
        CanTransmit_DJI_5678(&hcan2, hDJI[4].speedPID.output, hDJI[5].speedPID.output, hDJI[6].speedPID.output, hDJI[7].speedPID.output);
        HAL_Delay(2);
    } while (Button[0] == 1 || Button[1] == 1 || Button[2] == 1 || Button[3] == 1);
    for (uint8_t i = 0; i < 2; ++i)
    {
        Expansion_Down.m_StartAngle[i] = Expansion_Down.m_DJI[i].AxisData.AxisAngle_inDegree - 200;
        Expansion_Down.m_Angle[i] = Expansion_Down.m_StartAngle[i];
    }
    for (uint8_t i = 0; i < 2; ++i)
    {
        Expansion_Up.m_StartAngle[i] = Expansion_Up.m_DJI[i].AxisData.AxisAngle_inDegree - 200;
        Expansion_Up.m_Angle[i] = Expansion_Up.m_StartAngle[i];
    }
    for (;;)
    {
        if (MyRemote_Data.btn_LeftCrossUp == 1)
        {
            Expansion_Up.expandhighly();
            Expansion_Down.expandhighly();
        }
        else if (MyRemote_Data.btn_LeftCrossMid == 1)
        {
            Expansion_Up.expand();
            Expansion_Down.expand();
        }
        else if (MyRemote_Data.btn_LeftCrossDown == 1)
        {
            Expansion_Up.contract();
            Expansion_Down.contract();
        }
        /* 此处代码留作可添加部分，可以按照下面这个方法写，也可以直接将for(;;)前面的部分复制到这里 */
        // else if (按某一个按键再次进行升降机构的初始位置校准)
        // {
        //     goto STARTUP;
        // }
        for (uint8_t i = 0; i < 2; ++i)
        {
            positionServo(Expansion_Down.m_Angle[i],   Expansion_Down.m_DJI + i);
            if((Expansion_Down.m_DJI + i)->speedPID.output > 100)
                DJI_Output_WithFC_Down[i] = (Expansion_Down.m_DJI + i)->speedPID.output + (Expansion_Down.m_DJI + i)->f_current;
            else if((Expansion_Down.m_DJI + i)->speedPID.output < -100)
                DJI_Output_WithFC_Down[i] = (Expansion_Down.m_DJI + i)->speedPID.output - (Expansion_Down.m_DJI + i)->f_current;
            else
                DJI_Output_WithFC_Down[i] = (Expansion_Down.m_DJI + i)->speedPID.output;
            
            positionServo(Expansion_Up.m_Angle[i],     Expansion_Up.m_DJI + i);        
            if((Expansion_Down.m_DJI + i)->speedPID.output > 100)
                DJI_Output_WithFC_Up[i] = (Expansion_Up.m_DJI + i)->speedPID.output + (Expansion_Up.m_DJI + i)->f_current;
            else if((Expansion_Down.m_DJI + i)->speedPID.output < -100)
                DJI_Output_WithFC_Up[i] = (Expansion_Up.m_DJI + i)->speedPID.output - (Expansion_Up.m_DJI + i)->f_current;
            else
                DJI_Output_WithFC_Up[i] = (Expansion_Up.m_DJI + i)->speedPID.output;
        }
        
        CanTransmit_DJI_5678(MX_CAN, DJI_Output_WithFC_Down[0], DJI_Output_WithFC_Down[1], DJI_Output_WithFC_Up[0], DJI_Output_WithFC_Up[1]);
        //CanTransmit_DJI_5678(MX_CAN,0,0,0,0);
        osDelay(5);
    }
}