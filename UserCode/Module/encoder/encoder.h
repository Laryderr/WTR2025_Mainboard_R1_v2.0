#ifndef ENCODER_H
#define ENCODER_H

#include "stm32f4xx_hal.h"
#include "can.h"
#include <stdio.h>

#define ENCODER_Calculate_Time 0.01 //角编码器角速度计算间隔时间,秒（Tim3定时器）
#define ENCODER_Calculate_htim htim3
#define ENCODER_Calculate_TIM  TIM3

// 定义接收数据结构
typedef struct {
    float angle;       // 角度
    float angularSpeed; // 角速度
    int16_t revolutions; // 转数
    float temperature; // 温度
    float last_angle;
    float encoder_W;
} EncoderData;

//定义计算数据结构体
typedef struct {
    int32_t angle;       // 角度
    int32_t angularSpeed; // 角速度
    
}EncoderCalculateData;

// 函数声明
HAL_StatusTypeDef ENCODER_CANFilterInit(CAN_HandleTypeDef* hcan);
//void ReceiveCANData(CAN_HandleTypeDef* hcan, EncoderData* data);
HAL_StatusTypeDef SendConfigCommand(CAN_HandleTypeDef* hcan, uint8_t* command, uint8_t length);
void Encoder_Init(CAN_HandleTypeDef* hcan);
//void CheckCANError();
void ParseCANData(CAN_RxHeaderTypeDef *RxHeader, uint8_t *RxData, EncoderData *data,EncoderCalculateData *caldata);
//void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);

extern CAN_HandleTypeDef hcan2;

extern EncoderData encoderData;
extern EncoderCalculateData encoderCalculateData;
#endif // ENCODER_H