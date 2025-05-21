#ifndef _HANDLE_SHOOT_TASK_H
#define _HANDLE_SHOOT_TASK_H
#ifdef __cplusplus
extern "C" {
#endif

#include "usermain.h"
#include "DJI.h"

#define Rotation_r_Of_Ball 0.3987 //电机输出轴与球心距离，单位m
#define Deg_Ball_to_Arm 22.86 //输出轴对应铝管与球心线夹角 单位°
#define INITIAL_DELAY_TIME 2000//投篮臂自然下垂给定时间ms
#define Best_Throw_Degree 130 - 22.86 //输出轴最佳出手角度
#define Motor_to_Chassis_L 0.0395 //电机输出轴距离底盘中心水平距离
#define Motor_HeightLow   0 //三个高度挡位时输出轴离地高度
#define Motor_HeightMedium   0 
#define Motor_HeightHigh   0 

enum Shoot_Status{
    SHOOT_IDLE = 0,
    SHOOT_ING,
    SHOOT_STOP,//达到球所需出手角度
    SHOOT_FINISH,//完成投球
    DUNK_SHOOT, //配合扣篮
};

typedef struct{
    enum Shoot_Status myshoot_status;
    float target_throw_deg;//目标出手角度
    float target_throw_w;//目标出手角速度
    float Shootball_InitialPos[2];//投篮电机初始位置
    bool Foundshoot_InitialPos_Flag[2]; //是否找到投篮电机初始位置
    uint8_t Shoot_Completed_Flag; //投篮标志位

    float HorizontalDist_to_Basket;//篮球出手点距离篮筐中心水平距离
    float ShotTime_Ball_Heigh;//篮球中心出手离地高度
    
    
}Shoot_Task_T ;                                

extern osThreadId_t unitree_shoot_ctrl_TaskHandle;
extern Shoot_Task_T my_Shoot_Task_T;

void Handle_Shoot_TaskStart(void);

#ifdef __cplusplus
}
#endif
#endif 