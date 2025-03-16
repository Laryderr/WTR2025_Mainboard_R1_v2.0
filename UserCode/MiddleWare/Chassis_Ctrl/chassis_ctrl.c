/**
 * @file classis_ctrl.c
 * @author Lary
 * @brief 三全向轮底盘
 * @version 0.1
 * @date 2024-11-04
 * 
 * 方向定义：
 *      底盘方向定义
 *         ^x(wheel0)
 *         |
 *     y   |
 *    <————.   轮组0的轴线和x正方向重合，轮组1，2的中心和底盘中心的连线与y轴夹角为30度
 *        
 * @copyright Copyright (c) 2024
 * 
 */

#include "chassis_ctrl.h"


/**********************************************************
 * @brief 线程定义
 * 
 */
osThreadId_t can_message_TaskHandle;
const osThreadAttr_t can_message_Task_attributes = {
    .name       = "can_message_Task",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void my_CAN_Message_Task(void *argument);

osThreadId_t chassis_ctrl_TaskHandle;
const osThreadAttr_t chassis_ctrl_Task_attributes = {
    .name       = "chassis_ctrl_Task",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void my_Chassis_Ctrl_Task(void *argument);

/*********************************************************
 * @brief 数据接口定义
 * 
 */

//float wheel0_velocity, wheel1_velocity, wheel2_velocity;            //底盘目标电机速度

Alldir_Chassis_t my_Alldir_Chassis_t;
float v0,v1,v2;

/***********************************************************
 * @brief 底盘整体运动控制
 * 
 */


/************************************************************
 * @brief 底盘初始化
 * 
 */
void my_Chassis_Init(void)
{
    //注意cube里的can引脚配置
    CANFilterInit(&hcan1);
    osDelay(2);
    //CAN2FilterInit(&hcan2);
    ENCODER_CANFilterInit(&hcan2);
    HAL_CAN_Start(&hcan2);
    HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
    /*if (HAL_CAN_Start(&hcan2) != HAL_OK) {
        Error_Handler();
    }*/

    if (HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
        Error_Handler();
    }

    for (int i = 0; i < 4; i++){
        hDJI[i].motorType = M3508;
    }
    DJI_Init();
    for (int i = 0; i < 4; i++) {
        hDJI[i].posPID.KI=1.5;
        hDJI[i].posPID.KD=0;
        hDJI[i].speedPID.outputMax=5000;
        hDJI[i].speedPID.KP=1.7;
        hDJI[i].speedPID.KI=0.01;
        hDJI[i].speedPID.KD=0.3;
        
    }    
}

/***************************************************************
 * @brief   底盘电机CAN消息发送线程创建
 */
void my_Chassis_CAN_Message_TaskStart(void)
{
    can_message_TaskHandle = osThreadNew(my_CAN_Message_Task, NULL, &can_message_Task_attributes);
}

/**************************************************************
 * @brief   底盘控制启动线程
 */
void my_Chassis_Ctrl_TaskStart(void)
{
    chassis_ctrl_TaskHandle = osThreadNew(my_Chassis_Ctrl_Task, NULL, &chassis_ctrl_Task_attributes);
}



/**************************************************************
 * @brief 线程函数，底盘电机CAN发送消息线程
 * 
 * @param arguement 
 */
void my_CAN_Message_Task(void *arguement)
{
    for(;;)
    {
        //注意速度伺服函数的参数单位
        speedServo(my_Alldir_Chassis_t.my_wheel[0].target_v * hDJI[0].reductionRate,&hDJI[0]);
        speedServo(my_Alldir_Chassis_t.my_wheel[1].target_v * hDJI[0].reductionRate,&hDJI[1]);
        speedServo(my_Alldir_Chassis_t.my_wheel[2].target_v * hDJI[0].reductionRate,&hDJI[2]);
        CanTransmit_DJI_1234(&hcan1,hDJI[0].speedPID.output,hDJI[1].speedPID.output,hDJI[2].speedPID.output,1000);
        //CanTransmit_DJI_1234(&hcan1,0,0,0,0);
        //CanTransmit_DJI_1234(&hcan1,500,hDJI[1].speedPID.output,hDJI[2].speedPID.output,1000);
        osDelay(2);
    }
}

/**************************************************************
 * @brief 线程函数，底盘电机控制线程
 * 
 * @param arguement 
 */
void my_Chassis_Ctrl_Task(void *arguement)
{
    for(;;)
    {
        if(my_Alldir_Chassis_t.state == CHASSIS_HANDLE_RUNNING)
        {
            //850->2m/s
            my_Alldir_Chassis_t.target_v.vy = ((float)MyRemote_Data.usr_left_x)/283.3f ;
            my_Alldir_Chassis_t.target_v.vx = ((float)MyRemote_Data.usr_left_y)/283.3f * (-1.0f);
            my_Alldir_Chassis_t.target_v.vw = ((float)MyRemote_Data.usr_right_x)/230.0f;
        }
        Inverse_kinematic_equation(&my_Alldir_Chassis_t);
        osDelay(2);
    }
}

/**************************************************************
 * @brief 三全向轮底盘逆运动学解算
 * 
 * @param vx        底盘前向速度，单位m/s
 * @param vy        底盘左横向速度，单位m/s
 * @param wc        转向角速度，使用弧度，单位rad/s
 * @param out_v0    
 * @param out_v1    三个输出电机速度,单位rpm
 * @param out_v2 
 */
void Inverse_kinematic_equation(Alldir_Chassis_t *this_chassis)
{
    
    v0 = (float)((this_chassis->target_v.vy + this_chassis->target_v.vw * chassis_r) / chassis_R);
    v1 = (float)((-sqrt(3) * this_chassis->target_v.vx / 2 - this_chassis->target_v.vy / 2 + this_chassis->target_v.vw * chassis_r) / chassis_R);
    v2 = (float)((sqrt(3) * this_chassis->target_v.vx / 2 - this_chassis->target_v.vy / 2 + this_chassis->target_v.vw * chassis_r) / chassis_R);

    this_chassis->my_wheel[0].target_v = mps_to_rpm(v0);
    this_chassis->my_wheel[1].target_v = mps_to_rpm(v1);
    this_chassis->my_wheel[2].target_v = mps_to_rpm(v2);
}

/**************************************PID控制相关函数******************************************** */
/**
 * @brief       pid初始化
 * @param       KP  比例常数
 * @param       KI  积分常数
 * @param       KD  微分常数
 * @retval      无
 */
void chassis_pid_init(ChassisPID_t *upid, float KP, float KI, float KD)
{
    upid->ref         = 0;   /* 设定目标值 */
    upid->output      = 0.0; /* 期望输出值 */
    upid->integral    = 0.0; /* 积分值 */
    upid->cur_error   = 0.0; /* Error[1] */
    upid->error[0]    = 0.0; /* Error[-1] */
    upid->error[1]    = 0.0; /* Error[-2] */
    upid->KP          = KP;  /* 比例常数 Proportional Const */
    upid->KI          = KI;  /* 积分常数 Integral Const */
    upid->KD          = KD;  /* 微分常数 Derivative Const */
}

/**
 * @brief       底盘x,y坐标和偏航角pid闭环控制
 * @param       *PID：PID结构体变量地址
 * @param       Feedback_value：当前实际值
 * @retval      期望输出值
 */
float chassis_pid_calc(ChassisPID_t *upid, float Feedback_value)
{
    upid->cur_error = (float)(upid->ref - Feedback_value); /* 计算偏差 */

    upid->integral += upid->cur_error;
    upid->output        = (upid->KP * upid->cur_error)                        /* 比例环节 */
                        + (upid->KI * upid->error[0])                     /* 积分环节 */
                        + (upid->KD * (upid->cur_error - upid->error[0])); /* 微分环节 */
    upid->error[0] = upid->cur_error;
    
    return (upid->output); /* 返回计算后输出的数值 */
}



