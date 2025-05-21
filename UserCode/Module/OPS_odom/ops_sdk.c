#include "ops_sdk.h"
#include "userglobal_config.h"
#include <stdio.h>

typedef union {
    uint8_t data[24]; //接收数据缓冲
    float ActVal[6];  //储存解码后的六个姿态信息
} Posture;

OPS_t OPS_Data = {0};
Posture posture;
static uint8_t count = 0;
uint8_t i            = 0;
uint8_t ch[1];
char ops_calibration_msg[5] = "ACTR"; //校准消息

/****************************************************************/
/**
 * @brief   字符串拼接函数
 *          将strSource的num个字符拼接到strDestination的末尾
 */
static void Stract(char strDestination[], char strSource[], int num)
{
    int i = 0, j = 0;
    while (strDestination[i] != '\0') {
        i++;
    }
    for (j = 0; j < num; j++) {
        strDestination[i++] = strSource[j];
    }
}

/**
 * @brief   x坐标更新函数
 * @param   x   新的坐标值
 */
void OPS_Update_X(float x)
{
    int i            = 0;
    char update_x[8] = "ACTX";
    static union {
        float x;
        char data[4];
    } new_set;
    new_set.x = x;
    Stract(update_x, new_set.data, 4);
    for (i = 0; i < 8; i++) {
        HAL_UART_Transmit(&OPS_UART_HANDLE, update_x, 8, 0xFF);
    }
}

/**
 * @brief   y坐标更新函数
 * @param   y   新的坐标值
 */
void OPS_Update_Y(float y)
{
    int i            = 0;
    char update_y[8] = "ACTY";
    static union {
        float y;
        char data[4];
    } new_set;
    new_set.y = y;
    Stract(update_y, new_set.data, 4);
    for (i = 0; i < 8; i++) {
        HAL_UART_Transmit(&OPS_UART_HANDLE, update_y, 8, 0xFF);
    }
}

/**
 * @brief   航向角更新函数
 * @param   angle   新的航向角值
 */
void OPS_Update_A(float angle)
{
    int i            = 0;
    char update_a[8] = "ACTJ";
    static union {
        float angle;
        char data[4];
    } new_set;
    new_set.angle = angle;
    Stract(update_a, new_set.data, 4);
    for (i = 0; i < 8; i++) {
        HAL_UART_Transmit(&OPS_UART_HANDLE, update_a, 8, 0xFF);
    }
}

/**
 * @brief   码盘解码函数
 */
uint8_t OPS_Decode(void)
{
    HAL_UART_Receive_IT(&OPS_UART_HANDLE, (uint8_t *)&ch, 1);
#if (OPS_Calibration == 0)
    switch (count) {
        case 0:
            if (ch[0] == 0x0d)
                count++;
            else
                count = 0;
            break;
        case 1:
            if (ch[0] == 0x0a) {
                i = 0;
                count++;
            } else if (ch[0] == 0x0d)
                ;
            else
                count = 0;
            break;
        case 2:
            posture.data[i] = ch[0];
            i++;
            if (i >= 24) {
                i = 0;
                count++;
            }
            break;
        case 3:
            if (ch[0] == 0x0a)
                count++;
            else
                count = 0;
            break;
        case 4:
            if (ch[0] == 0x0d) {
                OPS_Data.z_angle = posture.ActVal[0];
                OPS_Data.x_angle = posture.ActVal[1];
                OPS_Data.y_angle = posture.ActVal[2];
                OPS_Data.pos_x   = posture.ActVal[3];
                OPS_Data.pos_y   = posture.ActVal[4];
                OPS_Data.w_z     = posture.ActVal[5];
            }
            count = 0;
            break;
        default:
            count = 0;
            break;
    }
#endif
#if (OPS_Calibration == 1)
    ;
#endif
    return 0;
}

/**
 * @brief   码盘初始化函数
 */
void OPS_Init(void)
{
    HAL_UART_Receive_IT(&OPS_UART_HANDLE, (uint8_t *)&ch, 1);
}

/**
 * @brief   码盘校准函数
 */
void OPS_calibration(void)
{
    HAL_UART_Transmit(&OPS_UART_HANDLE, (uint8_t *)ops_calibration_msg, 5, 0xFF);
}

/**
 * @brief   串口回调函数
 */
__weak void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == OPS_UART) {
        OPS_Decode();
    }
}
