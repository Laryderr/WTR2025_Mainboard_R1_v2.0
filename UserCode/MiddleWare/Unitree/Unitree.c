/**
 * @file Unitree.c
 * @author Lary (you@domain.com)
 * @brief  一代扣篮车底盘跳跃电机控制
 * @version 0.1
 * @date 2024-11-20
 * @note 对于轮腿电机，向上旋转是正速度
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Unitree.h"
UnitreeMotor unitree_DunkMotor_t[16];   //底盘跳跃电机对象创建

float unitree_DunkMotor_Tff[4];         //前馈力矩
float unitree_DunkMotor_Wdes[3];        //电机输出轴速度（-127.014 ~ 127.014  单位 rad/s）
float unitree_DunkMotor_posdes[3];        //期望电机输出轴输出位置（-65051.18 ~ 65051.18  单位 rad）
float unitree_DunkMotor_kp[3];          //电机刚度系数/位置误差比例系数（0 ~ 25.599）
float unitree_DunkMotor_kw[3];          //电机阻尼系数/速度误差比例（0 ~ 25.599）



/************************************************************
 * 线程定义
 */
osThreadId_t unitree_uart_message_TaskHandle;
const osThreadAttr_t unitree_uart_message_Task_attributes = {
    .name       = "unitree_uart_message_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void my_Unitree_UART_Message_Task(void *argument);




/***********************************************************接口函数************************************************************/
/**
 * @brief   Unitree电机初始化
 */
void my_Unitree_Init(void)
{
    for (uint8_t i = 0; i <6; i++)
    {
        if(Unitree_init(&unitree_DunkMotor_t[i], &UART_UNITREE_HANDLER, i ) == HAL_OK)
        {
            HAL_GPIO_TogglePin(LED6_GPIO_Port,LED6_Pin);
            osDelay(1);
        }
    }
    
    
}

/**
 * @brief   Unitree串口消息发送线程创建
 */
void my_Unitree_UART_Message_TaskStart(void)
{
    unitree_uart_message_TaskHandle = osThreadNew(my_Unitree_UART_Message_Task, NULL, &unitree_uart_message_Task_attributes);
}


/************************************************************线程函数**************************************************************************/
/**
 * @brief   底盘Unitree电机UART消息发送线程
 */
void my_Unitree_UART_Message_Task(void *argument)
{
    for (;;) {
        //Unitree_motor_0Torque();
        Unitree_UART_tranANDrev(&unitree_DunkMotor_t[0], 0, 1, unitree_DunkMotor_t[0].cmd.T,unitree_DunkMotor_t[0].cmd.W * UNITREE_REDUCTION_RATE, unitree_DunkMotor_t[0].cmd.Pos * UNITREE_REDUCTION_RATE, unitree_DunkMotor_t[0].cmd.K_P, unitree_DunkMotor_t[0].cmd.K_W);
        osDelay(2);
        Unitree_UART_tranANDrev(&unitree_DunkMotor_t[1], 1, 1, unitree_DunkMotor_t[1].cmd.T,unitree_DunkMotor_t[1].cmd.W * UNITREE_REDUCTION_RATE, unitree_DunkMotor_t[1].cmd.Pos * UNITREE_REDUCTION_RATE, unitree_DunkMotor_t[1].cmd.K_P, unitree_DunkMotor_t[1].cmd.K_W);
        osDelay(2);
        Unitree_UART_tranANDrev(&unitree_DunkMotor_t[2], 2, 1, unitree_DunkMotor_t[2].cmd.T,unitree_DunkMotor_t[2].cmd.W * UNITREE_REDUCTION_RATE, unitree_DunkMotor_t[2].cmd.Pos * UNITREE_REDUCTION_RATE, unitree_DunkMotor_t[2].cmd.K_P, unitree_DunkMotor_t[2].cmd.K_W);
        osDelay(2);
        Unitree_UART_tranANDrev(&unitree_DunkMotor_t[3], 3, 1, unitree_DunkMotor_t[3].cmd.T,unitree_DunkMotor_t[3].cmd.W * UNITREE_REDUCTION_RATE, unitree_DunkMotor_t[3].cmd.Pos , unitree_DunkMotor_t[3].cmd.K_P, unitree_DunkMotor_t[3].cmd.K_W);
        osDelay(2);
        Unitree_UART_tranANDrev(&unitree_DunkMotor_t[4], 4, 1, unitree_DunkMotor_t[4].cmd.T,unitree_DunkMotor_t[4].cmd.W * UNITREE_REDUCTION_RATE, unitree_DunkMotor_t[4].cmd.Pos , unitree_DunkMotor_t[4].cmd.K_P, unitree_DunkMotor_t[4].cmd.K_W);
    }
}


