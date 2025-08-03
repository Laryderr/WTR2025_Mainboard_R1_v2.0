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
        // myHandle_State = HANDLE_IDLE_MODE;
    }else if (BtnScan_Press(MyRemote_Data.btn_Btn1))
    {
        myHandle_State = HANDLE_DUNK_MODE;   
    }else if (BtnScan_Press(MyRemote_Data.btn_Btn4)
            ||BtnScan_Press(MyRemote_Data.btn_LeftCrossLeft)||BtnScan_Press(MyRemote_Data.btn_LeftCrossMid)
            ||BtnScan_Press(MyRemote_Data.btn_LeftCrossUp)||BtnScan_Press(MyRemote_Data.btn_LeftCrossDown)
            ||BtnScan_Press(MyRemote_Data.btn_RightCrossMid)||BtnScan_Press(MyRemote_Data.btn_LeftCrossRight)
            ||BtnScan_Press(MyRemote_Data.btn_Btn0)||BtnScan_Press(MyRemote_Data.btn_Btn1)
            ||BtnScan_Press(MyRemote_Data.btn_JoystickR))
    {
        myHandle_State = HANDLE_SHOOT_MODE;
    }else if (BtnScan_Press(MyRemote_Data.btn_Btn5))
    {
        myHandle_State = HANDLE_PATBALL_MODE;
    }
    if(MyRemote_Data.left_switch == 0)
    {
        myHandle_State = HANDLE_IDLE_MODE;
    }

    //模型训练
    my_Auto_Shoot_Task_T.now_train_distance = MIN_TRAIN_DISTANCE + MyRemote_Data.btn_LeftCrossRight_press_count*my_Auto_Shoot_Task_T.train_step - MyRemote_Data.btn_LeftCrossLeft_press_count*my_Auto_Shoot_Task_T.train_step;
    my_Auto_Shoot_Task_T.train_point[0] = BASKET_X - sin(PI/4)*my_Auto_Shoot_Task_T.now_train_distance;
    my_Auto_Shoot_Task_T.train_point[1] = BASKET_Y + sin(PI/4)*my_Auto_Shoot_Task_T.now_train_distance;
    my_Alldir_Chassis_t.test_KW = my_Auto_Shoot_Task_T.min_KW + MyRemote_Data.KW01 + MyRemote_Data.KW001 +
                                   MyRemote_Data.KW0001 + MyRemote_Data.KW00001 + MyRemote_Data.KW_00001;
    my_Alldir_Chassis_t.shoot_angle = 66 + MyRemote_Data.btn_RightCrossRight_press_count*0.75;
    
    //模型预测
    // my_Shoot_Task_T.model_calc_KW     = Calc_KW(2.7);
    // my_Shoot_Task_T.model_calc_degree = Calc_Degree(1.65);
    JoystickSwitchTitle(ID_SHOOT_READY, shoot_ready_title, &mav_shoot_ready_title);
    if(my_Shoot_Task_T.on_shoot_point == 1)
    {
        JoystickSwitchMsg(ID_SHOOT_READY, shoot_ready_msg, &mav_shoot_ready_msg);
    }else JoystickSwitchMsg(ID_SHOOT_READY, shoot_notready_msg, &mav_shoot_ready_msg);


    switch (myHandle_State)
    {
    case HANDLE_IDLE_MODE:
        /*Expansion_Up.contract();
        Expansion_Down.contract();*/
        osThreadSuspend(unitree_shoot_ctrl_TaskHandle);
        osThreadSuspend(unitree_dunk_ctrl_TaskHandle);
        osThreadSuspend(Patball_TaskHandle);
        JoystickDelete(ID_HANDLE_DUNK, &mav_joystick_del);
        JoystickDelete(ID_HANDLE_SHOOT, &mav_joystick_del);
        JoystickSwitchTitle(ID_MODE, mode_title, &mav_mode_title);
        JoystickSwitchMsg(ID_MODE, mode_idle_msg, &mav_mode_msg);

        if(my_Shoot_Task_T.shoot_point == 0)
        {
            JoystickSwitchMsg(ID_SHOOT_POINT, shoot_point_msg0, &mav_shoot_point_msg);
        }
        
        my_Alldir_Chassis_t.now_mark_flag = 0;
        my_Alldir_Chassis_t.Now_marked_pos.xpos = 0;
        my_Alldir_Chassis_t.Now_marked_pos.ypos = 0;

        osDelay(1);
        break;
    case HANDLE_DUNK_MODE:
        osThreadSuspend(unitree_shoot_ctrl_TaskHandle);
        osThreadSuspend(Patball_TaskHandle);
        osThreadResume(unitree_dunk_ctrl_TaskHandle);
        JoystickSwitchTitle(ID_MODE, mode_title, &mav_mode_title);
        JoystickSwitchMsg(ID_MODE, mode_dunk_msg, &mav_mode_msg);
        //自动定位 篮筐瞄准
        my_Alldir_Chassis_t.state = CHASSIS_AUTO_RUNNING;
        Now_Pose_Servo();
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

        JoystickSwitchTitle(ID_SHOOT_POINT, shoot_point_title, &mav_shoot_point_title);
    
        switch (my_Shoot_Task_T.shoot_point)
        {
        case 0:
            JoystickSwitchMsg(ID_SHOOT_POINT, shoot_point_msg0, &mav_shoot_point_msg);
            break;
        case 1:
            JoystickSwitchMsg(ID_SHOOT_POINT, shoot_point_msg1, &mav_shoot_point_msg);
            break;
        case 2:
            JoystickSwitchMsg(ID_SHOOT_POINT, shoot_point_msg2, &mav_shoot_point_msg);
            break;
        case 3:
            JoystickSwitchMsg(ID_SHOOT_POINT, shoot_point_msg3, &mav_shoot_point_msg);
            break;
        case 4:
            JoystickSwitchMsg(ID_SHOOT_POINT, shoot_point_msg4, &mav_shoot_point_msg);
            break;
        case 5:
            JoystickSwitchMsg(ID_SHOOT_POINT, shoot_point_msg5, &mav_shoot_point_msg);
            break;
        case 6:
            JoystickSwitchMsg(ID_SHOOT_POINT, shoot_point_msg6, &mav_shoot_point_msg);
            break;
        case 7:
            JoystickSwitchMsg(ID_SHOOT_POINT, shoot_point_msg7, &mav_shoot_point_msg);
            break;
        
        default:
            break;
        }

        
        //自动定位 篮筐瞄准
        my_Alldir_Chassis_t.state = CHASSIS_AUTO_RUNNING;
        //Now_Pose_Servo();
        //my_Alldir_Chassis_t.YAWPosServo(FORWARD_ANGLE);
        osDelay(1);
        break;
    default:
        break;
    }
    osDelay(1);
}