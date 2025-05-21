/*
 * @Description: 伸缩机构
 * @Author: Alex
 * @Date: 2025-03-22 10:35:23
 * @LastEditors: Alex
 * @LastEditTime: 2025-03-22 17:10:58
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

void m_up_expandhighly()
{
    Expansion_Up.m_Angle = -360 * UPHIGH_ROTATION;
}

void m_up_expand()
{
    Expansion_Up.m_Angle = -360 * UP_ROTATION;
}

void m_up_contract()
{
    Expansion_Up.m_Angle = 0;
}

void m_down_expandhighly()
{
    Expansion_Down.m_Angle = -360 * DOWNHIGH_ROTATION;
}

void m_down_expand()
{
    Expansion_Down.m_Angle = -360 * DOWN_ROTATION;
}

void m_down_contract()
{
    Expansion_Down.m_Angle = 0;
}

/**
 * @brief 伸缩机构结构体初始化函数
 * 
 */
void Expansion_Init()
{
    for (uint8_t i = 4; i < 8; ++i) hDJI[i].motorType = M3508;
    Expansion_Down.m_DJI = &hDJI[4];
    Expansion_Down.m_Angle = 0;
    Expansion_Down.expandhighly = m_down_expandhighly;
    Expansion_Down.expand = m_down_expand;
    Expansion_Down.contract = m_down_contract;

    Expansion_Up.m_DJI = &hDJI[6];
    Expansion_Up.m_Angle = 0;
    Expansion_Up.expandhighly = m_up_expandhighly;
    Expansion_Up.expand = m_up_expand;
    Expansion_Up.contract = m_up_contract;

    /*for (int i = 4; i < 8; i++) {
        // 速度环PID
		hDJI[i].speedPID.KP = 12;
		hDJI[i].speedPID.KI = 0.2;
		hDJI[i].speedPID.KD = 5;
		hDJI[i].speedPID.outputMax = 5000;

		// 位置环PID
		hDJI[i].posPID.KP = 80.0f;
		hDJI[i].posPID.KI = 1.0f;
		hDJI[i].posPID.KD = 0.0f;
		hDJI[i].posPID.outputMax = 5000;
    } */
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

        for (uint8_t i = 0; i < 2; ++i)
        {
            positionServo(Expansion_Down.m_Angle,   Expansion_Down.m_DJI + i);
            if((Expansion_Down.m_DJI + i)->speedPID.output > 100)
                DJI_Output_WithFC_Down[i] = (Expansion_Down.m_DJI + i)->speedPID.output + (Expansion_Down.m_DJI + i)->f_current;
            else if((Expansion_Down.m_DJI + i)->speedPID.output < -100)
                DJI_Output_WithFC_Down[i] = (Expansion_Down.m_DJI + i)->speedPID.output - (Expansion_Down.m_DJI + i)->f_current;
            else
                DJI_Output_WithFC_Down[i] = (Expansion_Down.m_DJI + i)->speedPID.output;
            
            positionServo(Expansion_Up.m_Angle,     Expansion_Up.m_DJI + i);        
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