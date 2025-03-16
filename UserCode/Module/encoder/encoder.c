#include "encoder.h"
#include "can.h"

#define DEBUG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)

EncoderData encoderData;
EncoderCalculateData encoderCalculateData;

HAL_StatusTypeDef ENCODER_CANFilterInit(CAN_HandleTypeDef* hcan) {
    CAN_FilterTypeDef sFilterConfig;

    sFilterConfig.FilterBank = 14;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;

    // 设置过滤器 ID 为 0x50
    sFilterConfig.FilterIdHigh = (0x50 << 5); // 左移 5 位
    sFilterConfig.FilterIdLow = 0x0000;

    // 设置掩码为 0xFFFF，表示匹配所有位
    // sFilterConfig.FilterMaskIdHigh = 0xFFFF;
    // sFilterConfig.FilterMaskIdLow = 0xFFFF;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;

    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.SlaveStartFilterBank = 28;

    if (HAL_CAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_CAN_Start(hcan) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
        Error_Handler();
    }

    return HAL_OK;
}
// void CheckCANError() {
//     uint32_t errorCode = HAL_CAN_GetError(&hcan2);
//     // 检查是否有错误
//     if (errorCode != HAL_CAN_ERROR_NONE) {
//         // 打印错误状态
//         printf("CAN Error Code: %lu\n", errorCode);
//         // 检查特定的错误类型
//         if (errorCode & HAL_CAN_ERROR_EWG) {
//             printf("Error Warning Flag set\n");
//         }
//         if (errorCode & HAL_CAN_ERROR_EPV) {
//             printf("Error Passive Flag set\n");
//         }
//         if (errorCode & HAL_CAN_ERROR_BOF) {
//             printf("Bus Off Flag set\n");
//         }
//         if (errorCode & HAL_CAN_ERROR_ACK) {
//             printf("ACK Error Flag set\n");
//         }
//         if (errorCode & HAL_CAN_ERROR_STF) {
//             printf("Stuff Error Flag set\n");
//         }
//         if (errorCode & HAL_CAN_ERROR_FOR) {
//             printf("Form Error Flag set\n");
//         }
//         if (errorCode & HAL_CAN_ERROR_BR) {
//             printf("Bit Recessive Error Flag set\n");
//         }
//         // 去掉 HAL_CAN_ERROR_BS 和 HAL_CAN_ERROR_ID
//         if (errorCode & HAL_CAN_ERROR_CRC) {
//             printf("CRC Error Flag set\n");
//         }
//     } else {
//         printf("No CAN errors detected\n");
//     }
// }

// void ReceiveCANData(CAN_HandleTypeDef* hcan, EncoderData* data) {
//     CAN_RxHeaderTypeDef RxHeader;
//     uint8_t RxData[8];
//     CheckCANError();
//     // 检查接收FIFO是否有消息
//     if (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) > 0) {
//         // 接收消息
//         if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
//             printf("Received message: ");
//             for (int i = 0; i < RxHeader.DLC; i++) {
//                 printf("%02X ", RxData[i]);
//             }
//             printf("\n");
//             // 解析数据
//             if (RxData[0] == 0x55 && RxData[1] == 0x55) {
//                 uint16_t angleReg = (RxData[2] << 8) | RxData[3];
//                 data->angle = angleReg * 360.0f / 32768.0f;
//                 uint16_t angularSpeedReg = (RxData[4] << 8) | RxData[5];
//                 data->angularSpeed = angularSpeedReg * 360.0f / 32768.0f / 0.1f;
//                 data->revolutions = (RxData[6] << 8) | RxData[7];
//             } else if (RxData[0] == 0x55 && RxData[1] == 0x56) {
//                 uint16_t tempReg = (RxData[2] << 8) | RxData[3];
//                 data->temperature = tempReg / 100.0f;
//             }
//         } else {
//             printf("Failed to get message from FIFO\n");
//         }
//     } else {
//         printf("No message in FIFO\n");
//     }
//     if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
//           // 处理接收到的数据
//           printf("Received message ID: %X, Data: ", RxHeader.StdId);
//           for (int i = 0; i < RxHeader.DLC; i++) {
//               printf("%02X ", RxData[i]);
//           }
//           printf("\n");
//       } else {
//           printf("Failed to get message from FIFO\n");
//       }
// }

void ParseCANData(CAN_RxHeaderTypeDef *RxHeader, uint8_t *RxData, EncoderData *data,EncoderCalculateData *caldata) {
    // 解析数据
    if (RxData[0] == 0x55 && RxData[1] == 0x55) {
        // 角度计算
        uint16_t angleReg = (RxData[3] << 8) | RxData[2]; // 角度寄存器数值
        data->angle = angleReg * 360.0f / 32768.0f; // 角度计算
        int32_t angleScaled = (int32_t)(data->angle * 100); // 乘以100并存储为int32_t
        caldata->angle = angleScaled;
        // printf("Debug: Angle Register Value: %u, Calculated Angle: %ld degrees\n", angleReg, (long)angleScaled);
        // printf("Debug:caldata->angle: %ld degrees\n", (long)caldata->angle);
        // 角速度计算
        uint16_t angularSpeedReg = (RxData[5] << 8) | RxData[4]; // 角速度寄存器数值
        data->angularSpeed = angularSpeedReg * 360.0f / 32768.0f / 0.1f; // 角速度计算
        int32_t angularSpeedScaled = (int32_t)(data->angularSpeed * 100); // 乘以100并存储为int32_t
        caldata->angularSpeed = angularSpeedScaled;
        // printf("Debug: Angular Speed Register Value: %u, Calculated Angular Speed: %ld degrees/s\n", angularSpeedReg, (long)angularSpeedScaled);

        // 转数计算
        data->revolutions = (int16_t)((RxData[7] << 8) | RxData[6]); // 转数计算，使用int16_t
        // printf("Debug: Revolutions Register Value: %d\n", data->revolutions);
        // printf("Parsed Data (Type 0x55 0x55): Angle: %ld (x100), Angular Speed: %ld (x100), Revolutions: %d\n",
            // (long)angleScaled, (long)angularSpeedScaled, data->revolutions);
    } else if (RxData[0] == 0x55 && RxData[1] == 0x56) {
        // 温度计算
        uint16_t tempReg = (RxData[3] << 8) | RxData[2]; // 注意这里组合的是bb在高位，aa在低位
        data->temperature = tempReg / 100.0f; // 确保先组合后再除
        // printf("Debug: Temperature Register Value: %u, Calculated Temperature: %d °C\n", tempReg, (int)data->temperature);
        // printf("Parsed Data (Type 0x55 0x56): Temperature: %d°C\n", (int)data->temperature);
    } else {
        // printf("Unknown data type received.\n");
    }
}

