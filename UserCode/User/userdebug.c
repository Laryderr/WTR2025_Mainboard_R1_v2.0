/**
 * @file userdebug.c
 * @author Lary (you@domain.com)
 * @brief  调车用
 *         任务堆栈监测功能未测试，也可以放在定时器中断执行
 * @version 0.1
 * @date 2024-11-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "userdebug.h"

/* 全局任务堆栈信息数组 */
/* 任务名称映射表 */
const char* taskNames[TASK_MAX_NUM] = {
    "DefaultTask",       // TASK_DEFAULT
    "RemoteCtrl",        // TASK_REMOTE_CTRL
    "ChassisCANMsg",     // TASK_CHASSIS_CAN_MSG
    "ChassisCtrl",       // TASK_CHASSIS_CTRL
    "UnitreeUARTMsg",    // TASK_UNITREE_UART_MSG
    "ChassisGyro",       // TASK_CHASSIS_GYRO
    "ExpansionExec",     // TASK_EXPANSION_EXECUTOR
    "ChassisOdom",       // TASK_CHASSIS_ODOM
    "HandleShoot",        // TASK_HANDLE_SHOOT
    "DebugTask",         // TASK_DEBUG
    "MainTask",          // TASK_MAIN
    "IWDGTask"           // TASK_IWDG
};
TaskStackInfo_t taskStackInfo[TASK_MAX_NUM] = {0};


/**********************************************************
 * @brief 线程定义
 * 
 */
