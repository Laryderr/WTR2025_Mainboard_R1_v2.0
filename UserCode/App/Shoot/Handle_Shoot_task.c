/**
 * @file Shoot_task.c
 * @author Lary (you@domain.com)
 * @brief  WTR2025R1 后抛投篮线程
 * @version 0.1
 * @date 2025-03-10
 * @note 投篮线程需要接受
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #include "Handle_Shoot_task.h"

 Shoot_Task_T my_Shoot_Task_T;
 /****************************************************线程定义******************************************/
osThreadId_t unitree_shoot_ctrl_TaskHandle;
const osThreadAttr_t unitree_shoot_ctrl_Task_attributes = {
    .name       = "unitree_shoot_ctrl_Task",
    .stack_size = 128*4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void Handle_Shoot_Task(void *argument);


/**
 * @brief Unitree电机后抛手动投篮控制线程创建
 * 
 */
void Handle_Shoot_TaskStart(void)
{
    unitree_shoot_ctrl_TaskHandle = osThreadNew(Handle_Shoot_Task,NULL,&unitree_shoot_ctrl_Task_attributes);
}

/**************************************************内部控制函数***********************************************************************/

/**
 * @brief 寻找投篮电机姿态初始位置
 * @return float 
 */
HAL_StatusTypeDef Shootmotor_Contect_InitialPos(Shoot_Task_T * my_T)
{
    for (uint8_t i = 3; i < 5; i++)
    {
        unitree_DunkMotor_t[i].cmd.Pos = 0;
        unitree_DunkMotor_t[i].cmd.K_P = 0;
        unitree_DunkMotor_t[i].cmd.K_W = 0;
        unitree_DunkMotor_t[i].cmd.T = 0;
        unitree_DunkMotor_t[i].cmd.W = 0;
    }
    //osDelay(INITIAL_DELAY_TIME);//使投篮臂自然垂下
    
    for (uint8_t i = 0; i < 10; i++)
    {
        my_T->Shootball_InitialPos[0] += unitree_DunkMotor_t[3].data.Pos;
        my_T->Shootball_InitialPos[1] += unitree_DunkMotor_t[4].data.Pos;
    }
    my_T->Shootball_InitialPos[0] = my_T->Shootball_InitialPos[0]/10.0f;
    my_T->Shootball_InitialPos[1] = my_T->Shootball_InitialPos[1]/10.0f;
    
    for (uint8_t i = 0; i < 2; i++)
    {
        if(my_T->Shootball_InitialPos[i] != 0)
        {
            my_T->Foundshoot_InitialPos_Flag[i] = 1;
        }else return HAL_ERROR; 
    }
    return HAL_OK;
    
}

/**
 * @brief 投篮电机初始化
 * 
 * @param my_T 
 * @return HAL_StatusTypeDef 
 */
HAL_StatusTypeDef Shootmotor_Init(Shoot_Task_T * my_T)
{
    my_T->myshoot_status = SHOOT_IDLE;
    my_T->Foundshoot_InitialPos_Flag[0]=0;
    my_T->Foundshoot_InitialPos_Flag[1]=0;
    my_T->Shootball_InitialPos[0]=0;
    my_T->Shootball_InitialPos[1]=0;
    my_T->target_throw_deg = 0;
    my_T->target_throw_w   = 0;
    my_T->Shoot_Completed_Flag =0;

    if (Shootmotor_Contect_InitialPos(my_T) == HAL_OK )
    {
        return HAL_OK;
    }else return HAL_ERROR;
    
}




/****************************************************************************************************************************** 
 * @brief Unitree电机后抛投篮控制线程实现
 * 
 */
void Handle_Shoot_Task(void*argument)
{
    if (Shootmotor_Init(&my_Shoot_Task_T) != HAL_OK)
    {
        Error_Handler();
    }
    
    for (;;)
    {
        //遥控器控制投篮状态机
        if(MyRemote_Data.btn_JoystickR == 1)
        {
            my_Shoot_Task_T.myshoot_status = SHOOT_ING;
        }
        if (MyRemote_Data.btn_JoystickL == 1)
        {
            my_Shoot_Task_T.myshoot_status = SHOOT_IDLE;
        }
        my_Shoot_Task_T.myshoot_status = SHOOT_ING;
        
        switch (my_Shoot_Task_T.myshoot_status)
        {
        case SHOOT_IDLE:
            my_Shoot_Task_T.Shoot_Completed_Flag = 0;

            unitree_DunkMotor_t[3].cmd.Pos = 0;
            unitree_DunkMotor_t[3].cmd.K_P = 0;
            unitree_DunkMotor_t[3].cmd.K_W = 0;
            unitree_DunkMotor_t[3].cmd.W = 0;
            unitree_DunkMotor_t[3].cmd.T = 0;
            unitree_DunkMotor_t[4].cmd.Pos = 0;
            unitree_DunkMotor_t[4].cmd.K_P = 0;
            unitree_DunkMotor_t[4].cmd.K_W = 0;
            unitree_DunkMotor_t[4].cmd.W = 0;
            unitree_DunkMotor_t[4].cmd.T = 0;
            osDelay(1);
            break;
        case SHOOT_ING:
            if (my_Shoot_Task_T.Shoot_Completed_Flag == 0)
            {
                unitree_DunkMotor_t[3].cmd.Pos = 0;
                unitree_DunkMotor_t[3].cmd.K_P = 0;
                unitree_DunkMotor_t[3].cmd.K_W = 0;
                unitree_DunkMotor_t[3].cmd.W = 0;
                unitree_DunkMotor_t[3].cmd.T = -3.8;
                unitree_DunkMotor_t[4].cmd.Pos = 0;
                unitree_DunkMotor_t[4].cmd.K_P = 0;
                unitree_DunkMotor_t[4].cmd.K_W = 0;
                unitree_DunkMotor_t[4].cmd.W = 0;
                unitree_DunkMotor_t[4].cmd.T = 3.8;
                my_Shoot_Task_T.Shoot_Completed_Flag =1;
            }

            //刹停力矩
            if(my_Shoot_Task_T.Shoot_Completed_Flag==1 && encoderData.angle >= Encoder_VertPos + 100 &&
               encoderData.angle <= 250)
            {
                my_Shoot_Task_T.Shoot_Completed_Flag =2;
                
                unitree_DunkMotor_t[3].cmd.Pos = 0;
                unitree_DunkMotor_t[3].cmd.K_P = 0;
                unitree_DunkMotor_t[3].cmd.K_W = 0;
                unitree_DunkMotor_t[3].cmd.W = 0;
                unitree_DunkMotor_t[3].cmd.T = 2;

                unitree_DunkMotor_t[4].cmd.Pos = 0;
                unitree_DunkMotor_t[4].cmd.K_P = 0;
                unitree_DunkMotor_t[4].cmd.K_W = 0;
                unitree_DunkMotor_t[4].cmd.W = 0;
                unitree_DunkMotor_t[4].cmd.T = 0;
                
            }
            //下降控制
            if (encoderData.angle <= Encoder_VertPos + 90 && my_Shoot_Task_T.Shoot_Completed_Flag == 2 )
            {
                my_Shoot_Task_T.Shoot_Completed_Flag =3;
                unitree_DunkMotor_t[3].cmd.Pos = 0;
                unitree_DunkMotor_t[3].cmd.K_P = 0;
                unitree_DunkMotor_t[3].cmd.K_W = 0.2;
                unitree_DunkMotor_t[3].cmd.W = 0.05;
                unitree_DunkMotor_t[3].cmd.T = 0;

                unitree_DunkMotor_t[4].cmd.Pos = 0;
                unitree_DunkMotor_t[4].cmd.K_P = 0;
                unitree_DunkMotor_t[4].cmd.K_W = 0;
                unitree_DunkMotor_t[4].cmd.W = 0;
                unitree_DunkMotor_t[4].cmd.T = 0;

                /*unitree_DunkMotor_t[4].cmd.Pos = 0;
                unitree_DunkMotor_t[4].cmd.K_P = 0;
                unitree_DunkMotor_t[4].cmd.K_W = 0.2;
                unitree_DunkMotor_t[4].cmd.W = -0.1;*/
                //unitree_DunkMotor_t[3].cmd.T = - 0.6;
            }

            if(my_Shoot_Task_T.Shoot_Completed_Flag == 3&&
              (encoderData.angle <= Encoder_VertPos||encoderData.angle >= 300))
            {
                my_Shoot_Task_T.myshoot_status = SHOOT_IDLE;
                unitree_DunkMotor_t[3].cmd.Pos = 0;
                unitree_DunkMotor_t[3].cmd.K_P = 0;
                unitree_DunkMotor_t[3].cmd.K_W = 0;
                unitree_DunkMotor_t[3].cmd.W = 0;
                unitree_DunkMotor_t[3].cmd.T = 0;

                unitree_DunkMotor_t[4].cmd.Pos = 0;
                unitree_DunkMotor_t[4].cmd.K_P = 0;
                unitree_DunkMotor_t[4].cmd.K_W = 0;
                unitree_DunkMotor_t[4].cmd.W = 0;
                unitree_DunkMotor_t[4].cmd.T = 0;
            }
            osDelay(1);
            break;
        case SHOOT_FINISH :
            my_Shoot_Task_T.myshoot_status = SHOOT_IDLE;
            break;    
        default:
            break;
        }

        osDelay(2);
    }
    

}