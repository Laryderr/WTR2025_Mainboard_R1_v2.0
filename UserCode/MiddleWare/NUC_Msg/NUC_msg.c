#include "NUC_msg.h"

static uint8_t nuc_rev_buffer[PACKET_SIZE] = {0};
uint8_t nuc_rev_byte = 0;
static uint8_t rx_index = 0;

float Lidar_pose[3] = {0};
float camera_basket_xyz[3] = {0}; //篮筐的x(水平),y（竖直）,z（深度）坐标（相机坐标系）
bool packet_valid = false;

//激光雷达相关
static bool set_offset_flag = false;
static float raw_lidar[3];
static float lidar_offset[3] = {0};
static float lidar_offset_sum[3] = {0};
static uint8_t lidar_offset_count = 0;

//相机相关
#define CAMERA_FILTER_WINDOW 5
static float raw_camera[3]; // 中间变量：相机原始值
static float camera_buffer[3][CAMERA_FILTER_WINDOW] = {0};
static uint8_t camera_index = 0;
static bool camera_buffer_full = false;

//雷达相关
#define LIDAR_FILTER_WINDOW 10
static float lidar_buffer[3][LIDAR_FILTER_WINDOW];
static uint8_t lidar_index = 0;
static bool lidar_buffer_full = false;

/**
 * @brief NUC接收初始化
 * 
 */
void NUC_rev_init()
{
    HAL_UART_Receive_IT(&NUC_MSG_UART_HANDLE, &nuc_rev_byte, 1);
}

/**
 * @brief CRC接收计算
 * 
 * @param data 
 * @param length 
 * @return uint16_t 
 */
uint16_t ComputeCRC16(uint8_t* data, uint16_t length) {
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; ++i) {
        crc ^= (uint16_t)data[i] << 8;
        for (uint8_t j = 0; j < 8; ++j)
            crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : (crc << 1);
    }
    return crc;
}

/**
 * @brief NUC数据解码
 * 
 */
