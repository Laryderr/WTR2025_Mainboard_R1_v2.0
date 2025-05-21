//
// Created by Liam 2023/09/24
//

#include "HWT101CT_sdk.h"
#include "stdio.h"

float fAcc[3] = {0.0}, fGyro[3] = {0.0}, fAngle[3] = {0.0};
const uint32_t c_uiBaud[10] = {0, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};
volatile char s_cDataUpdate = 0, s_cCmd = 0xff;
const USART_TypeDef *USART_CURRENT = USART1;

float gyrodata[3]  = {0};
uint8_t begin_code = 0x55;
uint8_t end_code   = 0xff;
uint8_t s          = 0;

/**
 * @brief HW101初始化函数
 *
 * @return uint8_t 如果返回0则Init正常执行
 */
uint8_t HW101_Init(void)
{
    __HAL_UART_ENABLE_IT(&HUART_CURRENT, UART_IT_RXNE);
    HAL_UART_Receive_IT(&HUART_CURRENT, &s, 1);

    WitInit(WIT_PROTOCOL_NORMAL, 0x50);
    WitSerialWriteRegister(SensorUartSend);
    WitRegisterCallBack(SensorDataUpdata);
    WitDelayMsRegister(Delayms);

    return HAL_OK;
}

/**
 * @brief 六轴计算函数
 *
 */
void ProcessData(void)
{
    if (s_cDataUpdate) {
        for (int i = 0; i < 3; i++) {
            fAcc[i]   = sReg[AX + i] / 32768.0f * 16.0f;
            fGyro[i]  = sReg[GX + i] / 32768.0f * 2000.0f;
            fAngle[i] = sReg[Roll + i] / 32768.0f * 180.0f;
        }
        if (s_cDataUpdate & ACC_UPDATE) {
            // printf("acc:%.3f %.3f %.3f\r\n", fAcc[0], fAcc[1], fAcc[2]);
            s_cDataUpdate &= ~ACC_UPDATE;
        }
        if (s_cDataUpdate & GYRO_UPDATE) {
            gyrodata[0] = fGyro[1];
            gyrodata[1] = fGyro[2];
            // printf("gyro:%.3f %.3f %.3f\r\n", fGyro[0], fGyro[1], fGyro[2]);
            s_cDataUpdate &= ~GYRO_UPDATE;
        }
        if (s_cDataUpdate & ANGLE_UPDATE) {
            gyrodata[2] = fAngle[2];
            // printf("angle:%.3f %.3f %.3f\r\n", fAngle[0], fAngle[1], fAngle[2]);
            s_cDataUpdate &= ~ANGLE_UPDATE;
        }
        if (s_cDataUpdate & MAG_UPDATE) {
            // printf("mag:%d %d %d\r\n", sReg[HX], sReg[HY], sReg[HZ]);
            s_cDataUpdate &= ~MAG_UPDATE;
        }
    }
}

/**
 * @brief 解码发送函数
 *
 */
void trans_float(UART_HandleTypeDef huart)
{
    HAL_UART_Transmit(&huart, &begin_code, 1, 100);
    for (int i = 0; i < 3; i++) {
        data_imu[i].data = gyrodata[i];
        for (int j = 0; j < 4; j++) {
            HAL_UART_Transmit(&huart, &data_imu[i].byte[j], 1, 100);
            ;
        }
    }
    HAL_UART_Transmit(&huart, &end_code, 1, 100);
}

/**
 * @brief
 *
 * @param ucData
 */
void CopeCmdData(unsigned char ucData)
{
    static unsigned char s_ucData[50], s_ucRxCnt = 0;

    s_ucData[s_ucRxCnt++] = ucData;
    if (s_ucRxCnt < 3) return; // Less than three data returned
    if (s_ucRxCnt >= 50) s_ucRxCnt = 0;
    if (s_ucRxCnt >= 3) {
        if ((s_ucData[1] == '\r') && (s_ucData[2] == '\n')) {
            s_cCmd = s_ucData[0];
            memset(s_ucData, 0, 50);
            s_ucRxCnt = 0;
        } else {
            s_ucData[0] = s_ucData[1];
            s_ucData[1] = s_ucData[2];
            s_ucRxCnt   = 2;
        }
    }
}

/**
 * @brief 毫秒延时函数绑定函数，为SDK对外接口
 *
 * @param ucMs 延时时间（ms）
 */
void Delayms(uint16_t ucMs)
{
    for (int i = 0; i < ucMs; i++) {
        uint32_t cnt = 0;
        __HAL_TIM_SET_COUNTER(&htim6, 0);
        __HAL_TIM_SET_AUTORELOAD(&htim6, 1000);
        HAL_TIM_Base_Start(&htim6);
        while (cnt != 1000) {
            cnt = __HAL_TIM_GET_COUNTER(&htim6);
        }
        HAL_TIM_Base_Stop(&htim6);
    }
}

/**
 * @brief 串口函数绑定函数，为该SDK对外接口
 *
 * @param p_data 要发送的数据
 * @param uiSize 数据大小
 */
void SensorUartSend(uint8_t *p_data, uint32_t uiSize)
{
    HAL_UART_Transmit(&HUART_CURRENT, p_data, uiSize, 0xffff);
}

/**
 * @brief 数据更新函数
 *
 * @param uiReg
 * @param uiRegNum
 */
void SensorDataUpdata(uint32_t uiReg, uint32_t uiRegNum)
{
    int i;
    for (i = 0; i < uiRegNum; i++) {
        switch (uiReg) {
                //            case AX:
                //            case AY:
            case AZ:
                s_cDataUpdate |= ACC_UPDATE;
                break;
                //            case GX:
                //            case GY:
            case GZ:
                s_cDataUpdate |= GYRO_UPDATE;
                break;
                //            case HX:
                //            case HY:
            case HZ:
                s_cDataUpdate |= MAG_UPDATE;
                break;
                //            case Roll:
                //            case Pitch:
            case Yaw:
                s_cDataUpdate |= ANGLE_UPDATE;
                break;
            default:
                s_cDataUpdate |= READ_UPDATE;
                break;
        }
        uiReg++;
    }
}

/**
 * @brief 这个函数用于更改串口波特率
 *
 * @param USART 串口结构体指针
 * @param uiBaud 设置的波特率
 */
void UsartInit(UART_HandleTypeDef huart, USART_TypeDef *USART, unsigned int uiBaud)
{
    huart.Instance          = USART;
    huart.Init.BaudRate     = uiBaud;
    huart.Init.WordLength   = UART_WORDLENGTH_8B;
    huart.Init.StopBits     = UART_STOPBITS_1;
    huart.Init.Parity       = UART_PARITY_NONE;
    huart.Init.Mode         = UART_MODE_TX_RX;
    huart.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart) != HAL_OK) {
        Error_Handler();
    }
}

__weak void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    static uint8_t ucTemp = 0;
    if (huart->Instance == HUART_CURRENT.Instance) {
        WitSerialDataIn(ucTemp);
        HAL_UART_Receive_IT(huart, &ucTemp, 1);
    }
}