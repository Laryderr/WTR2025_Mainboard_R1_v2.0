/**
 * @file userdebug.c
 * @author Lary (you@domain.com)
 * @brief  调车用
 * @version 0.1
 * @date 2024-11-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "userdebug.h"


/**********************************************************
 * @brief 线程定义
 * 
 */
osThreadId_t user_debug_TaskHandle;
const osThreadAttr_t user_debug_Task_attributes = {
    .name       = " user_debug_Task",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void my_debug_Task(void *argument);


/**************************************************************
 * @brief   调试启动线程
 */
void my_debug_TaskStart(void)
{
    user_debug_TaskHandle = osThreadNew(my_debug_Task, NULL, &user_debug_Task_attributes);
}


/**
 * @brief 调试线程实现
 * 
 * @param argument 
 */
void my_debug_Task(void *argument)
{
    for(; ;)
    {
        //调试底盘3508PID
        if(BtnScan_Press(MyRemote_Data.btn_LeftCrossUp,MyLastRemote_Data.btn_LeftCrossUp))
        {
            for (int i = 0; i < 4; i++) {
            hDJI[i].speedPID.KP = hDJI[i].speedPID.KP + 0.05;
            }    
        }

        if(BtnScan_Press(MyRemote_Data.btn_LeftCrossMid,MyLastRemote_Data.btn_LeftCrossMid))
        {
            for (int i = 0; i < 4; i++) {
            hDJI[i].speedPID.KI = hDJI[i].speedPID.KI + 0.01;
            }    
        }

        if(BtnScan_Press(MyRemote_Data.btn_LeftCrossDown,MyLastRemote_Data.btn_LeftCrossDown))
        {
            for (int i = 0; i < 4; i++) {
            hDJI[i].speedPID.KD = hDJI[i].speedPID.KD + 0.05;
            }    
        }

        if (BtnScan_Press(MyRemote_Data.btn_RightCrossUp,MyLastRemote_Data.btn_RightCrossUp))
        {
            for (int i = 0; i < 4; i++) {
            hDJI[i].speedPID.KP = hDJI[i].speedPID.KP - 0.05;
            }   
        }

        if (BtnScan_Press(MyRemote_Data.btn_RightCrossMid,MyLastRemote_Data.btn_RightCrossMid))
        {
            for (int i = 0; i < 4; i++) {
            hDJI[i].speedPID.KI = hDJI[i].speedPID.KI - 0.01;
            }   
        }

        if (BtnScan_Press(MyRemote_Data.btn_RightCrossDown,MyLastRemote_Data.btn_RightCrossDown))
        {
            for (int i = 0; i < 4; i++) {
            hDJI[i].speedPID.KD= hDJI[i].speedPID.KD - 0.05;
            }   
        }

        //调试宇树PD
        if (BtnScan_Press(MyRemote_Data.btn_LeftCrossLeft,MyLastRemote_Data.btn_LeftCrossLeft))
        {
            for (uint8_t i = 0; i <3; i++)
            {
                unitree_DunkMotor_t[i].cmd.K_P = unitree_DunkMotor_t[i].cmd.K_P + 0.02f;
            }
        }

        if (BtnScan_Press(MyRemote_Data.btn_LeftCrossRight,MyLastRemote_Data.btn_LeftCrossRight))
        {
            for (uint8_t i = 0; i <3; i++)
            {
                unitree_DunkMotor_t[i].cmd.K_W = unitree_DunkMotor_t[i].cmd.K_W + 0.02f;
            }
        }

        if (BtnScan_Press(MyRemote_Data.btn_RightCrossLeft,MyLastRemote_Data.btn_RightCrossLeft))
        {
            for (uint8_t i = 0; i <3; i++)
            {
                unitree_DunkMotor_t[i].cmd.K_P = unitree_DunkMotor_t[i].cmd.K_P - 0.02f;
            }
        }

        if (BtnScan_Press(MyRemote_Data.btn_RightCrossRight,MyLastRemote_Data.btn_RightCrossRight))
        {
            for (uint8_t i = 0; i <3; i++)
            {
                unitree_DunkMotor_t[i].cmd.K_W = unitree_DunkMotor_t[i].cmd.K_W - 0.02f;
            }
        }

    }
}