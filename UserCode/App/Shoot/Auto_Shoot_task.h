#ifndef AUTO_SHOOT_TASK_H
#define AUTO_SHOOT_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "usermain.h"
#include "math.h"
#include "stdio.h"

#define MIN_TRAIN_DISTANCE 1.3 
#define MAX_TRAIN_DISTANCE 5





typedef struct Auto_Shoot_Task_T
{
    float Auto_shoot_point[10][3]; //投篮点位 点位id(从1开始) + 1X 2Y坐标、3离篮筐距离
    float test_KW;
    float now_train_distance;
    float train_step;
    float train_point[2];
    float min_KW;
    float max_KW;
    float KW01;
    float KW001;
    float KW0001;
    float KW00001;
    

}Auto_Shoot_Task_T;

extern Auto_Shoot_Task_T my_Auto_Shoot_Task_T;

void Auto_Shoot_TaskStart(void);

double Calc_Degree(double distance);
double Calc_KW(double distance);

#ifdef __cplusplus
}
#endif
#endif // !