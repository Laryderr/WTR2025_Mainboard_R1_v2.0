/*
 * @Description: 
 * @Author: Alex
 * @Date: 2025-03-16 10:28:26
 * @LastEditors: Alex
 * @LastEditTime: 2025-03-16 10:28:38
 */
#ifndef ENCODER_H
#define ENCODER_H

#include "stm32f4xx_hal.h"
#include "can.h"
#include "encoder.h"
#include <stdio.h>

// 定义接收数据结构
typedef struct {
    float angle;       // 角度
    float angularSpeed; // 角速度
    int16_t revolutions; // 转数
    float temperature; // 温度
} EncoderData;

//定义计算数据结构体
typedef struct {
    int32_t angle;       // 角度
    int32_t angularSpeed; // 角速度
}EncoderCalculateData;

extern EncoderData data;
extern EncoderCalculateData caldata;

// 函数声明
HAL_StatusTypeDef ENCODER_CANFilterInit(CAN_HandleTypeDef* hcan);
//void ReceiveCANData(CAN_HandleTypeDef* hcan, EncoderData* data);
HAL_StatusTypeDef SendConfigCommand(CAN_HandleTypeDef* hcan, uint8_t* command, uint8_t length);
void Encoder_Init(CAN_HandleTypeDef* hcan);
//void CheckCANError();
void ParseCANData(CAN_RxHeaderTypeDef *RxHeader, uint8_t *RxData, EncoderData *data,EncoderCalculateData *caldata);
//void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);

extern CAN_HandleTypeDef hcan2;
#endif // ENCODER_H