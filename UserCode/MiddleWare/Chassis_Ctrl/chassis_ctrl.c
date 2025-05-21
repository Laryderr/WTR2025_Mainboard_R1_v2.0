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
 * @note 底盘x，y由上位机雷达获取，yaw由雷达/陀螺仪获取，w由陀螺仪获取
 *        
 * @copyright Copyright (c) 2024
 * 
 */

#include "chassis_ctrl.h"
bool YawPosServo_Flag = 0;
float test1=0;

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



/**************************************PID控制相关函数******************************************** */
/**
 * @brief       pid初始化
 * @param       KP  比例常数
 * @param       KI  积分常数
 * @param       KD  微分常数
 * @retval      无
 */
void chassis_pid_init(ChassisPID_t *upid,float ref, float KP, float KI, float KD)
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
    upid->outputMax   = 5;
    upid->outputMin   = -5;
}

/**
 * @brief       底盘闭环控制
 * @param       *PID：PID结构体变量地址
 * @param       Feedback_value：当前实际值
 * @retval      期望输出值
 */
float chassis_pid_calc(ChassisPID_t *upid, float Feedback_value)
{
    upid->cur_error = (float)(upid->ref - Feedback_value); /* 计算偏差 */

    upid->integral += upid->cur_error;
    upid->output        = (upid->KP * upid->cur_error)                        /* 比例环节 */
                        + (upid->KI * upid->integral)                     /* 积分环节 */
                        + (upid->KD * (upid->cur_error - upid->error[0])); /* 微分环节 */
    upid->error[0] = upid->cur_error;
    
    if(upid->output >= upid->outputMax) upid->output = upid->outputMax;
    if(upid->output <= upid->outputMin) upid->output = upid->outputMin;

    return (upid->output); /* 返回计算后输出的数值 */
}

/**
 * @brief 位置闭环控制
 * 
 * @param ref 
 */
void chassis_XPoseServo_calc(float ref)
{
    my_Alldir_Chassis_t.chassis_xpos_pid.ref = ref;
    my_Alldir_Chassis_t.chassis_xpos_pid.fdb = my_Alldir_Chassis_t.current_pos.xpos;
    my_Alldir_Chassis_t.target_v.vx = -chassis_pid_calc(&my_Alldir_Chassis_t.chassis_xpos_pid,my_Alldir_Chassis_t.chassis_xpos_pid.fdb);
    
    //暂时不放内环
    /*my_Alldir_Chassis_t.chassis_vx_pid.ref = upid->output;
    chassis_pid_calc(&my_Alldir_Chassis_t.chassis_vx_pid,my_Alldir_Chassis_t.current_v.vx);*/
}

void chassis_YPoseServo_calc(float ref)
{
    my_Alldir_Chassis_t.chassis_ypos_pid.ref = ref;
    my_Alldir_Chassis_t.chassis_ypos_pid.fdb = my_Alldir_Chassis_t.current_pos.ypos;
    my_Alldir_Chassis_t.target_v.vy = chassis_pid_calc(&my_Alldir_Chassis_t.chassis_ypos_pid,my_Alldir_Chassis_t.chassis_ypos_pid.fdb);
    
    //暂时不放内环
    /*my_Alldir_Chassis_t.chassis_vx_pid.ref = upid->output;
    chassis_pid_calc(&my_Alldir_Chassis_t.chassis_vx_pid,my_Alldir_Chassis_t.current_v.vx);*/
}

void chassis_YAWPoseServo_calc(float ref)
{
    my_Alldir_Chassis_t.chassis_yawpos_pid.ref = ref;
    my_Alldir_Chassis_t.chassis_yawpos_pid.fdb = my_Alldir_Chassis_t.current_pos.yawpos;
    chassis_pid_calc(&my_Alldir_Chassis_t.chassis_yawpos_pid,my_Alldir_Chassis_t.chassis_yawpos_pid.fdb);
    
    my_Alldir_Chassis_t.chassis_vw_pid.ref = my_Alldir_Chassis_t.chassis_yawpos_pid.output;
    my_Alldir_Chassis_t.target_v.vw = my_Alldir_Chassis_t.chassis_yawpos_pid.output;
}

