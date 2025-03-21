//
// Created by liam on 2023/10/4.
//

#ifndef UNITREE_M8010_UNITREE_USER_H
#define UNITREE_M8010_UNITREE_USER_H
#ifdef __cplusplus
extern "C" {
#endif
#include "motor_control.h"
#include "usart.h"


typedef struct
{
    unsigned short id;                  //���ID��0����ȫ�����// 电机ID，标识电机（0表示广播）
    unsigned short mode;
    float T;                            //�����ؽڵ�������أ������������أ���Nm�� // 扭矩，单位：Nm
    float W;                            //�����ؽ��ٶȣ����������ٶȣ�(rad/s)// 速度，单位：rad/s
    float Pos;                          //�����ؽ�λ�ã�rad��// 位置，单位：rad
    float K_P;                          //�ؽڸն�ϵ�� // 刚度系数/位置控制比例
    float K_W;                          //�ؽ��ٶ�ϵ�� // 阻尼系数/速度控制比例
}M_UNITREE_DATA;

typedef struct{
    UART_HandleTypeDef *huart;
    MOTOR_send cmd;
    MOTOR_recv data;
    M_UNITREE_DATA m_data;
}UnitreeMotor;

//extern UnitreeMotor Unitree_Motor[1];
UnitreeMotor *Unitree_Create_Motor();
HAL_StatusTypeDef Unitree_init(UnitreeMotor *MotorInstance,UART_HandleTypeDef *usartx,int id);

HAL_StatusTypeDef Unitree_UART_tranANDrev(UnitreeMotor *MotorInstance, unsigned char motor_id,
                                          unsigned char mode, float T,float W,
                                          float Pos, float K_P, float K_W);

HAL_StatusTypeDef Unitree_Encoder_Autoclibrating(UnitreeMotor *MotorInstance);

void Unitree_ChangeState(UnitreeMotor *MotorInstance, unsigned char motor_id,
                                          unsigned char mode, float T,float W,
                                          float Pos, float K_P, float K_W);

#endif //UNITREE_M8010_UNITREE_USER_H


#ifndef UNITREE_M8010_UNITREE_USER_H_
#define  UNITREE_M8010_UNITREE_USER_H_

#define UNITREE_REDUCTION_RATE 6.33



#define RS485_DE_GPIO_Port           GPIOA
#define RS485_DE_Pin                 GPIO_PIN_2

#define RS485_RE_GPIO_Port           GPIOA
#define RS485_RE_Pin                 GPIO_PIN_1

#define SET_485_DE_UP()              HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET)
#define SET_485_DE_DOWN()            HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET)

#define SET_485_RE_UP()              HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_SET)
#define SET_485_RE_DOWN()            HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_RESET)

#define UART_UNITREE_HANDLER         huart6
#ifdef __cplusplus
}
#endif
#endif //UNITREE_M8010_UNITREE_USER_H_