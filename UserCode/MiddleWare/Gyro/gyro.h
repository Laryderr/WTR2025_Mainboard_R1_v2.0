#ifndef __GYRO_H
#define __GYRO_H

#include "usermain.h"
#include "HWT101CT_sdk.h"

extern uint8_t chassis_gyro_state;
extern float chassis_offset;
extern float chassis_yaw;
extern uint8_t chassis_gyro_init_flag; 

/*typedef struct 
{
    bool chassis_gyro_init_flag;
    bool chassis_gyro_state;
    float chassis_offset;
    float chassis_yaw;
}My_Chassis_Gyro_t;*/


void m_Chassis_Gyro_Init(void);
void m_Chassis_Gyro_TaskStart(void);
void m_Chassis_Gyro_Task(void *argument);

#endif