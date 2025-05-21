/**
 * @brief   OPS定位系统解码
 * @author  SSC
 * @note    注意，OPS需要使用RS232转TTL模块且在TTL端使用TX--TX RX--RX接线方法
 *          使能对应串口的DMA RX通道，不必开全局中断
 *          调用OPS_Init()函数进行初始化，然后再DMA空闲中断中进行解码
 *          开机后保持15s静止时间；
 *          码盘陀螺仪会产生零点漂移，建议在上坡后使用激光辅助清零。
 *          代码提供了OPS_Data作为数据接口
 */
#ifndef __WTR_OPS_H
#define __WTR_OPS_H

#include "stm32f4xx.h"
#include "usart.h"
/************************ 用户定义 ***************************/
#define OPS_UART_HANDLE huart2 // 串口选项
#define OPS_UART        USART2  // 串口选项

/************************ 结构体定义 *************************/

typedef struct OPS_t {
    float pos_x;   // x坐标
    float pos_y;   // y坐标
    float z_angle; // 俯仰角
    float x_angle; // 横滚角
    float y_angle; // 偏航角
    float w_z;     // 角速度
} OPS_t;           // OPS数据结构体

extern OPS_t OPS_Data;

extern uint8_t ch[1];

/************************ 用户函数 *************************/
void OPS_Init(void);
uint8_t OPS_Decode(void);
void OPS_Update_X(float x);
void OPS_Update_Y(float y);
void OPS_Update_A(float angle);
void OPS_calibration(void);

#endif