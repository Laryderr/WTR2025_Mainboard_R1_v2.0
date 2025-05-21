/**
 * @file chassis_ctrl.h
 * @author lary (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef  CHASSIS_CTRL_H
#define CHASSIS_CTRL_H
#ifdef __cplusplus
extern "C" {
#endif

#include "usermain.h"

#include "DJI.h"
#include "Caculate.h"
#include "math.h"
#include "wtr_can.h"

#define FORWARD_ANGLE -77

/**
 * @brief 底盘状态机
 * 
 */
typedef enum{
    CHASSIS_HANDLE_RUNNING,
    CHASSIS_AUTO_RUNNING,
    CHASSIS_STOP,
    CHASSIS_READY,
    CHASSIS_ERROR,
    CHASSIS_NULL}Chassis_state_e;  

/**
 * @brief 全向轮状态机
 * 
 */
typedef enum {
    WHEEL_RUNNING,   //运行
    WHEEL_CORRECTING,//校正
    WHEEL_READY,
    WHEEL_STOP,
    WHEEL_ERROR} Alldir_wheel_state_e; 

/**
 * @brief 底盘PID结构体
 * 
 */
typedef struct
{
    float KP;        // PID参数P
    float KI;        // PID参数I
    float KD;        // PID参数D
    float fdb;       // PID反馈值
    float ref;       // PID目标值
    float cur_error; // 当前误差
    float error[2];  // 前两次误差
    float output;    // 输出值
    float outputMax; // 最大输出值的绝对值
    float outputMin; // 最小输出值的绝对值用于防抖
    float integral;  // 积分值
    float integralMax; // 积分最大值
    float integralMin; // 积分最小值
} ChassisPID_t;

/**
 * @brief 底盘速度结构体
 * 
 */
typedef struct 
{
    float vx;
    float vy;
    float vw;
}Chassis_velocity_t;

typedef struct 
{
    float xpos;
    float ypos;
    float yawpos;
    float yaw_offset;
}Chassis_pos_t;

/**
 * @brief 全向轮结构体
 * 
 */
typedef struct 
{
    uint8_t id;                 //每个全向轮id
    Alldir_wheel_state_e state; //全向轮当前状态
    float target_v;             //遥控器 / 上位机获取,单位rpm
    float current_v;            //码盘/电机编码器获取
    int16_t correcting_stage;
}Alldir_wheel_t;


typedef void (*__XPosServo)(float);
typedef void (*__YPosServo)(float);
typedef void (*__YAWPosServo)(float);
typedef void (*__AllPosServo)(float, float, float);


/**
 * @brief 整车底盘结构体
 * 
 */
typedef struct 
{
    //整车位置PID对象
    ChassisPID_t chassis_yawpos_pid;
    ChassisPID_t chassis_xpos_pid;
    ChassisPID_t chassis_ypos_pid;

    //整车速度PID对象
    ChassisPID_t chassis_vx_pid;
    ChassisPID_t chassis_vy_pid;
    ChassisPID_t chassis_vw_pid;

    Chassis_velocity_t target_v;
    Chassis_velocity_t current_v;//码盘获取，m/s

    Chassis_pos_t target_pos;
    Chassis_pos_t current_pos;
    Chassis_pos_t last_pos;
    float init_x_pos;
    float init_y_pos;
    bool chassis_calibrate_flag;

    float chassis_to_basket; //底盘中心到篮筐中心水平距离

    Chassis_state_e state;
    Alldir_wheel_t my_wheel[3];

    //整车位置伺服函数
    __XPosServo XPosServo;
    __YPosServo YPosServo;
    __YAWPosServo YAWPosServo;
    __AllPosServo AllPosServo; 

}Alldir_Chassis_t;

//extern  wheel0_velocity, wheel1_velocity, wheel2_velocity;    //底盘目标电机速度
extern  Alldir_Chassis_t my_Alldir_Chassis_t;        //整车底盘对象

//外部接口函数
void my_Chassis_Init(void);
void my_Chassis_CAN_Message_TaskStart(void);
void my_Chassis_Ctrl_TaskStart(void);

void chassis_pid_init(ChassisPID_t *upid,float ref, float KP, float KI, float KD);
float chassis_pid_calc(ChassisPID_t *upid, float Feedback_value);


void Inverse_kinematic_equation(Alldir_Chassis_t *this_chassis);
void Forward_kinematics_equation(Alldir_Chassis_t *this_chassis);

void chassis_XPoseServo_calc(float ref);
void chassis_YPoseServo_calc(float ref);
void chassis_YAWPoseServo_calc(float ref);

#ifdef __cplusplus
}
#endif
#endif // ! CLASSIS_CTRL_H


