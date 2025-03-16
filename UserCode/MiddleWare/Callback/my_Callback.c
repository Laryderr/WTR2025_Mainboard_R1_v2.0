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
float temp = 0;
float temp1 =0;
float temp2 =0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == AS69_UART) {
        wtrMavlink_UARTRxCpltCallback(huart, MAVLINK_COMM_0);
        //temp=temp+0.001;
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