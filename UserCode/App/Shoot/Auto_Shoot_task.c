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
