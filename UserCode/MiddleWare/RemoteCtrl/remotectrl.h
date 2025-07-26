#ifndef _REMOTECTEL_H
#define _REMOTECTEL_H
#ifdef __cplusplus
extern "C" {
#endif 
//C
#include "ashining_as69.h"
#include "cmsis_os2.h"

/**
 * @brief 遥控器数据变量
 * 
 */
typedef struct 
{
    float KW01;
    float KW001;
    float KW0001;
    float KW00001;
    float KW_00001;
    bool btn_LeftCrossUp;
    uint32_t btn_LeftCrossUp_press_count;    // 总按下次数
    uint32_t btn_LeftCrossMid_press_count;    // 总按下次数
    uint32_t btn_LeftCrossDown_press_count;    // 总按下次数
    uint32_t btn_LeftCrossRight_press_count;
    uint32_t btn_LeftCrossLeft_press_count;
    uint32_t btn_RightCrossLeft_press_count;
    uint32_t btn_RightCrossRight_press_count;
    uint32_t btn_JoystickL_press_count;
    uint32_t btn_JoystickR_press_count;

    bool btn_LeftCrossDown;
    bool btn_LeftCrossLeft;
    bool btn_LeftCrossRight;
    bool btn_LeftCrossMid;
    bool btn_RightCrossUp;
    bool btn_RightCrossDown;
    bool btn_RightCrossLeft;
    bool btn_RightCrossRight;
    bool btn_RightCrossMid;
    bool btn_Btn0;
    bool btn_Btn1;
    bool btn_Btn2;
    bool btn_Btn3;
    bool btn_Btn4;
    bool btn_Btn5;
    bool btn_KnobL;
    bool btn_KnobR;
    bool btn_JoystickL;
    bool btn_JoystickR;
    bool left_switch;
    bool right_switch;
    float right_y;
    float right_x;
    float left_y;
    float left_x;
    int usr_right_y; /*< -850 , 850 >*/
    int usr_right_x; /*< -850 , 850 >*/
    int usr_left_y;  /*< -850 , 850 >*/
    int usr_left_x;  /*< -850 , 850 >*/
    float right_knob;
    float left_knob;
    float usr_right_knob;
    float usr_left_knob;
}Remote_Data;

typedef struct {
    uint8_t filter_cnt;      // 连续稳定计数
    uint8_t stable_state;    // 稳定后的状态
    uint8_t last_state;      // 上次状态
    bool is_pressed;         // 单次按下标志
    uint32_t total_press;    // 总按下次数 [6](@ref)
} KeyDebounce_t;

extern Remote_Data MyRemote_Data;  
extern Remote_Data MyLastRemote_Data;  


// 对外接口函数
void my_RemoteCtrl_Init(void);
void my_RemoteCtrl_Task_Start(void);

bool BtnScan_Press(bool this_status);
void BtnPress_Once(bool this_btn);



#ifdef __cplusplus
}
#endif
//C++
#endif 
