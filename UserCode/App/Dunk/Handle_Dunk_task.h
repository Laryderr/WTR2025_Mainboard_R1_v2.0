#ifndef __HANDLE_DUNK_TASK_H
#define __HANDLE_DUNK_TASK_H
#ifdef __cplusplus
extern "C" {
#endif

#include "usermain.h"
#include "DJI.h"

#define Max_Extension_Angle 16  //轮腿电机从抬腿到伸腿的最大正转弧度
#define Max_ThrowBall_Angle 18.9 //投球电机旋转总角度（已乘减速比）
#define Encoder_VertPos 94       ////投篮臂垂直时编码器角度
#define DegreetoRad     6.283


enum Dunk_Status{
    DUNK_IDLE = 0,
    DUNK_STAND_UP,  //站立
    DUNK_SQUAT,     //下蹲
    DUNK_JUMP_UP,   //跳起
    DUNK_TEST,      //测试
};

/*enum ThrowBall_Status{
    ThrowBALL_UP = 0, //向上投球
    ThrowBALL_DOWN, //下放
};*/

typedef struct{
    enum Dunk_Status my_Dunk_Status; //当前的状态
    enum Dunk_Status mylast_Dunk_Status; //上一次的状态
    //站立
    uint8_t my_StatusTransition_cnt; //状态切换计数器
    uint8_t Found_LimitingPos_Flag; //是否找到底盘跳跃电机极限位置
    bool Stood_Flag[3];             //三条腿伸直完成标志
    bool throw_delay_flag ;//站立后延时稳定标志位
    uint8_t Jump_Completed_Flag ;   //是否完成一次跳跃
    //投篮
    float Throwball_InitialPos[2]; //投球电机初始位置
    bool Foundthrow_InitialPos_Flag; //是否找到投球电机初始位置
    bool Time_TO_Jump ; //投球臂是否转动到跳跃时机
    uint8_t Throwball_Flag; //投球状态标志

    bool Buffer_flag ; //1开启
    bool BallHelp_flag ; //1 on
    float Unitree_DunkMotor_LimitingPos[2][3]; // 0:抬腿极限位置，1：伸腿极限位置

    int32_t Encoder_InitialPos ; //投篮臂上电时编码器角度
    PID_t Encoder_PosPID_t; //投篮臂角编码器位置PID结构体
}Dunk_Task_t;

extern Dunk_Task_t my_Dunk_Task_t;



//外部接口函数定义
extern osThreadId_t unitree_dunk_ctrl_TaskHandle;
void Handle_Dunk_TaskStart(void);
void Encoder_Pospid_Init(PID_t *upid,float ref, float KP, float KI, float KD);
float Encoder_PosServo(PID_t *upid, float Feedback_value);

#ifdef __cplusplus
}
#endif
// C++
#endif // 