/**
 * @file Handle_task.c
 * @author Lary (you@domain.com)
 * @brief  WTR2025扣篮车一代版本工程
 * @note   手动模式线程封装  
 * @version 0.1
 * @date 2024-11-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Handle_task.h"

enum Handle_State myHandle_State;   //手动模式状态

/*****************************************************
 * @brief 手动模式线程定义
 * 
 */
osThreadId_t handle_TaskHandle;
const osThreadAttr_t handle_Task_attributes = {
    .name       = "handle_Task",
    .stack_size = 256*4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void my_handle_Task(void *arguement);


/*****************************************************
 * @brief 手动模式线程初始化（外部接口函数）
 * 
 */
void my_handle_Task_Start(void)
{
    myHandle_State      = HANDLE_IDLE_MODE;
    handle_TaskHandle   = osThreadNew(my_handle_Task,NULL,&handle_Task_attributes);
}

/*****************************************************
 * @brief 手动模式线程函数实现
 * 
 */
void my_handle_Task(void *arguement)
{
    //Handle_Dunk_TaskStart();
    Handle_Shoot_TaskStart();

    //动作线程启动
    myHandle_State = HANDLE_IDLE_MODE;
    for(;;)
    {
        /*if (MyRemote_Data.btn_Btn0 == 1 )
        {
            if (myHandle_State == HANDLE_IDLE_MODE)
            {
                myHandle_State = HANDLE_DUNK_MODE;
                
            }
        }
        if (MyRemote_Data.btn_Btn1 == 1 )
        {
            if (myHandle_State == HANDLE_DUNK_MODE)
            {
                myHandle_State = HANDLE_IDLE_MODE;
                
            }
        }*/

        switch ((int)MyRemote_Data.usr_right_knob)
        {
        case 0:
            myHandle_State = HANDLE_IDLE_MODE;
            break;
        case 4:
            myHandle_State = HANDLE_DUNK_MODE;
            break;
        case 8:
            myHandle_State = HANDLE_SHOOT_MODE;
            break;
        default:
            break;
        }
        
        switch (myHandle_State)
        {
        case HANDLE_IDLE_MODE:
            //osThreadSuspend(unitree_shoot_ctrl_TaskHandle);
            osThreadSuspend(unitree_dunk_ctrl_TaskHandle);
            JoystickDelete(ID_HANDLE_DUNK, &mav_joystick_del);
            JoystickDelete(ID_HANDLE_SHOOT, &mav_joystick_del);
            JoystickSwitchTitle(ID_MODE, mode_title, &mav_mode_title);
            JoystickSwitchMsg(ID_MODE, mode_idle_msg, &mav_mode_msg);
            break;
        case HANDLE_DUNK_MODE:
            osThreadSuspend(unitree_shoot_ctrl_TaskHandle);
            osThreadResume(unitree_dunk_ctrl_TaskHandle);
            JoystickSwitchTitle(ID_MODE, mode_title, &mav_mode_title);
            JoystickSwitchMsg(ID_MODE, mode_dunk_msg, &mav_mode_msg);
            break;
        case HANDLE_INTERCEPT_MODE:
            break;
        case HANDLE_PATBALL_MODE:
            break;
        case HANDLE_SHOOT_MODE:
            osThreadSuspend(unitree_dunk_ctrl_TaskHandle);
            osThreadResume(unitree_shoot_ctrl_TaskHandle);
            JoystickSwitchTitle(ID_MODE, mode_title, &mav_mode_title);
            JoystickSwitchMsg(ID_MODE, mode_shoot_msg, &mav_mode_msg);
            break;
        default:
            break;
        }
        static int i = 0;
        i++;
        if (i == 1000) {
            i = 0;
            HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
        }
        osDelay(1);
    }
}