#ifndef __UNITREE_H
#define __UNITREE_H
#ifdef __cplusplus
extern "C" {
#endif

#include "usermain.h"
#include "Unitree_user.h"


extern UnitreeMotor unitree_DunkMotor_t[16];   //底盘跳跃电机对象创建

extern float unitree_DunkMotor_Tff[4];         //前馈力矩
extern float uinitree_DunkMotor_posdes[3];        //电机输出轴输出位置（-65051.18 ~ 65051.18  单位 rad）
extern float unitree_DunkMotor_kp[3];          //电机刚度系数/位置误差比例系数（0 ~ 25.599）
extern float unitree_DunkMotor_kw[3];          //电机阻尼系数/速度误差比例（0 ~ 25.599）

void my_Unitree_Init(void);
void my_Unitree_UART_Message_TaskStart(void);


#ifdef __cplusplus
}
#endif

#endif