// void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
//     CAN_RxHeaderTypeDef RxHeader;
//     uint8_t RxData[8];
//     EncoderData data;
//     CheckCANError();
//     检查接收FIFO是否有消息
//     if (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) > 0) {
//         // 接收消息
//         if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
//             printf("Received message ID: %X, Data: ", RxHeader.StdId);
//             for (int i = 0; i < RxHeader.DLC; i++) {
//                 printf("%02X ", RxData[i]);
//             }
//             printf("\n");
//             // 解析数据
//             ParseCANData(&RxHeader, RxData, &data);
//         } else {
//             printf("Failed to get message from FIFO\n");
//         }
//     } else {
//         printf("No message in FIFO\n");
//     }
// }

// 发送配置命令的函数
HAL_StatusTypeDef SendConfigCommand(CAN_HandleTypeDef* hcan, uint8_t* command, uint8_t length) {
    CAN_TxHeaderTypeDef TxHeader;
    TxHeader.StdId = 0x50; // 根据你的设备ID设置
    TxHeader.ExtId = 0x00;
    TxHeader.RTR = CAN_RTR_DATA;
    TxHeader.IDE = CAN_ID_STD;
    TxHeader.DLC = length;
    uint32_t TxMailbox;
    return HAL_CAN_AddTxMessage(hcan, &TxHeader, command, &TxMailbox); // 返回状态
}

//该角编码器是掉电保存配置，只需要配置一次即可
//白线置零也会置零波特率
void Encoder_Init(CAN_HandleTypeDef* hcan) {
    HAL_StatusTypeDef status;

    // Step 1: 解锁
    uint8_t unlockCommand[] = {0xFF, 0xAA, 0x69, 0x88, 0xB5};
    status = SendConfigCommand(hcan, unlockCommand, sizeof(unlockCommand));
    DEBUG_PRINT("Unlock Command Status: %d\n", status);
    if (status != HAL_OK) {
        DEBUG_PRINT("Unlock command failed!\n");
        return;
    }
    HAL_Delay(10); // 等待解锁指令生效
    // Step 2: 设置波特率
    uint8_t setBaudRateCommand[] = {0xFF, 0xAA, 0x04, 0x00, 0x00}; // 0x00 表示 1000K
    status = SendConfigCommand(hcan, setBaudRateCommand, sizeof(setBaudRateCommand));
    DEBUG_PRINT("Set Baud Rate Command Status: %d\n", status);
    if (status != HAL_OK) {
        DEBUG_PRINT("Set Baud Rate command failed!\n");
        return;
    }
    HAL_Delay(10); // 等待波特率设置生效

    // Step 3: 设置回传内容
    uint8_t setRswCommand[] = {0xFF, 0xAA, 0x02, 0x03, 0x00}; // 角度、角速度、转数和温度
    status = SendConfigCommand(hcan, setRswCommand, sizeof(setRswCommand));
    DEBUG_PRINT("Set RSW Command Status: %d\n", status);
    if (status != HAL_OK) {
        DEBUG_PRINT("RSW command failed!\n");
        return;
    }
    HAL_Delay(10);

    // Step 4: 设置编码器模式为多圈模式
    uint8_t setModeCommand[] = {0xFF, 0xAA, 0x10, 0x01, 0x00}; // 多圈模式
    status = SendConfigCommand(hcan, setModeCommand, sizeof(setModeCommand));
    DEBUG_PRINT("Set Mode Command Status: %d\n", status);
    if (status != HAL_OK) {
        DEBUG_PRINT("Mode command failed!\n");
        return;
    }
    HAL_Delay(10);

    // Step 5: 设置旋转方向为逆时针
    uint8_t setSpinDirCommand[] = {0xFF, 0xAA, 0x15, 0x01, 0x00}; // 逆时针
    status = SendConfigCommand(hcan, setSpinDirCommand, sizeof(setSpinDirCommand));
    DEBUG_PRINT("Set Spin Dir Command Status: %d\n", status);
    if (status != HAL_OK) {
        DEBUG_PRINT("Spin dir command failed!\n");
        return;
    }
    HAL_Delay(10);

    // Step 6: 保存配置
    uint8_t saveCommand[] = {0xFF, 0xAA, 0x00, 0x00, 0x00}; // 保存当前配置
    status = SendConfigCommand(hcan, saveCommand, sizeof(saveCommand));
    DEBUG_PRINT("Save Command Status: %d\n", status);
    if (status != HAL_OK) {
        DEBUG_PRINT("Save command failed!\n");
        return;
    }
    HAL_Delay(10);

    DEBUG_PRINT("Encoder Initialization Complete\n");
}