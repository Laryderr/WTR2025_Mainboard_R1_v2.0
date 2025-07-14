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
    }else if (BtnScan_Press(MyRemote_Data.btn_Btn4)||BtnScan_Press(MyRemote_Data.btn_RightCrossUp)
              ||BtnScan_Press(MyRemote_Data.btn_RightCrossMid)||BtnScan_Press(MyRemote_Data.btn_RightCrossDown))
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

    /*float_to_char(my_Alldir_Chassis_t.current_pos.xpos,posx_msg,2);
    JoystickSwitchTitle(ID_POSITIONX, posx_title, &mav_posx_title);
    JoystickSwitchMsg(ID_POSITIONX, posx_msg, &mav_posx_msg);

    float_to_char(my_Alldir_Chassis_t.current_pos.ypos,posy_msg,2);
    JoystickSwitchTitle(ID_POSITIONY, posy_title, &mav_posy_title);
    JoystickSwitchMsg(ID_POSITIONY, posy_msg, &mav_posy_msg);*/
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
        if (BtnScan_Press(MyRemote_Data.btn_RightCrossLeft))
        {
            my_Shoot_Task_T.camera_aim_flag = 1;
        }
        if (BtnScan_Press(MyRemote_Data.btn_RightCrossRight))
        {
            my_Shoot_Task_T.camera_aim_flag = 0;
        }
        
        
        //↓测试用
        if(BtnScan_Press(MyRemote_Data.btn_RightCrossUp))
        {
            my_Shoot_Task_T.shoot_point = 1;
        }else if (BtnScan_Press(MyRemote_Data.btn_RightCrossMid))
        {
            my_Shoot_Task_T.shoot_point = 2;
        }else if (BtnScan_Press(MyRemote_Data.btn_RightCrossDown))
        {
            my_Shoot_Task_T.shoot_point = 3;
        }else if (BtnScan_Press(MyRemote_Data.btn_Btn2))
        {
            my_Shoot_Task_T.shoot_point = 4;
        }
        switch (my_Shoot_Task_T.shoot_point)
        {
        case 1:
            chassis_XYPoseServo_calc(2.05,3.32);
            
            if (my_Shoot_Task_T.camera_aim_flag == 1)
            {
                //精瞄
                my_Alldir_Chassis_t.chassis_Aim_at_Basket(0.05);
            }else Chassis_Pre_Aim(); //育苗
            //my_Alldir_Chassis_t.chassis_Aim_at_Basket(0.05);

            break;
        case 2:
            chassis_XYPoseServo_calc(4.77,3.35);
            
            if (my_Shoot_Task_T.camera_aim_flag == 1)
            {
                //精瞄
                my_Alldir_Chassis_t.chassis_Aim_at_Basket(0.05);
            }else Chassis_Pre_Aim(); //育苗
            //my_Alldir_Chassis_t.chassis_Aim_at_Basket(0.05);
            break;
        case 3:
            chassis_XYPoseServo_calc(BASKET_X,2.9);
            
            if (my_Shoot_Task_T.camera_aim_flag == 1)
            {
                //精瞄
                /*if (camera_basket_xyz[0]!=0.02&&my_Shoot_Task_T.camera_get_angle_flag == 0)
                {
                    my_Alldir_Chassis_t.chassis_Aim_at_Basket(0.02);
                }else if(camera_basket_xyz[0] == 0.02) 
                {
                    my_Shoot_Task_T.camera_aim_angle = my_Alldir_Chassis_t.current_pos.yawpos;
                    my_Shoot_Task_T.camera_get_angle_flag = 1;
                }
                if(my_Shoot_Task_T.camera_get_angle_flag==1){
                    my_Alldir_Chassis_t.YAWPosServo(my_Shoot_Task_T.camera_aim_angle);
                }*/
                my_Alldir_Chassis_t.chassis_Aim_at_Basket(0.055);
                
            }else Chassis_Pre_Aim(); //育苗
            //my_Alldir_Chassis_t.chassis_Aim_at_Basket(0.05);
            break;
        case 4:
            //复位校准位置
            chassis_XYPoseServo_calc(1.23,3.56);
            my_Alldir_Chassis_t.YAWPosServo(FORWARD_ANGLE);
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