/**
 * @file HWT101CT_sdk.h
 * @author Lyn Liam (Quan.2003@outlook.com)
 * @brief  WitHWT101CT陀螺仪
 * @version 0.1
 * @date 2024-01-13
 *
 * 使用说明：
 * 要求：
 *  波特率：115200
 *  实现函数：delay_ms()
 *  更改串口宏定义： HUART_CURRENT   (第37行)
 *
 * 使用：
 *  初始化函数：      HW101_Init();
 *  循环调用：       ProcessData();
 *  数据将会出现在 本文件的 gyrodata[3] 数组，
 *      gyrodata[0]、gyrodata[1]为角加速度 y,z 轴
 *      gyrodata[2]             为角度    z 轴
 * @copyright Copyright (c) 2024
 *
 */

#ifndef HWT101CT_SDK_H
#define HWT101CT_SDK_H

#ifdef __cplusplus
extern "C" {
#endif
/**--------------------------C-----------------------------**/
#include "usart.h"
#include "tim.h"
#include "wit_c_sdk.h"
#include <stdio.h>

#define HUART_CURRENT huart7

#define ACC_UPDATE    0x01
#define GYRO_UPDATE   0x02
#define ANGLE_UPDATE  0x04
#define MAG_UPDATE    0x08
#define READ_UPDATE   0x80

extern float fAcc[3], fGyro[3], fAngle[3];
extern const uint32_t c_uiBaud[10];

extern float gyrodata[3];
typedef union {
    float data;
    uint8_t byte[4];
} float_to_byte;

extern float_to_byte data_imu[13];

void trans_float(UART_HandleTypeDef huart);
void CopeCmdData(unsigned char ucData);
void SensorUartSend(uint8_t *p_data, uint32_t uiSize);
void SensorDataUpdata(uint32_t uiReg, uint32_t uiRegNum);
void Delayms(uint16_t ucMs);
void ProcessData(void);

void UsartInit(UART_HandleTypeDef huart, USART_TypeDef *USART, unsigned int uiBaud);
uint8_t HW101_Init(void);

#ifdef __cplusplus
};
/**-----------------------------C++---------------------------**/
#endif

#endif // !HWT101CT_SDK_H