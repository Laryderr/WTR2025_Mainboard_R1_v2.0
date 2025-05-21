/**
 * @file Handle_Dunk_task.c
 * @author Lary (you@domain.com)
 * @brief 扣篮实现线程
 * @version 0.1
 * @date 2024-11-24
 * 
 * @copyright Copyright (c) 2024
 * 
 * @attention 腿向下蹬是正转，抬腿是反转
 * 
 */

#include "Handle_Dunk_task.h"


Dunk_Task_t my_Dunk_Task_t;//定义扣篮任务结构体



/****************************************************线程定义******************************************/
osThreadId_t unitree_dunk_ctrl_TaskHandle;
const osThreadAttr_t unitree_dunk_ctrl_Task_attributes = {
    .name       = "unitree_dunk_ctrl_Task",
    .stack_size = 128*4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void Handle_Dunk_Task(void *argument);



/**
 * @brief Unitree底盘跳跃电机控制线程创建
 * 
 */
void Handle_Dunk_TaskStart(void)
{
    unitree_dunk_ctrl_TaskHandle = osThreadNew(Handle_Dunk_Task,NULL,&unitree_dunk_ctrl_Task_attributes);
}



/**************************************************内部控制函数***********************************************************************/

/**
 * @brief 宇树电机零力矩模式，阻尼最小
 * @param 电机id
 */
void Unitree_motor_0Torque(uint8_t i)
{
        unitree_DunkMotor_t[i].cmd.T = 0;
        unitree_DunkMotor_t[i].cmd.K_P = 0;
        unitree_DunkMotor_t[i].cmd.W = 0;
        unitree_DunkMotor_t[i].cmd.K_W = 0.02;
}

/**
 * @brief PD控制器力位混合控制，加入前馈力矩
 * 
 */
void Unitree_motor_PosAndTor(void)
{
    for (uint8_t i = 0; i < 3; i++)
    {
        unitree_DunkMotor_t[i].cmd.T = unitree_DunkMotor_Tff[i] + unitree_DunkMotor_t[i].cmd.K_P * (unitree_DunkMotor_t[i].cmd.Pos - 
        unitree_DunkMotor_t[i].data.Pos) + unitree_DunkMotor_t[i].cmd.K_W * (unitree_DunkMotor_t[i].cmd.W - unitree_DunkMotor_t[i].data.W);
    }
}

/**
 * @brief 寻找跳跃电机姿态极限位置
 * 
 * @return float 
 */
float Dunkmotor_Contect_LimitingPos(uint8_t i)
{
    for (uint8_t i = 0; i < 3; i++)
    {
            unitree_DunkMotor_t[i].cmd.Pos = 0;
            unitree_DunkMotor_t[i].cmd.K_P = 0;
            unitree_DunkMotor_t[i].cmd.K_W = 0;
            unitree_DunkMotor_t[i].cmd.T = -0.4;
            unitree_DunkMotor_t[i].cmd.W = 0;
    }
    return unitree_DunkMotor_t[i].data.Pos;

}

/**
 * @brief 角编码器位置pid初始化
 * 
 * @param upid 
 * @param ref 
 * @param KP 
 * @param KI 
 * @param KD 
 */
void Encoder_Pospid_Init(PID_t *upid,float ref, float KP, float KI, float KD)
{
    upid->ref         = ref;   /* 设定目标值 */
    upid->output      = 0.0; /* 期望输出值 */
    upid->integral    = 0.0; /* 积分值 */
    upid->cur_error   = 0.0; /* Error[1] */
    upid->error[0]    = 0.0; /* Error[-1] */
    upid->error[1]    = 0.0; /* Error[-2] */
    upid->KP          = KP;  /* 比例常数 Proportional Const */
    upid->KI          = KI;  /* 积分常数 Integral Const */
    upid->KD          = KD;  /* 微分常数 Derivative Const */
    upid->outputMax   = 2;
    upid->outputMin   = -2;
}

/**
 * @brief 角编码器位置伺服
 * 
 * @param upid 
 * @param Feedback_value 
 */
float Encoder_PosServo(PID_t *upid, float Feedback_value)
{
    upid->fdb =Feedback_value;
    upid->cur_error = (float)(upid->ref - upid->fdb); /* 计算偏差 */

    //upid->integral += upid->cur_error;
    upid->output        = (upid->KP * upid->cur_error)                        /* 比例环节 */
                        + (upid->KI * upid->error[0])                     /* 积分环节 */
                        + (upid->KD * (upid->cur_error - upid->error[0])); /* 微分环节 */
    upid->error[0] = upid->cur_error;
    
    if(upid->output >= upid->outputMax) upid->output = upid->outputMax;
    if(upid->output <= upid->outputMin) upid->output = upid->outputMin;

    return (upid->output); /* 返回计算后输出的数值 */
}


/*****************************************************************************************************************************
 * @brief 跳跃抛球
 * 
 * @param this_status 
 */
void ThrowBall(void)
{
    if(my_Dunk_Task_t.throw_delay_flag == 0)
    {
        osDelay(400);
        my_Dunk_Task_t.throw_delay_flag  = 1;
    }

    //开始摆臂
    if(my_Dunk_Task_t.Throwball_Flag == 0)
    {
        /*unitree_DunkMotor_t[3].cmd.Pos = (my_Dunk_Task_t.Throwball_InitialPos - Max_ThrowBall_Angle);
        unitree_DunkMotor_t[3].cmd.K_P = 1.9;
        unitree_DunkMotor_t[3].cmd.K_W = 0.2;
        unitree_DunkMotor_t[3].cmd.W = -1;
        unitree_DunkMotor_Tff[3] = -15.5;*/
        unitree_DunkMotor_t[3].cmd.T = -2.5;
        unitree_DunkMotor_t[4].cmd.T = 2.5;
        my_Dunk_Task_t.Throwball_Flag = 1;
    }
    //刹停力矩
    if(encoderData.angle >= Encoder_VertPos + 140 && my_Dunk_Task_t.Throwball_Flag == 1)
    {
        unitree_DunkMotor_t[3].cmd.Pos = 0;
        unitree_DunkMotor_t[3].cmd.K_P = 0;
        unitree_DunkMotor_t[3].cmd.K_W = 0;
        unitree_DunkMotor_t[3].cmd.W = 0;
        unitree_DunkMotor_t[3].cmd.T = 3;

        unitree_DunkMotor_t[4].cmd.Pos = 0;
        unitree_DunkMotor_t[4].cmd.K_P = 0;
        unitree_DunkMotor_t[4].cmd.K_W = 0;
        unitree_DunkMotor_t[4].cmd.W = 0;
        unitree_DunkMotor_t[4].cmd.T = 0;
        my_Dunk_Task_t.Throwball_Flag = 2;
    }
    //下降控制
    if (my_Dunk_Task_t.Throwball_Flag == 2 && encoderData.angle <= Encoder_VertPos + 100)
    {
        unitree_DunkMotor_t[3].cmd.Pos = my_Dunk_Task_t.Throwball_InitialPos[0];
        unitree_DunkMotor_t[3].cmd.K_P = 0;
        unitree_DunkMotor_t[3].cmd.K_W = 0.2;
        unitree_DunkMotor_t[3].cmd.W = 0.1;
        unitree_DunkMotor_t[3].cmd.T = 0;

        unitree_DunkMotor_t[4].cmd.Pos = 0;
        unitree_DunkMotor_t[4].cmd.K_P = 0;
        unitree_DunkMotor_t[4].cmd.K_W = 0;
        unitree_DunkMotor_t[4].cmd.W = 0;
        unitree_DunkMotor_t[4].cmd.T = 0;
        //unitree_DunkMotor_t[3].cmd.T = - 0.6;
        if(encoderData.angle <= Encoder_VertPos + 10)
        {
            my_Dunk_Task_t.Throwball_Flag = 3;
        }
    }

    osDelay(2);
    
    //}

}

/*****************************************************************************************************************************
 * @brief 开启底盘跳跃缓冲气缸
 * 
 */
void BufferOn(void)
{
    HAL_GPIO_WritePin(POWER3_GPIO_Port,POWER3_Pin,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(POWER4_GPIO_Port,POWER4_Pin,GPIO_PIN_SET);
    my_Dunk_Task_t.Buffer_flag = 1;
}

/**
 * @brief 关闭底盘跳跃缓冲气缸
 * 
 */
void BufferOff(void)
{
    HAL_GPIO_WritePin(POWER3_GPIO_Port,POWER3_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(POWER4_GPIO_Port,POWER4_Pin,GPIO_PIN_RESET);
    my_Dunk_Task_t.Buffer_flag = 0;
}

/**
 * @brief 辅助出球气缸开启
 * 
 */
void BallHelpOn(void)
{
    HAL_GPIO_WritePin(POWER1_GPIO_Port,POWER1_Pin,GPIO_PIN_RESET);
    //HAL_GPIO_WritePin(POWER2_GPIO_Port,POWER2_Pin,GPIO_PIN_RESET);
    my_Dunk_Task_t.Buffer_flag = 1;
}

/**
 * @brief 辅助出球气缸关闭
 * 
 */
void BallHelpOff(void)
{
    HAL_GPIO_WritePin(POWER1_GPIO_Port,POWER1_Pin,GPIO_PIN_SET);
    //HAL_GPIO_WritePin(POWER2_GPIO_Port,POWER2_Pin,GPIO_PIN_SET);
    my_Dunk_Task_t.Buffer_flag = 0;
}


/****************************************************************************************************************************** 
 * @brief 底盘Unitree跳跃电机控制线程
 * 
 */
void Handle_Dunk_Task(void *argument)
{
    for (uint8_t i = 0; i < 3; i++)
    {
        unitree_DunkMotor_t[i].cmd.K_P = 0.02;
        unitree_DunkMotor_t[i].cmd.K_W = 0.02;
    }
    //标志初始化
    my_Dunk_Task_t.my_Dunk_Status = DUNK_IDLE;
    my_Dunk_Task_t.Throwball_Flag = 0;
    my_Dunk_Task_t.Time_TO_Jump = 0;
    my_Dunk_Task_t.my_StatusTransition_cnt = 0;
    my_Dunk_Task_t.Found_LimitingPos_Flag = 0;
    my_Dunk_Task_t.Foundthrow_InitialPos_Flag = 0;
    my_Dunk_Task_t.Jump_Completed_Flag = 0;
    my_Dunk_Task_t.throw_delay_flag = 0;
    for (uint8_t i = 0; i < 3; i++)
    {
        my_Dunk_Task_t.Stood_Flag[i] = 0;
    }
    

    //检测抬腿极限位置
    if (my_Dunk_Task_t.Found_LimitingPos_Flag == 0)
    {
        for (uint8_t i = 0; i < 3; i++)
        {
            my_Dunk_Task_t.Unitree_DunkMotor_LimitingPos[0][i] =  Dunkmotor_Contect_LimitingPos(i);
        }
        /*my_Dunk_Task_t.Unitree_DunkMotor_LimitingPos[0][0] = my_Dunk_Task_t.Unitree_DunkMotor_LimitingPos[0][0]/10;
        my_Dunk_Task_t.Unitree_DunkMotor_LimitingPos[0][1] = my_Dunk_Task_t.Unitree_DunkMotor_LimitingPos[0][1]/10;
        my_Dunk_Task_t.Unitree_DunkMotor_LimitingPos[0][2] = my_Dunk_Task_t.Unitree_DunkMotor_LimitingPos[0][2]/10;*/
        
        //检测是否成功读取三条腿极限位置
        for (uint8_t i = 0; i < 3; i++)
        {
            if (my_Dunk_Task_t.Unitree_DunkMotor_LimitingPos[0][i] != 0)
            {
                my_Dunk_Task_t.Found_LimitingPos_Flag ++;
            }else Error_Handler();
        }

        if (my_Dunk_Task_t.Found_LimitingPos_Flag != 3)
        {
            Error_Handler();
        }
    }

    //检测投球初始位置
    if (my_Dunk_Task_t.Foundthrow_InitialPos_Flag == 0)
    {
        /*for (uint8_t i = 3; i < 5; i++)
        {
            unitree_DunkMotor_t[i].cmd.Pos = 0;
            unitree_DunkMotor_t[i].cmd.K_P = 0;
            unitree_DunkMotor_t[i].cmd.K_W = 0;
            unitree_DunkMotor_t[i].cmd.T = 0;
            unitree_DunkMotor_t[i].cmd.W = 0;
        }
        //osDelay(2000);//使投篮臂自然垂下
        
        for (uint8_t i = 0; i < 10; i++)
        {
            my_Dunk_Task_t.Throwball_InitialPos[0] += unitree_DunkMotor_t[3].data.Pos;
            my_Dunk_Task_t.Throwball_InitialPos[1] += unitree_DunkMotor_t[4].data.Pos;
        }
        my_Dunk_Task_t.Throwball_InitialPos[0] = my_Dunk_Task_t.Throwball_InitialPos[0]/10;
        my_Dunk_Task_t.Throwball_InitialPos[1] = my_Dunk_Task_t.Throwball_InitialPos[1]/10;

        my_Dunk_Task_t.Throwball_InitialPos[0] = (Encoder_VertPos - my_Dunk_Task_t.Encoder_InitialPos)/DegreetoRad*UNITREE_REDUCTION_RATE + my_Dunk_Task_t.Throwball_InitialPos[0];
        my_Dunk_Task_t.Throwball_InitialPos[1] = (Encoder_VertPos - my_Dunk_Task_t.Encoder_InitialPos)/DegreetoRad*UNITREE_REDUCTION_RATE + my_Dunk_Task_t.Throwball_InitialPos[1];*/
        my_Dunk_Task_t.Throwball_InitialPos[0] = my_Shoot_Task_T.Shootball_InitialPos[0];
        my_Dunk_Task_t.Throwball_InitialPos[1] = my_Shoot_Task_T.Shootball_InitialPos[1];

        /*if (my_Dunk_Task_t.Throwball_InitialPos[0] != 0&&my_Dunk_Task_t.Throwball_InitialPos[1] != 0)
        {
            my_Dunk_Task_t.Foundthrow_InitialPos_Flag = 1;
        }else Error_Handler();*/
    }

    for (; ;)
    {
        //遥控器控制跳跃阶段
        if(MyRemote_Data.btn_KnobR == 1)
        {
            my_Dunk_Task_t.my_Dunk_Status = DUNK_JUMP_UP;
        }
        if (MyRemote_Data.btn_KnobL == 1)
        {
            my_Dunk_Task_t.my_Dunk_Status = DUNK_IDLE;
        }
        if(MyRemote_Data.btn_RightCrossMid == 1)
        {
            my_Dunk_Task_t.my_Dunk_Status = DUNK_TEST;
        }
        if(MyRemote_Data.right_switch == 1)
        {
            BufferOn();
        }else BufferOff();

        //状态控制
        if(my_Dunk_Task_t.Found_LimitingPos_Flag == 3)
        {
            switch (my_Dunk_Task_t.my_Dunk_Status)
            {
            
            case DUNK_IDLE:
                //跳跃电机保持收腿
                for (uint8_t i = 0; i < 3; i++)
                {
                unitree_DunkMotor_t[i].cmd.Pos = 0;
                unitree_DunkMotor_t[i].cmd.K_P = 0;
                unitree_DunkMotor_t[i].cmd.K_W = 0;
                unitree_DunkMotor_t[i].cmd.T = 0.3;
                unitree_DunkMotor_t[i].cmd.W = 0;
                }
                
                //投球电机保持垂下
                Encoder_Pospid_Init(&my_Dunk_Task_t.Encoder_PosPID_t,Encoder_VertPos,0.25,0,0.01);
                unitree_DunkMotor_t[3].cmd.Pos = my_Dunk_Task_t.Throwball_InitialPos[0];
                unitree_DunkMotor_t[3].cmd.K_P = 0;
                unitree_DunkMotor_t[3].cmd.W = 0;
                unitree_DunkMotor_t[3].cmd.K_W = 0;
                unitree_DunkMotor_t[3].cmd.T = -Encoder_PosServo(&my_Dunk_Task_t.Encoder_PosPID_t,encoderData.angle);

                unitree_DunkMotor_t[4].cmd.Pos = 0;
                unitree_DunkMotor_t[4].cmd.K_P = 0;
                unitree_DunkMotor_t[4].cmd.W = 0;
                unitree_DunkMotor_t[4].cmd.K_W = 0;
                unitree_DunkMotor_t[4].cmd.T = 0;

                
                //标志量置零
                my_Dunk_Task_t.Jump_Completed_Flag = 0;
                my_Dunk_Task_t.Time_TO_Jump = 0;
                my_Dunk_Task_t.Throwball_Flag = 0;
                my_Dunk_Task_t.throw_delay_flag = 0;
                for (uint8_t i = 0; i < 3; i++)
                {
                    my_Dunk_Task_t.Stood_Flag[i] = 0;
                }
                
                if(MyRemote_Data.right_switch == 1)
                {
                    BufferOff();
                }else BufferOn();
                break;
            case DUNK_TEST:
                /*unitree_DunkMotor_t[1].cmd.Pos = 0;
                unitree_DunkMotor_t[1].cmd.K_P = 0;
                unitree_DunkMotor_t[1].cmd.K_W = 0;
                unitree_DunkMotor_t[1].cmd.T = -1.3 ;
                unitree_DunkMotor_t[1].cmd.W = 0;*/
                BufferOn();
                //开始摆臂
                if(my_Dunk_Task_t.Throwball_Flag == 0)
                {
                    /*unitree_DunkMotor_t[3].cmd.Pos = (my_Dunk_Task_t.Throwball_InitialPos - Max_ThrowBall_Angle);
                    unitree_DunkMotor_t[3].cmd.K_P = 1.9;
                    unitree_DunkMotor_t[3].cmd.K_W = 0.2;
                    unitree_DunkMotor_t[3].cmd.W = -1;
                    unitree_DunkMotor_Tff[3] = -15.5;*/
                    unitree_DunkMotor_t[3].cmd.T = -7;
                    unitree_DunkMotor_t[4].cmd.T = 7;
                    my_Dunk_Task_t.Throwball_Flag = 1;
                }
                //刹停力矩
                if(encoderData.angle >= Encoder_VertPos + 90 && my_Dunk_Task_t.Throwball_Flag == 1)
                {
                    unitree_DunkMotor_t[3].cmd.Pos = 0;
                    unitree_DunkMotor_t[3].cmd.K_P = 0;
                    unitree_DunkMotor_t[3].cmd.K_W = 0;
                    unitree_DunkMotor_t[3].cmd.W = 0;
                    unitree_DunkMotor_t[3].cmd.T = 7;

                    unitree_DunkMotor_t[4].cmd.Pos = 0;
                    unitree_DunkMotor_t[4].cmd.K_P = 0;
                    unitree_DunkMotor_t[4].cmd.K_W = 0;
                    unitree_DunkMotor_t[4].cmd.W = 0;
                    unitree_DunkMotor_t[4].cmd.T = -7;
                    my_Dunk_Task_t.Throwball_Flag = 2;
                }
                //下降控制
                if (my_Dunk_Task_t.Throwball_Flag == 2 && encoderData.angle <= Encoder_VertPos + 100)
                {
                    unitree_DunkMotor_t[3].cmd.Pos = my_Dunk_Task_t.Throwball_InitialPos[0];
                    unitree_DunkMotor_t[3].cmd.K_P = 0;
                    unitree_DunkMotor_t[3].cmd.K_W = 0.2;
                    unitree_DunkMotor_t[3].cmd.W = 0.1;
                    unitree_DunkMotor_t[3].cmd.T = 0;

                    unitree_DunkMotor_t[4].cmd.Pos = 0;
                    unitree_DunkMotor_t[4].cmd.K_P = 0;
                    unitree_DunkMotor_t[4].cmd.K_W = 0;
                    unitree_DunkMotor_t[4].cmd.W = 0;
                    unitree_DunkMotor_t[4].cmd.T = 0;
                    //unitree_DunkMotor_t[3].cmd.T = - 0.6;
                    if(encoderData.angle <= Encoder_VertPos + 10)
                    {
                        my_Dunk_Task_t.Throwball_Flag = 3;
                    }
                }
                
                //回到空闲
                if(my_Dunk_Task_t.Throwball_Flag == 3&&encoderData.angle <= Encoder_VertPos)
                {
                    unitree_DunkMotor_t[3].cmd.Pos = 0;
                    unitree_DunkMotor_t[3].cmd.K_P = 0;
                    unitree_DunkMotor_t[3].cmd.K_W = 0;
                    unitree_DunkMotor_t[3].cmd.W = 0;
                    unitree_DunkMotor_t[3].cmd.T = 0;

                    unitree_DunkMotor_t[4].cmd.Pos = 0;
                    unitree_DunkMotor_t[4].cmd.K_P = 0;
                    unitree_DunkMotor_t[4].cmd.K_W = 0;
                    unitree_DunkMotor_t[4].cmd.W = 0;
                    unitree_DunkMotor_t[4].cmd.T = 0;
                    my_Dunk_Task_t.my_Dunk_Status = DUNK_IDLE;
                }

                osDelay(2);
                break;
            case DUNK_JUMP_UP:
                BufferOn();
                //执行跳跃

                //未达到跳跃极限
                if(my_Dunk_Task_t.Jump_Completed_Flag == 0)
                {
                    for (uint8_t i = 0; i < 3; i++)
                    {
                        unitree_DunkMotor_t[i].cmd.Pos = 0;
                        unitree_DunkMotor_t[i].cmd.K_P = 0;
                        unitree_DunkMotor_t[i].cmd.K_W = 0;
                        unitree_DunkMotor_t[i].cmd.T = 20;
                        unitree_DunkMotor_t[i].cmd.W = 0;
                    }
                    my_Dunk_Task_t.Jump_Completed_Flag = 1;
                }

                //检测是否达到跳跃伸腿极限
                if (my_Dunk_Task_t.Jump_Completed_Flag == 1)
                {
                    for (uint8_t i = 0; i < 3; i++)
                    {
                        if(unitree_DunkMotor_t[i].data.Pos >= ( my_Dunk_Task_t.Unitree_DunkMotor_LimitingPos[0][i] + Max_Extension_Angle ))
                        {
                            my_Dunk_Task_t.Stood_Flag[i] = 1;
                        }
                    }
                    if(my_Dunk_Task_t.Stood_Flag[0] == 1)
                    {
                        if(my_Dunk_Task_t.Stood_Flag[1] == 1)
                        {
                            if(my_Dunk_Task_t.Stood_Flag[2] == 1)
                            {
                                //完全站立
                                my_Dunk_Task_t.Jump_Completed_Flag = 2;
                            }
                        }
                    }
                }
                
                if(my_Dunk_Task_t.Jump_Completed_Flag >= 2)//全部伸腿完毕
                {
                    //启动摆臂
                    //ThrowBall();
                }

                //开始收腿
                if( encoderData.angle >= Encoder_VertPos + 70 && my_Dunk_Task_t.Jump_Completed_Flag == 2)
                {
                    for (uint8_t i = 0; i < 3; i++)
                    {
                        unitree_DunkMotor_t[i].cmd.Pos = 0;
                        unitree_DunkMotor_t[i].cmd.K_P = 0;
                        unitree_DunkMotor_t[i].cmd.K_W = 0;
                        unitree_DunkMotor_t[i].cmd.T = -5;
                        unitree_DunkMotor_t[i].cmd.W = 0;
                    }
                    my_Dunk_Task_t.Jump_Completed_Flag = 3;
                }

                //完成收腿恢复正常扭矩
                if (my_Dunk_Task_t.Jump_Completed_Flag == 3)
                {
                    for(uint8_t i = 0; i < 3; i++)
                    {
                        if (unitree_DunkMotor_t[i].data.Pos <= my_Dunk_Task_t.Unitree_DunkMotor_LimitingPos[0][i])
                        {
                            unitree_DunkMotor_t[i].cmd.T = -0.35;
                        }
                    }
                    if(unitree_DunkMotor_t[0].cmd.T == -0.35)
                    {
                        if(unitree_DunkMotor_t[1].cmd.T == -0.35)
                        {
                            if(unitree_DunkMotor_t[2].cmd.T == -0.35)
                            {
                                my_Dunk_Task_t.Jump_Completed_Flag = 4;
                            }
                        }
                    }
                }

                //三条腿收腿完毕且投篮臂恢复
                if (encoderData.angle <= Encoder_VertPos && my_Dunk_Task_t.Jump_Completed_Flag == 4 )
                {
                    my_Dunk_Task_t.my_Dunk_Status = DUNK_IDLE;
                }

                    //调试起跳
                    /*for (uint8_t i = 0; i < 3; i++)
                    {
                        
                        //达到跳跃伸腿极限
                        if(unitree_DunkMotor_t[i].data.Pos >= ( my_Dunk_Task_t.Unitree_DunkMotor_LimitingPos[0][i] + Max_Extension_Angle ))
                        {
                            my_Dunk_Task_t.Jump_Completed_Flag ++;
                            

                            //开始收腿
                            unitree_DunkMotor_t[i].cmd.Pos = 0;
                            unitree_DunkMotor_t[i].cmd.K_P = 0;
                            unitree_DunkMotor_t[i].cmd.K_W = 0;
                            unitree_DunkMotor_t[i].cmd.T = -1.5;
                            unitree_DunkMotor_t[i].cmd.W = 0;

                            //完成收腿恢复正常扭矩
                            if (unitree_DunkMotor_t[i].data.Pos <= my_Dunk_Task_t.Unitree_DunkMotor_LimitingPos[0][i])
                            {
                                unitree_DunkMotor_t[i].cmd.T = -0.45;
                                my_Dunk_Task_t.Jump_Completed_Flag ++;
                            }

                            //三条腿收腿完毕
                            if (my_Dunk_Task_t.Jump_Completed_Flag >= 6 )
                            {
                                my_Dunk_Task_t.my_Dunk_Status = DUNK_IDLE;
                            }
                            
                            
                        }
                    }*/
                    
                    
                
                break;
            default:
                break;
            }
        
        }
        
        osDelay(1);
    }
    
}