void NUC_Msg_Decode() {
    switch (rx_index) {
        case 0:
            if (nuc_rev_byte == 0xAA) {
                nuc_rev_buffer[rx_index++] = nuc_rev_byte;
            }
            break;
        case 1:
            if (nuc_rev_byte == 0x55) {
                nuc_rev_buffer[rx_index++] = nuc_rev_byte;
            } else {
                rx_index = 0;
            }
            break;
        default:
            nuc_rev_buffer[rx_index++] = nuc_rev_byte;
            if (rx_index >= PACKET_SIZE) {
                rx_index = 0;

                // CRC校验（完全未改动）
                uint16_t received_crc;
                memcpy(&received_crc, nuc_rev_buffer + 26, 2);
                uint16_t computed_crc = ComputeCRC16(nuc_rev_buffer, 26);

                if (received_crc == computed_crc) {
                    packet_valid = true;

                    // 雷达原始数据接收（完全未改动）
                    memcpy(&raw_lidar[0], nuc_rev_buffer + 2, 4);
                    memcpy(&raw_lidar[1], nuc_rev_buffer + 6, 4);
                    memcpy(&raw_lidar[2], nuc_rev_buffer + 10, 4);

                    // ========== 新增的雷达滤波处理 ==========
                    // 更新雷达数据缓存
                    for (int i = 0; i < 3; i++) {
                        lidar_buffer[i][lidar_index] = raw_lidar[i];
                    }
                    lidar_index++;
                    if (lidar_index >= LIDAR_FILTER_WINDOW) {
                        lidar_index = 0;
                        lidar_buffer_full = true;
                    }

                    // 校准逻辑（完全保持原有结构）
                    if (my_Alldir_Chassis_t.chassis_calibrate_flag == 1) {
                        if (!set_offset_flag) {
                            lidar_offset_sum[0] += raw_lidar[0];
                            lidar_offset_sum[1] += raw_lidar[1];
                            lidar_offset_sum[2] += raw_lidar[2];
                            lidar_offset_count++;

                            if (lidar_offset_count >= 10) {
                                lidar_offset[0] = lidar_offset_sum[0] / 10.0f;
                                lidar_offset[1] = lidar_offset_sum[1] / 10.0f;
                                lidar_offset[2] = lidar_offset_sum[2] / 10.0f;
                                set_offset_flag = true;
                            }

                            Lidar_pose[0] = raw_lidar[0];
                            Lidar_pose[1] = raw_lidar[1];
                            Lidar_pose[2] = raw_lidar[2];
                        } else {
                            // 仅在此处修改：应用偏置后使用滤波数据
                            if (lidar_buffer_full) {
                                for (int i = 0; i < 3; i++) {
                                    float sum = 0;
                                    for (int j = 0; j < LIDAR_FILTER_WINDOW; j++) {
                                        sum += lidar_buffer[i][j];
                                    }
                                    Lidar_pose[i] = (sum / LIDAR_FILTER_WINDOW) - lidar_offset[i];
                                }
                            } else {
                                // 缓存未满时保持原逻辑
                                for (int i = 0; i < 3; i++) {
                                    Lidar_pose[i] = raw_lidar[i] - lidar_offset[i];
                                }
                            }
                        }
                    }

                    // 相机数据处理（完全未改动）
                    memcpy(&raw_camera[0], nuc_rev_buffer + 14, 4);
                    memcpy(&raw_camera[1], nuc_rev_buffer + 18, 4);
                    memcpy(&raw_camera[2], nuc_rev_buffer + 22, 4);

                    for (int i = 0; i < 3; i++) {
                        camera_buffer[i][camera_index] = raw_camera[i];
                    }
                    camera_index++;
                    if (camera_index >= CAMERA_FILTER_WINDOW) {
                        camera_index = 0;
                        camera_buffer_full = true;
                    }

                    if (camera_buffer_full) {
                        for (int i = 0; i < 3; i++) {
                            float sum = 0;
                            for (int j = 0; j < CAMERA_FILTER_WINDOW; j++) {
                                sum += camera_buffer[i][j];
                            }
                            camera_basket_xyz[i] = sum / CAMERA_FILTER_WINDOW;
                        }
                    } else {
                        for (int i = 0; i < 3; i++) {
                            camera_basket_xyz[i] = raw_camera[i];
                        }
                    }
                } else {
                    packet_valid = false;
                }
            }
            break;
    }
    // UART接收（完全未改动）
    HAL_UART_Receive_IT(&NUC_MSG_UART_HANDLE, &nuc_rev_byte, 1);
}

/**
 * @brief 向NUC发送底盘当前位姿数据
 * @param pos 要发送的位姿结构体指针
 */
void SendCurrentPos_ToNUC()
{
    static uint8_t tx_buffer[20] = {0}; // 帧头 + 4个float + CRC = 20字节
    uint16_t crc;

    // 帧头
    tx_buffer[0] = 0x55;
    tx_buffer[1] = 0xAA;

    // 写入 float 数据（xpos, ypos, yawpos, yaw_offset）
    memcpy(&tx_buffer[2],  &(my_Alldir_Chassis_t.current_pos.xpos),       4);
    memcpy(&tx_buffer[6],  &(my_Alldir_Chassis_t.current_pos.ypos),       4);
    memcpy(&tx_buffer[10], &(my_Alldir_Chassis_t.current_pos.yawpos),     4);
    memcpy(&tx_buffer[14], &(my_Alldir_Chassis_t.current_pos.yaw_offset), 4);

    // 计算 CRC16，前16字节为数据
    crc = ComputeCRC16(tx_buffer, 18);
    memcpy(&tx_buffer[18], &crc, 2);  // 低字节在前，高字节在后

    // 发送
    HAL_UART_Transmit_DMA(&NUC_MSG_UART_HANDLE, tx_buffer, sizeof(tx_buffer));
}
