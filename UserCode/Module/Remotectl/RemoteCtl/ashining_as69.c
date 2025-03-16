/* @file chassis_communicate.c
 * @author OriTwil
 * @brief 遥控器库函数&给遥控器发dashboard
 * @date 2023-5-18
 *
 */

#include "ashining_as69.h"
#if (USE_FREERTOS == 1)
#include "cmsis_os.h"
#endif

mavlink_joystick_air_t msg_joystick_air;               //按钮，操纵杆，旋钮，开关
mavlink_joystick_air_dashboard_del_t mav_joystick_del;  //仪表板
mavlink_joystick_air_led_t msg_joystick_air_led;

/***********************************用户函数段****************************************/

/**
 * @brief   遥控器初始化
 * @param
 */
void AS69_RemoteControl_Init()
{
    wtrMavlink_BindChannel(&AS69_UART_HANDLE, MAVLINK_COMM_0); // MAVLINK遥控器
    wtrMavlink_StartReceiveIT(MAVLINK_COMM_0);                 // 以mavlink接收遥控器

    //上位机配置用
    HAL_GPIO_WritePin(AS69_MD0_GPIO_Port,AS69_MD0_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(AS69_MD1_GPIO_Port,AS69_MD1_Pin,GPIO_PIN_SET);
}


/**
 * @brief 遥控器LED信息发送函数
 * 
 * @param r <   range: [0, 1] 
 * @param g     range: [0, 1]
 * @param b     range: [0, 1]
 * @param lightness Gain of lightness. Must be positive values
 * @param duration  Led will be on for this milliseconds
 * @param msg_joystick_air_led 
 */
void JoystickSwitchLED(float r, float g, float b, float lightness, uint16_t duration, mavlink_joystick_air_led_t *msg_joystick_air_led)
{
    msg_joystick_air_led->r                           = r;
    msg_joystick_air_led->g                           = g;
    msg_joystick_air_led->b                           = b;
    msg_joystick_air_led->lightness                   = lightness;
    msg_joystick_air_led->duration                    = duration;
    mavlink_joystick_air_led_t msg_joystick_send_temp = *msg_joystick_air_led;

    mavlink_msg_joystick_air_led_send_struct(MAVLINK_COMM_0, &msg_joystick_send_temp);
#if (USE_FREERTOS == 1)
    osDelay(2);
#else
    HAL_Delay(2);
#endif
}

/**
 * @brief   遥控器Title信息发送函数,设置遥控器上的标题信息
 */
void JoystickSwitchTitle(uint8_t id, char title[20], mavlink_joystick_air_dashboard_set_title_t *msg_joystick_air_title)
{

    msg_joystick_air_title->id = id;
    strncpy(msg_joystick_air_title->title, title, 20);
    mavlink_joystick_air_dashboard_set_title_t msg_joystick_air_title_temp = *msg_joystick_air_title;
    mavlink_msg_joystick_air_dashboard_set_title_send_struct(MAVLINK_COMM_0, &msg_joystick_air_title_temp);
#if (USE_FREERTOS == 1)
    osDelay(25);
#else
    HAL_Delay(25);
#endif
}

/**
 * @brief   遥控器Msg信息发送函数
 */
void JoystickSwitchMsg(uint8_t id, char message[20], mavlink_joystick_air_dashboard_set_msg_t *msg_joystick_air_msg)
{
    msg_joystick_air_msg->id = id;
    strncpy(msg_joystick_air_msg->message, message, 20);
    mavlink_joystick_air_dashboard_set_msg_t msg_joystick_air_msg_temp = *msg_joystick_air_msg;
    mavlink_msg_joystick_air_dashboard_set_msg_send_struct(MAVLINK_COMM_0, &msg_joystick_air_msg_temp);
#if (USE_FREERTOS == 1)
    osDelay(25);
#else
    HAL_Delay(25);
#endif
}

/**
 * @brief   遥控器Del信息发送函数，在遥控器仪表盘中删除一个显示项，
 *          将特定 ID 的仪表板项删除指令发送给遥控器
 */
void JoystickDelete(uint8_t id, mavlink_joystick_air_dashboard_del_t *msg_joystick_air_delete)
{
    msg_joystick_air_delete->id  = id;
    mavlink_joystick_air_dashboard_del_t msg_joystick_air_delete_temp = *msg_joystick_air_delete;
    mavlink_msg_joystick_air_dashboard_del_send_struct(MAVLINK_COMM_0, &msg_joystick_air_delete_temp);
#if (USE_FREERTOS == 1)
    osDelay(25);
#else
    HAL_Delay(25);
#endif
}

/**
 * @brief   遥控器Button信息解码函数
 */
bool ReadJoystickButtons(mavlink_joystick_air_t *msg_joystick_air_, KEYS index)
{
#if (USE_FREERTOS == 1)
    taskENTER_CRITICAL();//避免中断影响数据读取安全
#endif
    mavlink_joystick_air_t msg_joystick_air_temp = *msg_joystick_air_;
#if (USE_FREERTOS == 1)
    taskEXIT_CRITICAL();
#endif
    return ((msg_joystick_air_temp.buttons >> (index - 1)) & 1);
}

/**
 * @brief   遥控器左侧横摇杆信息解码函数
 */
float ReadJoystickLeft_x(mavlink_joystick_air_t *msg_joystick_air_)
{
#if (USE_FREERTOS == 1)
    taskENTER_CRITICAL();
#endif
    mavlink_joystick_air_t msg_joystick_air_temp = *msg_joystick_air_;
#if (USE_FREERTOS == 1)
    taskEXIT_CRITICAL();
#endif
    return msg_joystick_air_temp.joystickL[0];
}

/**
 * @brief   遥控器左侧纵摇杆信息解码函数
 */
float ReadJoystickLeft_y(mavlink_joystick_air_t *msg_joystick_air_)
{
#if (USE_FREERTOS == 1)
    taskENTER_CRITICAL();
#endif
    mavlink_joystick_air_t msg_joystick_air_temp = *msg_joystick_air_;
#if (USE_FREERTOS == 1)
    taskEXIT_CRITICAL();
#endif
    return msg_joystick_air_temp.joystickL[1];
}

/**
 * @brief   遥控器右侧横摇杆信息解码函数
 */
float ReadJoystickRight_x(mavlink_joystick_air_t *msg_joystick_air_)
{
#if (USE_FREERTOS == 1)
    taskENTER_CRITICAL();
#endif
    mavlink_joystick_air_t msg_joystick_air_temp = *msg_joystick_air_;
#if (USE_FREERTOS == 1)
    taskEXIT_CRITICAL();
#endif
    return msg_joystick_air_temp.joystickR[0];
}

/**
 * @brief   遥控器右侧纵摇杆信息解码函数
 */
float ReadJoystickRight_y(mavlink_joystick_air_t *msg_joystick_air_)
{
#if (USE_FREERTOS == 1)
    taskENTER_CRITICAL();
#endif
    mavlink_joystick_air_t msg_joystick_air_temp = *msg_joystick_air_;
#if (USE_FREERTOS == 1)
    taskEXIT_CRITICAL();
#endif
    return msg_joystick_air_temp.joystickR[1];
}

/**
 * @brief   遥控器左侧旋轮信息解码函数
 */
int16_t ReadJoystickKnobsLeft(mavlink_joystick_air_t *msg_joystick_air_)
{
#if (USE_FREERTOS == 1)
    taskENTER_CRITICAL();
#endif
    mavlink_joystick_air_t msg_joystick_air_temp = *msg_joystick_air_;
#if (USE_FREERTOS == 1)
    taskEXIT_CRITICAL();
#endif
    return msg_joystick_air_temp.knobs[0];
}

/**
 * @brief   遥控器右侧侧旋轮信息解码函数
 */
int16_t ReadJoystickKnobsRight(mavlink_joystick_air_t *msg_joystick_air_)
{
#if (USE_FREERTOS == 1)
    taskENTER_CRITICAL();
#endif
    mavlink_joystick_air_t msg_joystick_air_temp = *msg_joystick_air_;
#if (USE_FREERTOS == 1)
    taskEXIT_CRITICAL();
#endif

    return msg_joystick_air_temp.knobs[1];
}

/**
 * @brief   遥控器开关信息解码函数
 */
bool ReadJoystickSwitchs(mavlink_joystick_air_t *msg_joystick_air_, SWITCHS index)
{
#if (USE_FREERTOS == 1)
    taskENTER_CRITICAL();
#endif
    mavlink_joystick_air_t msg_joystick_air_temp = *msg_joystick_air_;
#if (USE_FREERTOS == 1)
    taskEXIT_CRITICAL();
#endif
    return ((msg_joystick_air_temp.switchs >> index) & 1);
}