osThreadId_t user_debug_TaskHandle;
const osThreadAttr_t user_debug_Task_attributes = {
    .name       = " user_debug_Task",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void my_debug_Task(void *argument);


/**************************************************************
 * @brief   调试启动线程
 */
void my_debug_TaskStart(void)
{
    user_debug_TaskHandle = osThreadNew(my_debug_Task, NULL, &user_debug_Task_attributes);
}


/**
 * @brief 初始化任务堆栈监测信息
 */
void InitTaskStackMonitor(void)
{
    // 初始化默认任务信息（StartDefaultTask）
    taskStackInfo[TASK_DEFAULT].taskHandle = xTaskGetHandle("DefaultTask");
    strncpy(taskStackInfo[TASK_DEFAULT].taskName, "DefaultTask", sizeof(taskStackInfo[TASK_DEFAULT].taskName) - 1);
    taskStackInfo[TASK_DEFAULT].stackSize = 2048; // 根据实际分配调整
    taskStackInfo[TASK_DEFAULT].minRemainingStack = 0;
    taskStackInfo[TASK_DEFAULT].stackUsagePercent = 0;
    
    // 初始化调试任务信息（my_debug_Task）
    taskStackInfo[TASK_DEBUG].taskHandle = xTaskGetHandle("DebugTask");
    strncpy(taskStackInfo[TASK_DEBUG].taskName, "DebugTask", sizeof(taskStackInfo[TASK_DEBUG].taskName) - 1);
    taskStackInfo[TASK_DEBUG].stackSize = 256*4; // 根据实际分配调整
    taskStackInfo[TASK_DEBUG].minRemainingStack = 0;
    taskStackInfo[TASK_DEBUG].stackUsagePercent = 0;
    
    // 可以继续初始化其他任务信息...
    // 注意：需要先获取各任务的实际句柄和分配的堆栈大小
}

/**
 * @brief 检测所有任务的剩余堆栈情况
 * @return bool 0:正常 1:存在堆栈溢出风险
 */
bool CheckAllTasksStackUsage(void)
{
    bool riskFlag = 0;// 0:正常 1:存在堆栈溢出风险
    UBaseType_t highWaterMark;
    
    for (int i = 0; i < TASK_MAX_NUM; i++) {
        if (taskStackInfo[i].taskHandle != NULL) {
            // 获取任务堆栈高水位线值[6,7]
            highWaterMark = uxTaskGetStackHighWaterMark(taskStackInfo[i].taskHandle);
            taskStackInfo[i].minRemainingStack = highWaterMark;
            
            // 计算堆栈使用百分比
            if (taskStackInfo[i].stackSize > 0) {
                taskStackInfo[i].stackUsagePercent = 100 - 
                    ((highWaterMark * 100) / taskStackInfo[i].stackSize);
                
                // 检查堆栈溢出风险（使用率超过80%或剩余堆栈接近0）
                if (highWaterMark < 10 || taskStackInfo[i].stackUsagePercent > 80) {
                    riskFlag = 1;
                }
            }
        }
    }
    
    return riskFlag;
}



/**
 * @brief 调试线程实现
 * 
 * @param argument 
 */
void my_debug_Task(void *argument)
{
    // 初始化堆栈监测
    InitTaskStackMonitor();
    for(; ;)
    {
        // 定期检查堆栈使用情况（例如每10秒检查一次）
        static uint32_t stackCheckTimer = 0;
        if (osKernelGetTickCount() - stackCheckTimer > 10000) {
            stackCheckTimer = osKernelGetTickCount();
            
            if (CheckAllTasksStackUsage() != 0) {
                // 存在堆栈溢出风险，可以采取相应措施
            }
        }
        
        osDelay(1);
        //调试底盘3508PID
        /*if(BtnScan_Press(MyRemote_Data.btn_LeftCrossUp,MyLastRemote_Data.btn_LeftCrossUp))
        {
            for (int i = 0; i < 4; i++) {
            hDJI[i].speedPID.KP = hDJI[i].speedPID.KP + 0.05;
            }    
        }

        if(BtnScan_Press(MyRemote_Data.btn_LeftCrossMid,MyLastRemote_Data.btn_LeftCrossMid))
        {
            for (int i = 0; i < 4; i++) {
            hDJI[i].speedPID.KI = hDJI[i].speedPID.KI + 0.01;
            }    
        }

        if(BtnScan_Press(MyRemote_Data.btn_LeftCrossDown,MyLastRemote_Data.btn_LeftCrossDown))
        {
            for (int i = 0; i < 4; i++) {
            hDJI[i].speedPID.KD = hDJI[i].speedPID.KD + 0.05;
            }    
        }

        if (BtnScan_Press(MyRemote_Data.btn_RightCrossUp,MyLastRemote_Data.btn_RightCrossUp))
        {
            for (int i = 0; i < 4; i++) {
            hDJI[i].speedPID.KP = hDJI[i].speedPID.KP - 0.05;
            }   
        }

        if (BtnScan_Press(MyRemote_Data.btn_RightCrossMid,MyLastRemote_Data.btn_RightCrossMid))
        {
            for (int i = 0; i < 4; i++) {
            hDJI[i].speedPID.KI = hDJI[i].speedPID.KI - 0.01;
            }   
        }

        if (BtnScan_Press(MyRemote_Data.btn_RightCrossDown,MyLastRemote_Data.btn_RightCrossDown))
        {
            for (int i = 0; i < 4; i++) {
            hDJI[i].speedPID.KD= hDJI[i].speedPID.KD - 0.05;
            }   
        }

        //调试宇树PD
        if (BtnScan_Press(MyRemote_Data.btn_LeftCrossLeft,MyLastRemote_Data.btn_LeftCrossLeft))
        {
            for (uint8_t i = 0; i <3; i++)
            {
                unitree_DunkMotor_t[i].cmd.K_P = unitree_DunkMotor_t[i].cmd.K_P + 0.02f;
            }
        }

        if (BtnScan_Press(MyRemote_Data.btn_LeftCrossRight,MyLastRemote_Data.btn_LeftCrossRight))
        {
            for (uint8_t i = 0; i <3; i++)
            {
                unitree_DunkMotor_t[i].cmd.K_W = unitree_DunkMotor_t[i].cmd.K_W + 0.02f;
            }
        }

        if (BtnScan_Press(MyRemote_Data.btn_RightCrossLeft,MyLastRemote_Data.btn_RightCrossLeft))
        {
            for (uint8_t i = 0; i <3; i++)
            {
                unitree_DunkMotor_t[i].cmd.K_P = unitree_DunkMotor_t[i].cmd.K_P - 0.02f;
            }
        }

        if (BtnScan_Press(MyRemote_Data.btn_RightCrossRight,MyLastRemote_Data.btn_RightCrossRight))
        {
            for (uint8_t i = 0; i <3; i++)
            {
                unitree_DunkMotor_t[i].cmd.K_W = unitree_DunkMotor_t[i].cmd.K_W - 0.02f;
            }
        }*/

    }
}