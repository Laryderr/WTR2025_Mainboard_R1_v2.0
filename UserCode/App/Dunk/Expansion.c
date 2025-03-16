#include "Expansion.h"

Expansion_t Expansion_Up;
Expansion_t Expansion_Down;

void m_up_expand();
void m_up_contract();
void m_down_expand();
void m_down_contract();

/**
 * @brief 伸缩机构结构体初始化函数
 * 
 */
void Expansion_Init()
{
    for (uint8_t i = 4; i < 8; ++i) hDJI[i].motorType = M3508;
    Expansion_Down.m_DJI = &hDJI[4];
    Expansion_Down.m_Angle = 0;
    Expansion_Down.expand = m_down_expand;
    Expansion_Down.contract = m_down_contract;

    Expansion_Up.m_DJI = &hDJI[6];
    Expansion_Up.m_Angle = 0;
    Expansion_Up.expand = m_up_expand;
    Expansion_Up.contract = m_up_contract;
}

/**
 * @brief 伸缩机构执行器
 * 
 */
void Expansion_Executor()
{
    float DJI_Output_WithFC_Down[2];
    float DJI_Output_WithFC_Up[2];
    for (uint8_t i = 0; i < 2; ++i)
    {
        positionServo(Expansion_Down.m_Angle,   Expansion_Down.m_DJI + i);
        if((Expansion_Down.m_DJI + i)->speedPID.output > 100)
            DJI_Output_WithFC_Down[i] = (Expansion_Down.m_DJI + i)->speedPID.output + (Expansion_Down.m_DJI + i)->f_current;
        else if((Expansion_Down.m_DJI + i)->speedPID.output < -100)
            DJI_Output_WithFC_Down[i] = (Expansion_Down.m_DJI + i)->speedPID.output - (Expansion_Down.m_DJI + i)->f_current;
        else
            DJI_Output_WithFC_Down[i] = (Expansion_Down.m_DJI + i)->speedPID.output;
        
        positionServo(Expansion_Up.m_Angle,     Expansion_Up.m_DJI + i);        
        if((Expansion_Down.m_DJI + i)->speedPID.output > 100)
            DJI_Output_WithFC_Up[i] = (Expansion_Up.m_DJI + i)->speedPID.output + (Expansion_Up.m_DJI + i)->f_current;
        else if((Expansion_Down.m_DJI + i)->speedPID.output < -100)
            DJI_Output_WithFC_Up[i] = (Expansion_Up.m_DJI + i)->speedPID.output - (Expansion_Up.m_DJI + i)->f_current;
        else
            DJI_Output_WithFC_Up[i] = (Expansion_Up.m_DJI + i)->speedPID.output;
    }
    CanTransmit_DJI_5678(&hcan2, DJI_Output_WithFC_Down[0], DJI_Output_WithFC_Down[1], DJI_Output_WithFC_Up[0], DJI_Output_WithFC_Up[1]);
}

void m_up_expand()
{
    Expansion_Up.m_Angle -= 360 * UP_ROTATION;
}

void m_up_contract()
{
    Expansion_Up.m_Angle += 360 * UP_ROTATION;
}

void m_down_expand()
{
    Expansion_Down.m_Angle -= 360 * DOWN_ROTATION;
}

void m_down_contract()
{
    Expansion_Down.m_Angle += 360 * DOWN_ROTATION;
}
