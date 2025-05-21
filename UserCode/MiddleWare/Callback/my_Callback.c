/**
 * @file my_Callback.c
 * @author Lary (you@domain.com)
 * @brief 回调函数汇总
 * @version 0.1
 * @date 2024-11-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ashining_as69.h"
#include "HWT101CT_sdk.h"
#include "odom.h"
#include "NUC_msg.h"
float temp = 0;
float temp1 =0;
float temp2 =0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    static uint8_t ucTemp = 0;
    //遥控器回调
    if (huart->Instance == AS69_UART) {
        wtrMavlink_UARTRxCpltCallback(huart, MAVLINK_COMM_0);
        //temp=temp+0.001;
    }
    //陀螺仪回调
    if (huart->Instance == HUART_CURRENT.Instance) {
        WitSerialDataIn(ucTemp);
        HAL_UART_Receive_IT(huart, &ucTemp, 1);
    }
    //码盘回调
    /*if (huart->Instance == OPS_UART) {
        OPS_Decode();
    }*/

    if(huart->Instance == NUC_MSG_UART){
        NUC_Msg_Decode();
    }

    if(huart->Instance == LASER_DATA_UART){
        // 将接收到的字节写入缓冲区
        //Laser_Data_Decode();
        Msg_Write(&Laser_rev_byte, 1);
        HAL_UART_Receive_IT(&LASER_DATA_UART_HANDLE, &Laser_rev_byte, 1);
    }
   

}    

void wtrMavlink_MsgRxCpltCallback(mavlink_message_t *msg)
{
    //temp1 ++;
    mavlink_msg_joystick_air_decode(msg, &msg_joystick_air);

    switch (msg->msgid) {
        case 209:
            mavlink_msg_joystick_air_decode(msg, &msg_joystick_air);
            //temp1 ++;
            break;
        default:
            break;
    }
}

/*__weak void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    //码盘速度计算定时器回调
    if(htim->Instance == OPS_Calculate_TIM)
    {
        my_Chassis_Odom_v.current_pos[0] = chassis_x_point;
        my_Chassis_Odom_v.current_pos[1] = chassis_y_point;
        my_Chassis_Odom_v.current_v[0] = (my_Chassis_Odom_v.current_pos[0] - my_Chassis_Odom_v.last_pos[0])/OPS_Calculate_Time;
        my_Chassis_Odom_v.current_v[1] = (my_Chassis_Odom_v.current_pos[1] - my_Chassis_Odom_v.last_pos[1])/OPS_Calculate_Time;
        my_Chassis_Odom_v.last_pos[0]  = my_Chassis_Odom_v.current_v[0];
        my_Chassis_Odom_v.last_pos[1]  = my_Chassis_Odom_v.current_v[1];
    }
}*/