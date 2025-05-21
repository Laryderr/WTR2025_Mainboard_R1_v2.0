#ifndef __ODOM_H
#define __ODOM_H

#include "usermain.h"
#include "ops_sdk.h"

extern float chassis_x_point; // 底盘纵向坐标
extern float chassis_y_point; // 底盘横向坐标

#define OPS_Calculate_Time 0.01 //码盘速度计算间隔时间,秒（Tim3定时器）
#define OPS_Calculate_htim htim3
#define OPS_Calculate_TIM  TIM3

//码盘速度
typedef struct 
{
   float last_pos[2];//1为x，2为y
   float current_pos[2];
   float current_v[2];
}Odom_v;

extern Odom_v my_Chassis_Odom_v;


void m_Chassis_Odom_Init(void);
void m_Chassis_Odom_TaskStart(void);
void m_Chassis_Odom_Task(void *argument);

#endif