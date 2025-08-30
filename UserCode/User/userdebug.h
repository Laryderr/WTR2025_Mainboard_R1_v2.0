#ifndef __USERDEBUG_H
#define __USERDEBUG_H
#ifdef __cplusplus
extern "C"{
#endif 

#include "usermain.h"
#include "FreeRTOS.h"
#include "task.h"

/* 任务堆栈信息结构体 */
typedef struct {
    TaskHandle_t taskHandle;    // 任务句柄
    char taskName[20];          // 任务名称
    UBaseType_t stackSize;      // 任务分配的堆栈大小（单位：字）
    UBaseType_t minRemainingStack; // 历史最小剩余堆栈（高水位线值）
    uint8_t stackUsagePercent;  // 堆栈使用百分比
} TaskStackInfo_t;

/* 需要监测的任务列表ID */
typedef enum {
    TASK_DEFAULT = 0,
    TASK_REMOTE_CTRL,
    TASK_CHASSIS_CAN_MSG,
    TASK_CHASSIS_CTRL,
    TASK_UNITREE_UART_MSG,
    TASK_CHASSIS_GYRO,
    TASK_EXPANSION_EXECUTOR,
    TASK_CHASSIS_ODOM,
    TASK_HANDLE_SHOOT,
    TASK_DEBUG,
    TASK_MAIN,
    TASK_IWDG,
    TASK_MAX_NUM
} TaskID_t;

// extern TaskStackInfo_t taskStackInfo[TASK_MAX_NUM];





void my_debug_TaskStart(void);


#ifdef __cplusplus
}
#endif
#endif