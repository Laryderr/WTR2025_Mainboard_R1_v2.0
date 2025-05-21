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
 * @brief 手动模式线程函数实现
 * 
 */
void my_handle_Task(void)
{
    if (BtnScan_Press(MyRemote_Data.btn_Btn0))
    {
        myHandle_State = HANDLE_IDLE_MODE;
    }else if (BtnScan_Press(MyRemote_Data.btn_Btn1))
    {
        myHandle_State = HANDLE_DUNK_MODE;   
    }else if (BtnScan_Press(MyRemote_Data.btn_Btn4))
    {
        myHandle_State = HANDLE_SHOOT_MODE;
    }else if (BtnScan_Press(MyRemote_Data.btn_Btn5))
    {
        myHandle_State = HANDLE_PATBALL_MODE;
    }

    switch (myHandle_State)
    {
    case HANDLE_IDLE_MODE:
        /*Expansion_Up.contract();
        Expansion_Down.contract();*/
        osThreadSuspend(unitree_shoot_ctrl_TaskHandle);
        //osThreadSuspend(unitree_dunk_ctrl_TaskHandle);
        osThreadSuspend(Patball_TaskHandle);
        JoystickDelete(ID_HANDLE_DUNK, &mav_joystick_del);
        JoystickDelete(ID_HANDLE_SHOOT, &mav_joystick_del);
        JoystickSwitchTitle(ID_MODE, mode_title, &mav_mode_title);
        JoystickSwitchMsg(ID_MODE, mode_idle_msg, &mav_mode_msg);
        osDelay(1);
        break;
    case HANDLE_DUNK_MODE:
        osThreadSuspend(unitree_shoot_ctrl_TaskHandle);
        osThreadSuspend(Patball_TaskHandle);
        osThreadResume(unitree_dunk_ctrl_TaskHandle);
        JoystickSwitchTitle(ID_MODE, mode_title, &mav_mode_title);
        JoystickSwitchMsg(ID_MODE, mode_dunk_msg, &mav_mode_msg);
        osDelay(1);
        break;
    case HANDLE_INTERCEPT_MODE:
        break;
    case HANDLE_PATBALL_MODE:
        osThreadSuspend(unitree_shoot_ctrl_TaskHandle);
        osThreadSuspend(unitree_dunk_ctrl_TaskHandle);
        osThreadResume(Patball_TaskHandle);
        JoystickSwitchTitle(ID_MODE, mode_title, &mav_mode_title);
        JoystickSwitchMsg(ID_MODE, mode_patball_msg, &mav_mode_msg);
        osDelay(1);
        break;
    case HANDLE_SHOOT_MODE:
        osThreadSuspend(unitree_dunk_ctrl_TaskHandle);
        osThreadSuspend(Patball_TaskHandle);
        osThreadResume(unitree_shoot_ctrl_TaskHandle);
        JoystickSwitchTitle(ID_MODE, mode_title, &mav_mode_title);
        JoystickSwitchMsg(ID_MODE, mode_shoot_msg, &mav_mode_msg);
        osDelay(1);
        break;
    default:
        break;
    }
    osDelay(1);
}