/**
 * @file Auto_Shoot_task.c
 * @author Lary (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-07-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "Auto_Shoot_task.h"

Auto_Shoot_Task_T my_Auto_Shoot_Task_T;

 /****************************************************线程定义******************************************/
// osThreadId_t auto_shoot_ctrl_TaskHandle;
// const osThreadAttr_t auto_shoot_ctrl_Task_attributes = {
//     .name       = "auto_shoot_ctrl_Task",
//     .stack_size = 128*4,
//     .priority   = (osPriority_t)osPriorityNormal,
// };
// void Auto_Shoot_Task(void *argument);


// /**
//  * @brief 自动多点位投篮控制线程创建
//  * 
//  */
// void Auto_Shoot_TaskStart(void)
// {
//     unitree_shoot_ctrl_TaskHandle = osThreadNew(Auto_Shoot_Task,NULL,&auto_shoot_ctrl_Task_attributes);
// }
/****************************************************************************************************** */

void train_45dgree_equation()
{

}

// // Degree计算（线性）
// float Calc_Degree(float distance){
//     return 7.5f * distance + 53.75f;
// }

/**
 * @brief 
 * 
 * @param distance 
 * @return double 
 */
double Calc_Degree(double distance) {
    // 定义数据点（按distance升序排列）
    struct DataPoint {
        double distance;
        double degree;
    } points[] = {
        {1.3, 62.0}, {1.4, 62.75}, {1.5, 63.5}, {1.6, 64.25}, 
        {1.7, 65.0}, {1.8, 65.75}, {1.9, 66.5}, {2.0, 67.25},
        {2.1, 68.0}, {2.2, 68.75}, {2.3, 69.5}, {2.4, 69.6},
        {2.5, 69.8}, {2.6, 68.0}, {2.7, 67.5}, {2.8, 67.0},
        {2.9, 68.0}, {3.0, 69.0}, {3.1, 70.0}, {3.2, 71.0}
    };
    const int num_points = sizeof(points) / sizeof(points[0]);

    // 边界检查
    if (distance <= points[0].distance) return points[0].degree;
    if (distance >= points[num_points-1].distance) return points[num_points-1].degree;

    // 查找最近的区间
    for (int i = 0; i < num_points - 1; i++) {
        if (distance >= points[i].distance && distance <= points[i+1].distance) {
            // 线性插值
            double ratio = (distance - points[i].distance) / 
                          (points[i+1].distance - points[i].distance);
            return points[i].degree + ratio * (points[i+1].degree - points[i].degree);
        }
    }

    return 0.0; 
}


/**
 * @brief 计算五次多项式函数值
 * @param x 自变量x
 * @return double 函数值y
 */
double Calc_KW(double distance) {
    return (((((0.0432 * distance - 0.5174) * distance + 2.5375) * distance - 6.5249) * distance + 9.2914) * distance - 6.9473) * distance + 3.0128;
}

/**
 * @brief 单个投篮点位设置
 * 
 * @param id 
 * @param x 
 * @param y 
 */
void Shoot_point_Init(uint8_t id,float x,float y)
{
    my_Auto_Shoot_Task_T.Auto_shoot_point[id][1] = x;
    my_Auto_Shoot_Task_T.Auto_shoot_point[id][2] = y;
}


void Auto_Shoot_Task(void*argument)
{
    // my_Auto_Shoot_Task_T.now_train_distance = 0;
    // my_Auto_Shoot_Task_T.train_step = 0.2;
    // my_Auto_Shoot_Task_T.min_KW = 0.0f;
    // my_Auto_Shoot_Task_T.max_KW = 0.0f;
    for (;;)
    {
        my_Auto_Shoot_Task_T.now_train_distance = MIN_TRAIN_DISTANCE + MyRemote_Data.btn_LeftCrossRight_press_count*my_Auto_Shoot_Task_T.train_step;
        my_Auto_Shoot_Task_T.train_point[0] = BASKET_X - sin(PI/4)*my_Auto_Shoot_Task_T.now_train_distance;
        my_Auto_Shoot_Task_T.train_point[1] = BASKET_Y + sin(PI/4)*my_Auto_Shoot_Task_T.now_train_distance;
        osDelay(2);
    }
}
