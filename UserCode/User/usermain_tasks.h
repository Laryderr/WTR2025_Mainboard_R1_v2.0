#ifndef __USERMAIN_TASKS_H
#define __USERMAIN_TASKS_H
#ifdef __cplusplus
extern "C" {
#endif
// C
#include "usermain.h"

enum Running_Status{
    AUTO_MODE = 0,
    HANDLE_MODE = 1,
};

/*enum Handle_State{
    HANDLE_IDLE_MODE = 0,   //空闲
    HANDLE_DUNK_MODE,       //扣篮
    HANDLE_INTERCEPT_MODE,  //拦截
    HANDLE_PATBALL_MODE,    //拍球
    HANDLE_SHOOT_MODE,      //投篮
};*/

extern enum Running_Status running_status;          //暂时不用

//extern  Alldir_Chassis_t my_Alldir_Chassis_t;        //整车底盘对象



// 接口函数

int motor_wait_to_finish(float feedback, float setpoint);
void my_main_Task_Start(void);


#ifdef __cplusplus
}
#endif
// C++
#endif