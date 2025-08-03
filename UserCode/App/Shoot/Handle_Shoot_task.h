#ifndef _HANDLE_SHOOT_TASK_H
#define _HANDLE_SHOOT_TASK_H
#ifdef __cplusplus
extern "C" {
#endif

#include "usermain.h"
#include "DJI.h"

#define Rotation_r_Of_Ball 0.3987 //电机输出轴与球心距离，单位m
#define Deg_Ball_to_Arm 22.86 //输出轴对应铝管与球心线夹角 单位°
#define INITIAL_DELAY_TIME 1000//投篮臂自然下垂给定时间ms
#define Best_Throw_Degree 130 - 22.86 //输出轴最佳出手角度
#define Motor_to_Chassis_L 0.0395 //电机输出轴距离底盘中心水平距离
#define Motor_HeightLow   0 //低挡位时输出轴离地高度
#define Motor_HeightMedium   0 //中挡位时输出轴离地高度
#define Motor_HeightHigh   0 //高挡位时输出轴离地高度

enum Shoot_Status{
    SHOOT_IDLE = 0,
    SHOOT_ING,
    SHOOT_STOP,//达到球所需出手角度
    SHOOT_FINISH,//完成投球
    DUNK_SHOOT, //配合扣篮
    SHOOT_BACK_TO_INIT,//回转
};
typedef struct 
{
    float actual_xpos;
    float actual_ypos;
    float distance;
    float X_offset;
    float Y_offset;
}Auto_Shoot_Point;


typedef struct{
    enum Shoot_Status myshoot_status;
    enum Shoot_Status mylastshoot_status;
    float target_throw_deg;//目标出手角度
    float target_throw_w;//目标出手角速度
    float Shootball_InitialPos[2];//投篮电机初始位置
    bool Foundshoot_InitialPos_Flag[2]; //是否找到投篮电机初始位置
    uint8_t Shoot_Completed_Flag; //投篮标志位

    float HorizontalDist_to_Basket;//篮球出手点距离篮筐中心水平距离
    float ShotTime_Ball_Heigh;//篮球中心出手离地高度
    uint8_t shoot_point; //出手点id
    uint8_t last_shoot_point;//上一出手点id

    bool camera_aim_flag;
    float camera_aim_angle;
    bool camera_get_angle_flag;
    PID_t Encoder_SpeedPID_t; //投篮臂角编码器速度PID结构体
    uint8_t shoot_count;//投篮总次数
    float model_calc_degree;
    float model_calc_KW;

    Auto_Shoot_Point Auto_shoot_point[8]; //七个点
    bool on_shoot_point;//是否覆盖在投篮点标志
    float dx; //车身点距离投篮点的x差
    float dy; //车身点距离投篮点的y差

    
}Shoot_Task_T ;                                

extern osThreadId_t unitree_shoot_ctrl_TaskHandle;
extern Shoot_Task_T my_Shoot_Task_T;

void Handle_Shoot_TaskStart(void);
void Ball_Hold(float W);

#ifdef __cplusplus
}
#endif
#endif 