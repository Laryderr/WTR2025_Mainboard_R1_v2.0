#include "NUC_msg.h"

static uint8_t nuc_rev_buffer[PACKET_SIZE] = {0};
uint8_t nuc_rev_byte = 0;
static uint8_t rx_index = 0;

float Lidar_pose[3] = {0};
float camera_basket_xyz[3] = {0}; //篮筐的x(水平),y（竖直）,z（深度）坐标（相机坐标系）
bool packet_valid = false;

static bool set_offset_flag = false;
static float lidar_offset[3] = {0};
static float lidar_offset_sum[3] = {0};
static uint8_t lidar_offset_count = 0;

void NUC_rev_init()
{
    HAL_UART_Receive_IT(&NUC_MSG_UART_HANDLE, &nuc_rev_byte, 1);
}

uint16_t ComputeCRC16(uint8_t* data, uint16_t length) {
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; ++i) {
        crc ^= (uint16_t)data[i] << 8;
        for (uint8_t j = 0; j < 8; ++j)
            crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : (crc << 1);
    }
    return crc;
}

void NUC_Msg_Decode()
{
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

                uint16_t received_crc;
                memcpy(&received_crc, nuc_rev_buffer + 26, 2);
                uint16_t computed_crc = ComputeCRC16(nuc_rev_buffer, 26);

                if (received_crc == computed_crc) {
                    packet_valid = true;

                    float raw_lidar[3];
                    memcpy(&raw_lidar[0], nuc_rev_buffer + 2, 4);
                    memcpy(&raw_lidar[1], nuc_rev_buffer + 6, 4);
                    memcpy(&raw_lidar[2], nuc_rev_buffer + 10, 4);

                    if(my_Alldir_Chassis_t.chassis_calibrate_flag==1){
                        // 初始化偏置处理 
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

                            // 初始化阶段直接复制原始值
                            Lidar_pose[0] = raw_lidar[0];
                            Lidar_pose[1] = raw_lidar[1];
                            Lidar_pose[2] = raw_lidar[2];
                        } else {
                            // 应用偏置修正
                            Lidar_pose[0] = raw_lidar[0] - lidar_offset[0];
                            Lidar_pose[1] = raw_lidar[1] - lidar_offset[1];
                            Lidar_pose[2] = raw_lidar[2] - lidar_offset[2];
                        }
                    }
                    
                    memcpy(&camera_basket_xyz[0], nuc_rev_buffer + 14, 4);
                    memcpy(&camera_basket_xyz[1], nuc_rev_buffer + 18, 4);
                    memcpy(&camera_basket_xyz[2], nuc_rev_buffer + 22, 4);

                } else {
                    packet_valid = false;
                }
            }
            break;
    }
    HAL_UART_Receive_IT(&NUC_MSG_UART_HANDLE, &nuc_rev_byte, 1);
}
