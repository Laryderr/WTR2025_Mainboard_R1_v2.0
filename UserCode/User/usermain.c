/**
 * @file usermain.c
 * @author Lary (you@domain.com)
 * @brief  WTR2025 RX 扣篮一代车版本工程
 * @note   总初始化线程
 * @version 0.1
 * @date 2024-11-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "usermain.h"

char debug_title[20] = "Debug";
char debug_msg[20];
mavlink_joystick_air_dashboard_set_title_t mav_debug_title;
mavlink_joystick_air_dashboard_set_msg_t mav_debug_msg;
uint8_t rst_flag = 0;


void StartDefaultTask(void *arguement)
{
    rst_flag = 0;
    //  Hardware Init
    my_Chassis_Init();      //底盘运动电机初始化
    my_RemoteCtrl_Init();   //遥控器初始化
    my_Unitree_Init();    //宇树电机初始化
    osDelay(100);

    //  Tasks Start
    my_RemoteCtrl_Task_Start();         //开启遥控器线程
    my_Chassis_CAN_Message_TaskStart(); //开启底盘电机消息线程
    my_Chassis_Ctrl_TaskStart();        //开启底盘控制线程
    my_Unitree_UART_Message_TaskStart();//开启跳跃电机消息线程
    osDelay(200);
    my_handle_Task_Start();             //开启手动控制线程
    //Handle_Dunk_TaskStart();            //开启跳跃电机控制线程
    //my_debug_TaskStart();               //调试线程
    



    osDelay(100);

    // entry chassis ready state
    my_Alldir_Chassis_t.state = CHASSIS_READY;

    // main task start!
    my_main_Task_Start();



    static int i = 0;
    for (;;) {
        // Run State
        i++;
        if (i == 500) {
            i = 0;
            HAL_GPIO_TogglePin(LED_Green_GPIO_Port, LED_Green_Pin);
        }
        //sprintf(debug_msg, "yaw:%d,seed:%d", (int)(chassis_yaw - chassis_offset), seed_count);
        //JoystickSwitchTitle(10, debug_title, &mav_debug_title);
        //JoystickSwitchMsg(10, debug_msg, &mav_dir_choose_msg);
        osDelay(1);
    }
}