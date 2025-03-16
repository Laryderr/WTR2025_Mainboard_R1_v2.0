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
    bool btn_LeftCrossUp;
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

extern Remote_Data MyRemote_Data;  
extern Remote_Data MyLastRemote_Data;  


// 对外接口函数
void my_RemoteCtrl_Init(void);
void my_RemoteCtrl_Task_Start(void);

bool BtnScan_Press(bool this_status,bool last_status);



#ifdef __cplusplus
}
#endif
//C++
#endif 
