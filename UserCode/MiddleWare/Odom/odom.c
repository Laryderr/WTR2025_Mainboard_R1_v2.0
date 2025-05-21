#include "odom.h"

float chassis_x_point = 0; // 底盘纵向坐标
float chassis_y_point = 0; // 底盘横向坐标*/
Odom_v my_Chassis_Odom_v;

osThreadId_t chassis_odom_TaskHandle;
const osThreadAttr_t chassis_odom_Task_attributes = {
    .name       = "chassis_odom_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal7,
};
void m_Chassis_Odom_Task(void *argument);

/**
 * @brief   码盘初始化
 */
void m_Chassis_Odom_Init(void)
{
    OPS_Init();
    my_Chassis_Odom_v.current_pos[0] = 0;
    my_Chassis_Odom_v.current_pos[1] = 0;
    my_Chassis_Odom_v.last_pos[0] = 0;
    my_Chassis_Odom_v.last_pos[1] = 0;
    my_Chassis_Odom_v.current_v[0] = 0;
    my_Chassis_Odom_v.current_v[1] = 0;
}

/**
 * @brief   码盘线程开启
 */
void m_Chassis_Odom_TaskStart(void)
{
    chassis_odom_TaskHandle = osThreadNew(m_Chassis_Odom_Task, NULL, &chassis_odom_Task_attributes);
}

/**
 * @brief   码盘线程
 */
void m_Chassis_Odom_Task(void *argument)
{
    for (;;) {
        chassis_x_point = -OPS_Data.pos_y;
        chassis_y_point = OPS_Data.pos_x;
        osDelay(1);
    }
}