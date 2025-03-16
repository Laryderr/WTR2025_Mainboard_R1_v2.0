#ifndef _HANDLE_SHOOT_TASK_H
#define _HANDLE_SHOOT_TASK_H
#ifdef __cplusplus
extern "C" {
#endif

#include "usermain.h"

#define Rotation_r_Of_Ball 0.3987 //电机输出轴与球心距离，单位m
#define Deg_Ball_to_Arm 22.86 //输出轴对应铝管与球心线夹角 单位°
#define INITIAL_DELAY_TIME 2000//投篮臂自然下垂给定时间ms


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
}Shoot_Task_T ;                                

extern osThreadId_t unitree_shoot_ctrl_TaskHandle;
void Handle_Shoot_TaskStart(void);

#ifdef __cplusplus
}
#endif
#endif 