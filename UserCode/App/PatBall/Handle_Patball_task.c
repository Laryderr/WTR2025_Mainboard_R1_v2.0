/*
 * @Description: 拍球线程
 * @Author: Alex
 * @Date: 2025-03-22 10:35:23
 * @LastEditors: Alex
 * @LastEditTime: 2025-07-25 12:27:34
 */

 #include "Handle_Patball_task.h"

 PATBALLSTATE patballstate;
 float Unitree_Patball_IniPos[2];
 
 /**
  * @brief 拍球线程定义
  * 
  */
 osThreadId_t Patball_TaskHandle;
 const osThreadAttr_t Patball_Task_attributes = {
     .name       = "Patball_Task",
     .stack_size = 128*4,
     .priority   = (osPriority_t)osPriorityNormal,
 };
 
 /**
  * @brief 创建拍球线程
  * 
  */
 void Patball_TaskStart()
 {
     Patball_TaskHandle =  osThreadNew(Patball_Task, NULL, &Patball_Task_attributes);
 }
 
 /**
  * @brief 拍球线程
  * 
  * @param argument 
  */
 void Patball_Task(void* argument)
 {
    for (uint8_t i = 3; i < 6; i++)
    {
        unitree_DunkMotor_t[i].cmd.Pos = 0;
        unitree_DunkMotor_t[i].cmd.K_P = 0;
        unitree_DunkMotor_t[i].cmd.K_W = 0;
        unitree_DunkMotor_t[i].cmd.T = 0;
        unitree_DunkMotor_t[i].cmd.W = 0;
    }
    for (uint8_t i = 0; i < 10; ++i)
    {
        Unitree_Patball_IniPos[0] += unitree_DunkMotor_t[3].data.Pos;
        Unitree_Patball_IniPos[1] += unitree_DunkMotor_t[5].data.Pos;
    }
    for (uint8_t i = 0; i < 2; ++i) Unitree_Patball_IniPos[i] /= 10;
    for (;;)
    {
        if (myHandle_State == HANDLE_PATBALL_MODE)
        { 
            if (MyRemote_Data.btn_KnobR == 1 && patballstate == PATBALL_IDLE)
            {
                patballstate = PATBALL_PATING;
            }
            
            if (patballstate == PATBALL_PATING)
            {
            //上升
                float x = my_Alldir_Chassis_t.current_pos.xpos;
                float y = my_Alldir_Chassis_t.current_pos.ypos;
                float yaw = my_Alldir_Chassis_t.current_pos.yawpos;
                chassis_RePosToAbPos(0.2, 0, &target_x, &target_y);
                unitree_DunkMotor_t[3].cmd.T = 0;
                unitree_DunkMotor_t[3].cmd.W = -0.7;
                unitree_DunkMotor_t[3].cmd.Pos = 0;
                unitree_DunkMotor_t[3].cmd.K_P = 0;
                unitree_DunkMotor_t[3].cmd.K_W = 0.916;
                unitree_DunkMotor_t[5].cmd.T = 0;
                unitree_DunkMotor_t[5].cmd.W = 0.7;
                unitree_DunkMotor_t[5].cmd.Pos = 0;
                unitree_DunkMotor_t[5].cmd.K_P = 0;
                unitree_DunkMotor_t[5].cmd.K_W = 0.916;
                // unitree_DunkMotor_t[3].cmd.T = 0;
                // unitree_DunkMotor_t[3].cmd.W = -2;
                // unitree_DunkMotor_t[3].cmd.Pos = Unitree_Patball_IniPos[0]-4;
                // unitree_DunkMotor_t[3].cmd.K_P = 2.5;
                // unitree_DunkMotor_t[3].cmd.K_W = 1.1;
                // unitree_DunkMotor_t[3].cmd.T = -3.5;
                // unitree_DunkMotor_t[3].cmd.W = 0;
                // unitree_DunkMotor_t[3].cmd.Pos = 0;
                // unitree_DunkMotor_t[3].cmd.K_P = 0;
                // unitree_DunkMotor_t[3].cmd.K_W = 0;
                // unitree_DunkMotor_t[5].cmd.T = 3.5;
                // unitree_DunkMotor_t[5].cmd.W = 0;
                // unitree_DunkMotor_t[5].cmd.Pos = 0;
                // unitree_DunkMotor_t[5].cmd.K_P = 0;
                // unitree_DunkMotor_t[5].cmd.K_W = 0;
                while (encoderCalculateData.angle < 9000)
                {
                    osDelay(1);
                }
                // while (encoderCalculateData.angle < 8500)
                // {
                //     osDelay(1);
                // }
                unitree_DunkMotor_t[3].cmd.T = 0;
                unitree_DunkMotor_t[3].cmd.W = 0.7;
                unitree_DunkMotor_t[3].cmd.Pos = 0;
                unitree_DunkMotor_t[3].cmd.K_P = 0;
                unitree_DunkMotor_t[3].cmd.K_W = 0.9;
                unitree_DunkMotor_t[5].cmd.T = 0;
                unitree_DunkMotor_t[5].cmd.W = -0.7;
                unitree_DunkMotor_t[5].cmd.Pos = 0;
                unitree_DunkMotor_t[5].cmd.K_P = 0;
                unitree_DunkMotor_t[5].cmd.K_W = 0.9;
                Expansion_Up.contract();
                Expansion_Down.contract();
                //回刹
                while (encoderCalculateData.angle > 7000)
                {
                    osDelay(1);
                }
                //缓降
                /*while (encoderCalculateData.angle <= 12000&&encoderCalculateData.angle > 9600)
                {
                Expansion_Up.contract();
                Expansion_Down.contract();
                unitree_DunkMotor_t[3].cmd.T = 0;
                unitree_DunkMotor_t[3].cmd.W = 0.01;
                unitree_DunkMotor_t[3].cmd.Pos = 0;
                unitree_DunkMotor_t[3].cmd.K_P = 0;
                unitree_DunkMotor_t[3].cmd.K_W = 0.2;
                unitree_DunkMotor_t[4].cmd.T = 0;
                unitree_DunkMotor_t[4].cmd.W = 0;
                unitree_DunkMotor_t[4].cmd.Pos = 0;
                unitree_DunkMotor_t[4].cmd.K_P = 0;
                unitree_DunkMotor_t[4].cmd.K_W = 0;
                osDelay(2);
                }
                if (encoderCalculateData.angle <= 9600)
                {
                patballstate = PATBALL_IDLE;
                }*/
                unitree_DunkMotor_t[3].cmd.T = 0.6;
                unitree_DunkMotor_t[3].cmd.W = 0;
                unitree_DunkMotor_t[3].cmd.Pos = 0;
                unitree_DunkMotor_t[3].cmd.K_P = 0;
                unitree_DunkMotor_t[3].cmd.K_W = 0;
                unitree_DunkMotor_t[5].cmd.T = -0.6;
                unitree_DunkMotor_t[5].cmd.W = 0;
                unitree_DunkMotor_t[5].cmd.Pos = 0;
                unitree_DunkMotor_t[5].cmd.K_P = 0;
                unitree_DunkMotor_t[5].cmd.K_W = 0;
                for(int i = 0; i < 500; ++i)
                {
                    my_Alldir_Chassis_t.target_v.vx = -3;
                    my_Alldir_Chassis_t.target_v.vy = 0;
                    chassis_YAWPoseServo_calc(yaw);
                    osDelay(1);
                }
                //osDelay(500);
                unitree_DunkMotor_t[3].cmd.T = 0;
                unitree_DunkMotor_t[3].cmd.W = 0;
                unitree_DunkMotor_t[3].cmd.Pos = 0;
                unitree_DunkMotor_t[3].cmd.K_P = 0;
                unitree_DunkMotor_t[3].cmd.K_W = 0;
                unitree_DunkMotor_t[5].cmd.T = 0;
                unitree_DunkMotor_t[5].cmd.W = 0;
                unitree_DunkMotor_t[5].cmd.Pos = 0;
                unitree_DunkMotor_t[5].cmd.K_P = 0;
                unitree_DunkMotor_t[5].cmd.K_W = 0;
                while (sqrt(pow((my_Alldir_Chassis_t.current_pos.xpos - (x+target_x)),2) + pow((my_Alldir_Chassis_t.current_pos.ypos - (y+target_y)),2)) > 0.05)
                {
                    chassis_XYPoseServo_calc(x + target_x, y + target_y);
                    chassis_YAWPoseServo_calc(yaw);
                    osDelay(1);
                }

                // float x = my_Alldir_Chassis_t.current_pos.xpos;
                // float y = my_Alldir_Chassis_t.current_pos.ypos;
                // float yaw = my_Alldir_Chassis_t.current_pos.yawpos;
                // float target_x, target_y;
                // chassis_RePosToAbPos(-0.8, 0.4, &target_x, &target_y);
                // while (sqrt(pow((my_Alldir_Chassis_t.current_pos.xpos - (x+target_x)),2) + pow((my_Alldir_Chassis_t.current_pos.ypos - (y+target_y)),2)) > 0.7)
                // {
                //     my_Alldir_Chassis_t.target_v.vx = 5;
                //     my_Alldir_Chassis_t.target_v.vy = -2.5;
                //     chassis_YAWPoseServo_calc(yaw);
                //     osDelay(2);
                // }
                // while (sqrt(pow((my_Alldir_Chassis_t.current_pos.xpos - (x+target_x)),2) + pow((my_Alldir_Chassis_t.current_pos.ypos - (y+target_y)),2)) > 0.05)
                // {
                //     chassis_XYPoseServo_calc(x + target_x, y + target_y);
                //     chassis_YAWPoseServo_calc(yaw);
                //     osDelay(1);
                // }
                my_Alldir_Chassis_t.target_v.vx = 0;
                my_Alldir_Chassis_t.target_v.vy = 0;
                my_Alldir_Chassis_t.target_v.vw = 0;
                patballstate = PATBALL_IDLE;
            }
            else if (patballstate == PATBALL_IDLE)
            {
            //Encoder_Pospid_Init(&my_Dunk_Task_t.Encoder_PosPID_t,Encoder_VertPos,0.25,0,0.01);//0.25,0,0.01
            unitree_DunkMotor_t[3].cmd.Pos = 0;
            unitree_DunkMotor_t[3].cmd.K_P = 0;
            unitree_DunkMotor_t[3].cmd.W = 0;
            unitree_DunkMotor_t[3].cmd.K_W = 0;
            unitree_DunkMotor_t[3].cmd.T = 0;
            //unitree_DunkMotor_t[3].cmd.T = -Encoder_PosServo(&my_Dunk_Task_t.Encoder_PosPID_t,encoderData.angle);
            // unitree_DunkMotor_t[5].cmd.Pos = Unitree_Patball_IniPos[1]  + 1.5;
            // unitree_DunkMotor_t[5].cmd.K_P = 0.4;
            // unitree_DunkMotor_t[5].cmd.K_W = 0.15;
            // unitree_DunkMotor_t[5].cmd.W = 0.1;
            // unitree_DunkMotor_t[5].cmd.T = 0;
            unitree_DunkMotor_t[5].cmd.Pos = 0;
            unitree_DunkMotor_t[5].cmd.K_P = 0;
            unitree_DunkMotor_t[5].cmd.W = 0;
            unitree_DunkMotor_t[5].cmd.K_W = 0;
            unitree_DunkMotor_t[5].cmd.T = 0;
            }
            osDelay(2);
        }
        else
        {
            // unitree_DunkMotor_t[3].cmd.Pos = 0;
            // unitree_DunkMotor_t[3].cmd.K_P = 0;
            // unitree_DunkMotor_t[3].cmd.W = 0;
            // unitree_DunkMotor_t[3].cmd.K_W = 0;
            // unitree_DunkMotor_t[3].cmd.T = 0;
            // unitree_DunkMotor_t[5].cmd.Pos = 0;
            // unitree_DunkMotor_t[5].cmd.K_P = 0;
            // unitree_DunkMotor_t[5].cmd.W = 0;
            // unitree_DunkMotor_t[5].cmd.K_W = 0;
            // unitree_DunkMotor_t[5].cmd.T = 0;
            osDelay(10);
        }
    }
 }