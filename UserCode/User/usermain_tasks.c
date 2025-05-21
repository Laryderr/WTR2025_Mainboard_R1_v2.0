/**
 * @file usermain_tasks.c
 * @author Lary (you@domain.com)
 * @brief WTR2025 RX 扣篮一代车版本工程
 * @note  整车总状态机，总运行线程
 * @version 0.1
 * @date 2024-11-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "usermain_tasks.h"

enum Running_Status running_status;
//enum Handle_State myHandle_State;   //手动模式状态

/******************************************************
 * @brief 定义总线程
 * 
 */
osThreadId_t main_TaskHandle;
const osThreadAttr_t main_Task_attributes = {
    .name       = "main_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void my_main_Task(void *argument);



/**
 * @brief 总线程初始化（接口函数）
 * 
 */
void my_main_Task_Start(void)
{
    running_status       = HANDLE_MODE;
    main_TaskHandle = osThreadNew(my_main_Task, NULL, &main_Task_attributes);
}

/**
 * @brief 辅助函数
 * 
 */
int motor_wait_to_finish(float feedback, float setpoint)
{
    if (((feedback - setpoint) < -5.0f) || ((feedback - setpoint) > 5.0f)) {
        return 0;
    } else {
        return 1;
    }
}


/*****************************************************
 * @brief 线程函数定义
 * 
 */

/**
 * @brief 总线程实现
 * 
 */
void my_main_Task(void *arguement)
{
    //my_handle_Task_Start();
    myHandle_State = HANDLE_IDLE_MODE;

    for(;;)
    {
        //发送遥控器状态显示面框

         //判断遥控器底盘指令
        switch ((int)MyRemote_Data.usr_right_knob)
        {
        case 0:
        my_Alldir_Chassis_t.state = CHASSIS_STOP;
            break;
        case 4:
        my_Alldir_Chassis_t.state = CHASSIS_HANDLE_RUNNING;
            break;
        case -4:
        my_Alldir_Chassis_t.state = CHASSIS_AUTO_RUNNING;
            break;
        
        default:
            break;
        }
        
        if(MyRemote_Data.left_switch == 0 && my_Alldir_Chassis_t.state != CHASSIS_READY)
        {
            my_Alldir_Chassis_t.state = CHASSIS_STOP;
            for (uint8_t i = 0; i < 3; i++)
            {
                my_Alldir_Chassis_t.my_wheel[i].state = WHEEL_STOP;
            }
        }

        //底盘状态
        switch (my_Alldir_Chassis_t.state)
        {
        case CHASSIS_READY:

            // Speed & Position Init
            my_Alldir_Chassis_t.target_v.vx = 0;
            my_Alldir_Chassis_t.target_v.vy = 0;
            my_Alldir_Chassis_t.target_v.vw = 0;

            for (uint8_t i = 0; i < 3; i++)
            {
                my_Alldir_Chassis_t.my_wheel[i].state = WHEEL_READY;
            }
            if(MyRemote_Data.right_switch == 1)
            {
                my_Alldir_Chassis_t.state = CHASSIS_STOP;
            }
            //显示当前状态
            JoystickSwitchTitle(ID_STATUS, status_title, &mav_status_title);
            JoystickSwitchMsg(ID_STATUS, status_ready_msg, &mav_status_msg);

            osDelay(2);
            break;
        case CHASSIS_HANDLE_RUNNING:
            //osThreadSuspend(auto_TaskHandle);
            //osThreadResume(handle_TaskHandle);
            JoystickSwitchTitle(ID_RUN, run_title, &mav_run_title);
            JoystickSwitchMsg(ID_RUN, run_handle_msg, &mav_run_msg);

            JoystickSwitchTitle(ID_STATUS, status_title, &mav_status_title);
            JoystickSwitchMsg(ID_STATUS, status_running_msg, &mav_status_msg);
            osDelay(2);
            break;
        case CHASSIS_AUTO_RUNNING :
            JoystickSwitchTitle(ID_RUN, run_title, &mav_run_title);
            JoystickSwitchMsg(ID_RUN, run_auto_msg, &mav_run_msg);

            JoystickSwitchTitle(ID_STATUS, status_title, &mav_status_title);
            JoystickSwitchMsg(ID_STATUS, status_running_msg, &mav_status_msg);
            break;
        case CHASSIS_STOP :
            my_Alldir_Chassis_t.target_v.vx = 0;
            my_Alldir_Chassis_t.target_v.vy = 0;
            my_Alldir_Chassis_t.target_v.vw = 0;
            JoystickSwitchTitle(ID_RUN, run_title, &mav_run_title);
            JoystickSwitchMsg(ID_RUN, run_stop_msg, &mav_run_msg);
            
            JoystickSwitchTitle(ID_STATUS, status_title, &mav_status_title);
            JoystickSwitchMsg(ID_STATUS, status_stop_msg, &mav_status_msg);
            osDelay(2);
            break;
        case CHASSIS_ERROR:
            my_Alldir_Chassis_t.target_v.vx = 0;
            my_Alldir_Chassis_t.target_v.vy = 0;
            my_Alldir_Chassis_t.target_v.vw = 0;

            JoystickSwitchTitle(ID_STATUS, status_title, &mav_status_title);
            JoystickSwitchMsg(ID_STATUS, status_error_msg, &mav_status_msg);
            osDelay(2);
            break;
        default:
            break;
        }

        my_handle_Task(); //循环手动线程函数
        osDelay(1);
    }
}