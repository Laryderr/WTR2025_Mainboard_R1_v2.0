/*
 * @Description: 拍球线程
 * @Author: Alex
 * @Date: 2025-03-22 10:35:23
 * @LastEditors: Alex
 * @LastEditTime: 2025-03-28 23:06:20
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
     for (uint8_t i = 3; i < 5; i++)
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
         Unitree_Patball_IniPos[1] += unitree_DunkMotor_t[4].data.Pos;
     }
     for (uint8_t i = 0; i < 2; ++i) Unitree_Patball_IniPos[i] /= 10;
 
     for (;;)
     {
         if (MyRemote_Data.btn_KnobR == 1 && patballstate == PATBALL_IDLE)
         {
             patballstate = PATBALL_PATING;
         }
         
         if (patballstate == PATBALL_PATING)
         {
            //上升
             while (encoderCalculateData.angle < 14000)
             {
                 unitree_DunkMotor_t[3].cmd.T = -5;
                 unitree_DunkMotor_t[3].cmd.W = 0;
                 unitree_DunkMotor_t[3].cmd.Pos = 0;
                 unitree_DunkMotor_t[3].cmd.K_P = 0;
                 unitree_DunkMotor_t[3].cmd.K_W = 0;
                 unitree_DunkMotor_t[4].cmd.T = 5;
                 unitree_DunkMotor_t[4].cmd.W = 0;
                 unitree_DunkMotor_t[4].cmd.Pos = 0;
                 unitree_DunkMotor_t[4].cmd.K_P = 0;
                 unitree_DunkMotor_t[4].cmd.K_W = 0;
                 osDelay(2);
             }
             Expansion_Up.contract();
             Expansion_Down.contract();
             //回刹
             while (encoderCalculateData.angle > 10000)
             {
                unitree_DunkMotor_t[3].cmd.T = 0;
                unitree_DunkMotor_t[3].cmd.W = 0;
                unitree_DunkMotor_t[3].cmd.Pos = 0;
                unitree_DunkMotor_t[3].cmd.K_P = 0;
                unitree_DunkMotor_t[3].cmd.K_W = 0;
                unitree_DunkMotor_t[4].cmd.T = -4;
                unitree_DunkMotor_t[4].cmd.W = 0;
                unitree_DunkMotor_t[4].cmd.Pos = 0;
                unitree_DunkMotor_t[4].cmd.K_P = 0;
                unitree_DunkMotor_t[4].cmd.K_W = 0;
                osDelay(2);
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
             
             
             osDelay(500);
             unitree_DunkMotor_t[3].cmd.T = 0;
             unitree_DunkMotor_t[3].cmd.W = 0;
             unitree_DunkMotor_t[3].cmd.Pos = 0;
             unitree_DunkMotor_t[3].cmd.K_P = 0;
             unitree_DunkMotor_t[3].cmd.K_W = 0;
             unitree_DunkMotor_t[4].cmd.T = 0;
             unitree_DunkMotor_t[4].cmd.W = 0;
             unitree_DunkMotor_t[4].cmd.Pos = 0;
             unitree_DunkMotor_t[4].cmd.K_P = 0;
             unitree_DunkMotor_t[4].cmd.K_W = 0;
             patballstate = PATBALL_IDLE;
         }else if (patballstate == PATBALL_IDLE)
         {
            Encoder_Pospid_Init(&my_Dunk_Task_t.Encoder_PosPID_t,Encoder_VertPos,0.25,0,0.01);//0.25,0,0.01
            unitree_DunkMotor_t[3].cmd.Pos = 0;
            unitree_DunkMotor_t[3].cmd.K_P = 0;
            unitree_DunkMotor_t[3].cmd.W = 0;
            unitree_DunkMotor_t[3].cmd.K_W = 0;
            unitree_DunkMotor_t[3].cmd.T = -Encoder_PosServo(&my_Dunk_Task_t.Encoder_PosPID_t,encoderData.angle);

            unitree_DunkMotor_t[4].cmd.Pos = 0;
            unitree_DunkMotor_t[4].cmd.K_P = 0;
            unitree_DunkMotor_t[4].cmd.W = 0;
            unitree_DunkMotor_t[4].cmd.K_W = 0;
            unitree_DunkMotor_t[4].cmd.T = 0;
         }
         
         if (MyRemote_Data.btn_LeftCrossUp == 1)
         {
             Expansion_Up.expandhighly();
             Expansion_Down.expandhighly();
         }
         else if (MyRemote_Data.btn_LeftCrossLeft == 1)
         {
             Expansion_Up.expand();
             Expansion_Down.expand();
         }
         else if (MyRemote_Data.btn_LeftCrossDown == 1)
         {
             Expansion_Up.contract();
             Expansion_Down.contract();
         }
         osDelay(2);
     }
 }