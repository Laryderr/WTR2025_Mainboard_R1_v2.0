/**
 * @file userglobal_config.c
 * @author 用户配置 (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "userglobal_config.h"

/***********************************************************
 * @brief 仪表板消息定义
 * 
 */

mavlink_joystick_air_dashboard_set_title_t mav_run_title;
mavlink_joystick_air_dashboard_set_msg_t mav_run_msg;
char run_title[20]      = "Running Mode";
char run_auto_msg[20]   = "AutoMode";
char run_handle_msg[20] = "HandleMode";
char run_stop_msg[20] = "StopMode";

mavlink_joystick_air_dashboard_set_title_t mav_rst_choose_title;
mavlink_joystick_air_dashboard_set_msg_t mav_rst_choose_msg;
char rst_choose_title[20]  = "RstChoose";
char rst_choose_msg[20] = "NoStart";
char norst_choose_msg[20] = "Start";

mavlink_joystick_air_dashboard_set_title_t mav_dir_choose_title;
mavlink_joystick_air_dashboard_set_msg_t mav_dir_choose_msg;
char direct_choose_title[20] = "DirectChoose";
char direct_choose_msg[20]   = "Waiting";

mavlink_joystick_air_dashboard_set_title_t mav_mode_title;
mavlink_joystick_air_dashboard_set_msg_t mav_mode_msg;
char mode_title[20]    = "Action Mode";
char mode_idle_msg[20] = "IDLEMode";
char mode_shoot_msg[20] = "SHOOTMode";
char mode_intercept_msg[20] = "INTERCEPTMode";
char mode_patball_msg[20] = "PATBALLMode";
char mode_dunk_msg[20] = "DUNKMode";

mavlink_joystick_air_dashboard_set_title_t mav_status_title;
mavlink_joystick_air_dashboard_set_msg_t mav_status_msg;
char status_title[20]    = "Chassis Status";
char status_running_msg[20] = "Running";
char status_ready_msg[20] = "Ready";
char status_stop_msg[20] = "Stop";
char status_error_msg[20] = "Error";