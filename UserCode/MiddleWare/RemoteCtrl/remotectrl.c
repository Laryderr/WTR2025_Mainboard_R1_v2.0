/**
 * @file remotectrl.c
 * @author Lary (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "remotectrl.h"

Remote_Data MyRemote_Data;      
Remote_Data MyLastRemote_Data;  

/**********************************************************
 * @brief 定义遥控器控制线程
 * @note 注意遥控器线程优先级要较高
 * 
 */
osThreadId_t remotectrl_TaskHandle;
const osThreadAttr_t remotectrl_Task_attributes ={
    .name       = "remotectrl_Task",
    .stack_size = 128 * 4,
    .priority   =(osPriority_t)osPriorityNormal,
};
void my_Remotectrl_Task(void *arguement);


/*****************************************************************************
 * @brief 以下是接口函数定义
 * 
 */
 

/**
 * @brief 初始化遥控器（接口函数）
 * 
 */
void my_RemoteCtrl_Init(void)
{
    AS69_RemoteControl_Init();  //形式上封装
}

/**
 * @brief 遥控器控制线程创建（接口函数）
 * 
 */
void my_RemoteCtrl_Task_Start(void)
{
    remotectrl_TaskHandle = osThreadNew(my_Remotectrl_Task,NULL,&remotectrl_Task_attributes);
}

/**
 * @brief 检测按键是否按下
 * 
 * @param this_status 
 * @param last_status 
 * @return true 
 * @return false 
 */
bool BtnScan_Press(bool this_status,bool last_status)
{
    if(this_status == 1 && last_status == 0)
    {
        osDelay(400);
        return true;
    }
    return false;
}


/*****************************************************************************
 * @brief 以下是线程函数定义
 * 
 */

/**
 * @brief 遥控器控制接收线程实现
 * 
 */
