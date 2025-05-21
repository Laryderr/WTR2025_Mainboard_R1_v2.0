#ifndef __HANDLE_TASK_H
#define __HANDLE_TASK_H
#ifdef __cplusplus
extern "C" {
#endif

#include "usermain.h"

enum Handle_State{
    HANDLE_IDLE_MODE = 0,   //空闲
    HANDLE_DUNK_MODE,       //扣篮
    HANDLE_INTERCEPT_MODE,  //拦截
    HANDLE_PATBALL_MODE,    //拍球
    HANDLE_SHOOT_MODE,      //投篮
};

extern enum Handle_State myHandle_State;   //手动模式状态
extern osThreadId_t handle_TaskHandle;

//外部接口函数
void my_handle_Task(void);

#ifdef __cplusplus
}
#endif
// C++
#endif //    