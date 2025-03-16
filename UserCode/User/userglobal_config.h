/**
 * @file userglobal_config.h
 * @author Lary (you@domain.com)
 * @brief 存放整车通用参数配置
 * @version 0.1
 * @date 2024-11-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __USERGLOBAL_CONFIG_H
#define __USERGLOBAL_CONFIG_H
#ifdef __cplusplus
extern "C"{
#endif 

#include "usermain.h"

#define USE_FREERTOS 1

#define AS69_UART_HANDLE    huart8  //接收器串口
#define AS69_UART           UART8

#define ID_HANDLE_DUNK      2
#define ID_HANDLE_INTERCEPT 2
#define ID_HANDLE_PATBALL   2
#define ID_HANDLE_SHOOT     2
#define ID_RUN              3
#define ID_STATUS           4
#define ID_MODE             5
#define ID_RST              6



#define chassis_R           (152 * 0.001) // 全向轮半径
#define chassis_r           (293.26 * 0.001) // 底盘半径
#define PI                  3.1415926


/***********************************************************
 * @brief 仪表板消息定义
 * 
 */

extern mavlink_joystick_air_dashboard_set_title_t mav_run_title;
extern mavlink_joystick_air_dashboard_set_msg_t mav_run_msg;
extern char run_title[20]     ;
extern char run_auto_msg[20]   ;
extern char run_handle_msg[20];

extern mavlink_joystick_air_dashboard_set_title_t mav_rst_choose_title;
extern mavlink_joystick_air_dashboard_set_msg_t mav_rst_choose_msg;
extern char rst_choose_title[20]   ;
extern char rst_choose_msg[20]  ;
extern char norst_choose_msg[20]  ;

extern mavlink_joystick_air_dashboard_set_title_t mav_dir_choose_title;
extern mavlink_joystick_air_dashboard_set_msg_t mav_dir_choose_msg;
extern char direct_choose_title[20]  ;
extern char direct_choose_msg[20]    ;

extern mavlink_joystick_air_dashboard_set_title_t mav_mode_title;
extern mavlink_joystick_air_dashboard_set_msg_t mav_mode_msg;
extern char mode_title[20]     ;
extern char mode_idle_msg[20]  ;
extern char mode_shoot_msg[20] ;
extern char mode_intercept_msg[20]  ;
extern char mode_patball_msg[20]  ;
extern char mode_dunk_msg[20] ;

extern mavlink_joystick_air_dashboard_set_title_t mav_status_title;
extern mavlink_joystick_air_dashboard_set_msg_t mav_status_msg;
extern char status_title[20];
extern char status_running_msg[20];
extern char status_ready_msg[20];
extern char status_stop_msg[20];
extern char status_error_msg[20];












#ifdef __cplusplus
}
#endif

#endif