void my_Remotectrl_Task(void *arguement)
{
    static float knob_offset[2];
    for (int i = 0; i < 10; i++) {
        MyRemote_Data.left_knob = ReadJoystickKnobsLeft(&msg_joystick_air);
        MyRemote_Data.right_knob = ReadJoystickKnobsRight(&msg_joystick_air);
        osDelay(5);
    }
    knob_offset[0] = MyRemote_Data.left_knob;
    knob_offset[1] = MyRemote_Data.right_knob;
    for (;;) {
        //数据处理
        MyLastRemote_Data.btn_LeftCrossUp     = MyRemote_Data.btn_LeftCrossUp;
        MyLastRemote_Data.btn_LeftCrossDown   = MyRemote_Data.btn_LeftCrossDown;
        MyLastRemote_Data.btn_LeftCrossLeft   = MyRemote_Data.btn_LeftCrossLeft;
        MyLastRemote_Data.btn_LeftCrossRight  = MyRemote_Data.btn_LeftCrossRight;
        MyLastRemote_Data.btn_LeftCrossMid    = MyRemote_Data.btn_LeftCrossMid;
        MyLastRemote_Data.btn_RightCrossUp    = MyRemote_Data.btn_RightCrossUp;
        MyLastRemote_Data.btn_RightCrossDown  = MyRemote_Data.btn_RightCrossDown;
        MyLastRemote_Data.btn_RightCrossLeft  = MyRemote_Data.btn_RightCrossLeft;
        MyLastRemote_Data.btn_RightCrossRight = MyRemote_Data.btn_RightCrossRight;
        MyLastRemote_Data.btn_RightCrossMid   = MyRemote_Data.btn_RightCrossMid;
        MyLastRemote_Data.btn_Btn0            = MyRemote_Data.btn_Btn0;
        MyLastRemote_Data.btn_Btn1            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn1);
        MyLastRemote_Data.btn_Btn2            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn2);
        MyLastRemote_Data.btn_Btn3            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn3);
        MyLastRemote_Data.btn_Btn4            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn4);
        MyLastRemote_Data.btn_Btn5            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn5);
        MyLastRemote_Data.btn_JoystickL       = ReadJoystickButtons(&msg_joystick_air, Btn_JoystickL);
        MyLastRemote_Data.btn_JoystickR       = ReadJoystickButtons(&msg_joystick_air, Btn_JoystickR);
        MyLastRemote_Data.btn_KnobL           = ReadJoystickButtons(&msg_joystick_air, Btn_KnobL);
        MyLastRemote_Data.btn_KnobR           = ReadJoystickButtons(&msg_joystick_air, Btn_KnobR);
        MyLastRemote_Data.left_knob           = ReadJoystickKnobsLeft(&msg_joystick_air);
        MyLastRemote_Data.right_knob          = ReadJoystickKnobsRight(&msg_joystick_air);

        MyRemote_Data.btn_LeftCrossUp     = ReadJoystickButtons(&msg_joystick_air, Btn_LeftCrossUp);
        MyRemote_Data.btn_LeftCrossDown   = ReadJoystickButtons(&msg_joystick_air, Btn_LeftCrossDown);
        MyRemote_Data.btn_LeftCrossLeft   = ReadJoystickButtons(&msg_joystick_air, Btn_LeftCrossLeft);
        MyRemote_Data.btn_LeftCrossRight  = ReadJoystickButtons(&msg_joystick_air, Btn_LeftCrossRight);
        MyRemote_Data.btn_LeftCrossMid    = ReadJoystickButtons(&msg_joystick_air, Btn_LeftCrossMid);
        MyRemote_Data.btn_RightCrossUp    = ReadJoystickButtons(&msg_joystick_air, Btn_RightCrossUp);
        MyRemote_Data.btn_RightCrossDown  = ReadJoystickButtons(&msg_joystick_air, Btn_RightCrossDown);
        MyRemote_Data.btn_RightCrossLeft  = ReadJoystickButtons(&msg_joystick_air, Btn_RightCrossLeft);
        MyRemote_Data.btn_RightCrossRight = ReadJoystickButtons(&msg_joystick_air, Btn_RightCrossRight);
        MyRemote_Data.btn_RightCrossMid   = ReadJoystickButtons(&msg_joystick_air, Btn_RightCrossMid);
        MyRemote_Data.btn_Btn0            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn0);
        MyRemote_Data.btn_Btn1            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn1);
        MyRemote_Data.btn_Btn2            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn2);
        MyRemote_Data.btn_Btn3            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn3);
        MyRemote_Data.btn_Btn4            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn4);
        MyRemote_Data.btn_Btn5            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn5);
        MyRemote_Data.btn_JoystickL       = ReadJoystickButtons(&msg_joystick_air, Btn_JoystickL);
        MyRemote_Data.btn_JoystickR       = ReadJoystickButtons(&msg_joystick_air, Btn_JoystickR);
        MyRemote_Data.btn_KnobL           = ReadJoystickButtons(&msg_joystick_air, Btn_KnobL);
        MyRemote_Data.btn_KnobR           = ReadJoystickButtons(&msg_joystick_air, Btn_KnobR);
        MyRemote_Data.left_switch         = ReadJoystickSwitchs(&msg_joystick_air, Left_switch);
        MyRemote_Data.right_switch        = ReadJoystickSwitchs(&msg_joystick_air, Right_switch);
        MyRemote_Data.right_x             = ReadJoystickRight_x(&msg_joystick_air);
        MyRemote_Data.right_y             = ReadJoystickRight_y(&msg_joystick_air);
        MyRemote_Data.left_x              = ReadJoystickLeft_x(&msg_joystick_air);
        MyRemote_Data.left_y              = ReadJoystickLeft_y(&msg_joystick_air);
        MyRemote_Data.left_knob           = ReadJoystickKnobsLeft(&msg_joystick_air);
        MyRemote_Data.right_knob           = ReadJoystickKnobsRight(&msg_joystick_air);

        if (MyRemote_Data.right_x < 0.2f && MyRemote_Data.right_x > -0.2f) {
            MyRemote_Data.usr_right_x = 0;
        } else if (MyRemote_Data.right_x > 0.2f) {
            MyRemote_Data.usr_right_x = (int)((MyRemote_Data.right_x - 0.2) * 1000);
        } else {
            MyRemote_Data.usr_right_x = (int)((MyRemote_Data.right_x + 0.2) * 1000);
        }

        if (MyRemote_Data.right_y < 0.15f && MyRemote_Data.right_y > -0.15f) {
            MyRemote_Data.usr_right_y = 0;
        } else if (MyRemote_Data.right_y > 0.15f) {
            MyRemote_Data.usr_right_y = (int)((MyRemote_Data.right_y - 0.15) * 1000);
        } else {
            MyRemote_Data.usr_right_y = (int)((MyRemote_Data.right_y + 0.15) * 1000);
        }

        if (MyRemote_Data.left_x < 0.15f && MyRemote_Data.left_x > -0.15f) {
            MyRemote_Data.usr_left_x = 0;
        } else if (MyRemote_Data.left_x > 0.15f) {
            MyRemote_Data.usr_left_x = (int)((MyRemote_Data.left_x - 0.15) * 1000);
        } else {
            MyRemote_Data.usr_left_x = (int)((MyRemote_Data.left_x + 0.15) * 1000);
        }

        if (MyRemote_Data.left_y < 0.15f && MyRemote_Data.left_y > -0.15f) {
            MyRemote_Data.usr_left_y = 0;
        } else if (MyRemote_Data.left_y > 0.15f) {
            MyRemote_Data.usr_left_y = (int)((MyRemote_Data.left_y - 0.15) * 1000);
        } else {
            MyRemote_Data.usr_left_y = (int)((MyRemote_Data.left_y + 0.15) * 1000);
        }
        MyLastRemote_Data.usr_right_knob = MyRemote_Data.usr_right_knob;
        MyRemote_Data.usr_left_knob = MyRemote_Data.left_knob - knob_offset[0];
        MyRemote_Data.usr_right_knob = MyRemote_Data.right_knob - knob_offset[1];

        //特殊处理
        if(MyRemote_Data.usr_right_knob >= 12)
        {
            MyRemote_Data.usr_right_knob = 0;
        }        

        JoystickSwitchLED(1,0.5,0,10,2000,&msg_joystick_air_led);
        JoystickSwitchTitle(ID_RUN, run_title, &mav_run_title);
        osDelay(1);
    }
}