/**
 * @brief 全位置伺服
 * 
 * @param ref_x 
 * @param ref_y 
 * @param ref_yaw 
 */
void All_Pose_Servo(float ref_x,float ref_y,float ref_yaw)
{
    chassis_XPoseServo_calc(ref_x);
    chassis_YPoseServo_calc(ref_y);
    chassis_YAWPoseServo_calc(ref_yaw);
}


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

    my_Alldir_Chassis_t.XPosServo = chassis_XPoseServo_calc;
    my_Alldir_Chassis_t.YPosServo = chassis_YPoseServo_calc;
    my_Alldir_Chassis_t.YAWPosServo = chassis_YAWPoseServo_calc;
    my_Alldir_Chassis_t.AllPosServo = All_Pose_Servo;

    for (int i = 0; i < 4; i++){
        hDJI[i].motorType = M3508;
    }
    DJI_Init();
    for (int i = 0; i < 4; i++) {
        hDJI[i].posPID.KI=1.5;
        hDJI[i].posPID.KD=0;
        hDJI[i].speedPID.outputMax=5000;
        hDJI[i].speedPID.KP=6;
        hDJI[i].speedPID.KI=0.01;
        hDJI[i].speedPID.KD=0.3;
    } 
    //底盘速度pid初始化
    chassis_pid_init(&my_Alldir_Chassis_t.chassis_vx_pid,my_Alldir_Chassis_t.target_v.vx,0,0,0);
    chassis_pid_init(&my_Alldir_Chassis_t.chassis_vy_pid,my_Alldir_Chassis_t.target_v.vy,0,0,0);
    chassis_pid_init(&my_Alldir_Chassis_t.chassis_vw_pid,my_Alldir_Chassis_t.target_v.vw,0.1,0,0);
    //底盘位置pid初始化
    chassis_pid_init(&my_Alldir_Chassis_t.chassis_xpos_pid,my_Alldir_Chassis_t.target_pos.xpos,5,0,0.1);
    chassis_pid_init(&my_Alldir_Chassis_t.chassis_ypos_pid,my_Alldir_Chassis_t.target_pos.ypos,5,0,0.1);
    chassis_pid_init(&my_Alldir_Chassis_t.chassis_yawpos_pid,my_Alldir_Chassis_t.target_pos.yawpos,0.2,0,0.02);
    //底盘位置初始化
    my_Alldir_Chassis_t.current_pos.xpos = 0;
    my_Alldir_Chassis_t.current_pos.ypos = 0;
    my_Alldir_Chassis_t.current_pos.yawpos = 0;

    my_Alldir_Chassis_t.last_pos.xpos = 0;
    my_Alldir_Chassis_t.last_pos.ypos = 0;
    my_Alldir_Chassis_t.last_pos.yawpos = 0;
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
        speedServo(my_Alldir_Chassis_t.my_wheel[1].target_v * hDJI[1].reductionRate,&hDJI[1]);
        speedServo(my_Alldir_Chassis_t.my_wheel[2].target_v * hDJI[2].reductionRate,&hDJI[2]);
        CanTransmit_DJI_1234(&hcan1,hDJI[0].speedPID.output,hDJI[1].speedPID.output,hDJI[2].speedPID.output,0);
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
    
    my_Alldir_Chassis_t.init_x_pos = 0;
    for(;;)
    {
        //位置获取
        if(my_Alldir_Chassis_t.chassis_calibrate_flag == 0){
            //DT35初始位置校准
            my_Alldir_Chassis_t.current_pos.xpos = -Laser_x ;
            my_Alldir_Chassis_t.current_pos.ypos = -Laser_y ;
        }else{
            //激光雷达里程计
            my_Alldir_Chassis_t.current_pos.xpos = my_Alldir_Chassis_t.init_x_pos - Lidar_pose[0];
            my_Alldir_Chassis_t.current_pos.ypos = my_Alldir_Chassis_t.init_y_pos - Lidar_pose[1];
        }

        if(my_Alldir_Chassis_t.state == CHASSIS_HANDLE_RUNNING)
        {
            //初始化位置校准
            /*if (my_Alldir_Chassis_t.chassis_calibrate_flag == 0)
            {
                my_Alldir_Chassis_t.YAWPosServo(FORWARD_ANGLE);
                if (my_Alldir_Chassis_t.current_pos.yawpos <= (FORWARD_ANGLE+1)&&my_Alldir_Chassis_t.current_pos.yawpos >= (FORWARD_ANGLE-1))
                {
                    uint32_t start_time = HAL_GetTick();
                    if (HAL_GetTick() - start_time >= 1000) 
                    {
                        //注意加入遥控器反馈
                        my_Alldir_Chassis_t.chassis_calibrate_flag = 1;
                        my_Alldir_Chassis_t.init_x_pos = Laser_x;
                        my_Alldir_Chassis_t.init_y_pos = -Laser_y;
                    }
                }
            }*/

            //850->2m/s
            //遥控器目标速度获取
            my_Alldir_Chassis_t.target_v.vy = ((float)MyRemote_Data.usr_left_x)/283.3f*2.5f ;
            my_Alldir_Chassis_t.target_v.vx = ((float)MyRemote_Data.usr_left_y)/283.3f * (-1.0f)*2.5f;
            my_Alldir_Chassis_t.target_v.vw = ((float)MyRemote_Data.usr_right_x)/230.0f*2.0f;
            chassis_pid_calc(&my_Alldir_Chassis_t.chassis_vw_pid,my_Alldir_Chassis_t.current_v.vw);
            my_Alldir_Chassis_t.target_v.vw += my_Alldir_Chassis_t.chassis_vw_pid.output;
            //my_Alldir_Chassis_t.target_v.vw = ((float)MyRemote_Data.usr_right_x)/230.0f*2.0f + 
            //chassis_pid_calc(&my_Alldir_Chassis_t.chassis_vw_pid,my_Alldir_Chassis_t.current_v.vw);//航向角速度闭环减少打滑
        }else if(my_Alldir_Chassis_t.state == CHASSIS_AUTO_RUNNING)
        {
            my_Alldir_Chassis_t.YAWPosServo(FORWARD_ANGLE);
            //my_Alldir_Chassis_t.XPosServo(0.5);
            //my_Alldir_Chassis_t.YPosServo(0.5);
        }

        //底盘运动学逆解算
        Inverse_kinematic_equation(&my_Alldir_Chassis_t);

        //获取底盘速度值,注意码盘坐标和底盘坐标的转换(暂时不用)
        /*my_Alldir_Chassis_t.current_v.vy = -my_Chassis_Odom_v.current_v[0]/1000;
        my_Alldir_Chassis_t.current_v.vx = my_Chassis_Odom_v.current_v[1]/1000;*/
        my_Alldir_Chassis_t.current_v.vw = -gyrodata[1]/360*2*PI;

        my_Alldir_Chassis_t.chassis_to_basket = 0;

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
    float cps_vx,cps_vy,cps_vw;
    //暂时不需要x，y速度闭环
    cps_vx = this_chassis->target_v.vx ;
    cps_vy = this_chassis->target_v.vy ;
    cps_vw = this_chassis->target_v.vw ;

    /*cps_vx = this_chassis->target_v.vx ;
    cps_vy = this_chassis->target_v.vy ;
    cps_vw = this_chassis->target_v.vw ;*/

    v0 = (float)((cps_vy + cps_vw * chassis_r) / chassis_R);
    v1 = (float)((-sqrt(3) * cps_vx / 2 - cps_vy / 2 + cps_vw * chassis_r) / chassis_R);
    v2 = (float)((sqrt(3) * cps_vx / 2 - cps_vy / 2 + cps_vw * chassis_r) / chassis_R);

    this_chassis->my_wheel[0].target_v = mps_to_rpm(v0);
    this_chassis->my_wheel[1].target_v = mps_to_rpm(v1);
    this_chassis->my_wheel[2].target_v = mps_to_rpm(v2);
}






