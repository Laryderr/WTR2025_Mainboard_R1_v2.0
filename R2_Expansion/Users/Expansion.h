/*
 * @Description: 
 * @Author: Alex
 * @Date: 2024-11-25 20:03:00
 * @LastEditors: Alex
 * @LastEditTime: 2025-03-10 21:41:33
 */
#ifndef EXPANSION_H
#define EXPANSION_H

#include "DJI.h"
#include "can.h"
#include "wtr_can.h"
#include "Caculate.h"

#define UP_ROTATION 1      //上层机构伸缩所需旋转圈数   10
#define DOWN_ROTATION 10    //下层机构伸缩所需旋转圈数   14
#define MX_CAN &hcan1

typedef void (*__expand)(void);
typedef void (*__contract)(void);

typedef struct
{
    DJI_t* m_DJI;               //伸缩机构使用两个电机数组首地址
    float m_Angle;              //伸缩机构电机目前角度

    __expand expand;            //伸缩机构伸长函数
    __contract contract;        //伸缩机构收缩函数
}Expansion_t;

extern Expansion_t Expansion_Up;
extern Expansion_t Expansion_Down;

void Expansion_Init();
void Expansion_Executor();

#endif