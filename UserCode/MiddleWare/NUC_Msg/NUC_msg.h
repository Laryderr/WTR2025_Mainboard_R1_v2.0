#ifndef NUC_MSG_H
#define NUC_MSG_H

#include "usermain.h"

#define NUC_MSG_UART_HANDLE huart2 // 串口选项
#define NUC_MSG_UART        USART2  // 串口选项
#define PACKET_SIZE 28 //包的字节数

//extern uint8_t nuc_rev_buffer[PACKET_SIZE];
extern float Lidar_pose[3];
extern float camera_basket_xyz[3];
extern void NUC_Msg_Decode();
extern void NUC_rev_init();

#endif // !1