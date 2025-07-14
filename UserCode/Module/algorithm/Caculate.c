/*pid算法和速度位置伺服*/

#include "Caculate.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"

//增量式PID算法
void PID_Calc(PID_t *pid){
	pid->cur_error = pid->ref - pid->fdb;
	pid->output += pid->KP * (pid->cur_error - pid->error[1]) + pid->KI * pid->cur_error + pid->KD * (pid->cur_error - 2 * pid->error[1] + pid->error[0]);
	pid->error[0] = pid->error[1];
	pid->error[1] = pid->ref - pid->fdb;
	/*设定输出上限*/
	if(pid->output > pid->outputMax) pid->output = pid->outputMax;
	if(pid->output < -pid->outputMax) pid->output = -pid->outputMax;

}

//比例算法
void P_Calc(PID_t *pid){
	pid->cur_error = pid->ref - pid->fdb;
	pid->output = pid->KP * pid->cur_error;
	/*设定输出上限*/
	if(pid->output > pid->outputMax) pid->output = pid->outputMax;
	if(pid->output < -pid->outputMax) pid->output = -pid->outputMax;
	
	if(fabs(pid->output)<pid->outputMin)
		pid->output=0;
}

/*位置式PID*/
void PosePID_Calc(PID_t *pid)
{
    pid->integral += pid->cur_error;

    /*防止积分饱和*/
    if (pid->integral > pid->integralMax)
        pid->integral = pid->integralMax;
    if (pid->integral < pid->integralMin)
        pid->integral = pid->integralMin;

    pid->output = pid->KP * pid->cur_error + pid->KI * pid->integral + pid->KD * (pid->error[1] - pid->error[0]);
    pid->error[0] = pid->error[1];
    pid->error[1] = pid->cur_error;

    /*设定输出上限*/
    if (pid->output > pid->outputMax)
        pid->output = pid->outputMax;
    if (pid->output < -pid->outputMax)
        pid->output = -pid->outputMax;

	if(fabs(pid->output) < pid->outputMin)
		pid->output = 0;
}


/**
 * @brief 位置伺服函数
 * @param ref <输出轴>转向角度，单位度
 */
void positionServo(float ref, DJI_t * motor){
	
	motor->posPID.ref = ref;
	motor->posPID.fdb = motor->AxisData.AxisAngle_inDegree;
	PID_Calc(&motor->posPID);
	
	motor->speedPID.ref = motor->posPID.output;
	motor->speedPID.fdb = motor->FdbData.rpm;
	PID_Calc(&motor->speedPID);

}

/**
 * @brief 速度伺服函数
 * @param ref <电机转子(尾部)>转速，单位rpm
 */
void speedServo(float ref, DJI_t * motor){
	motor->speedPID.ref = ref;
	motor->speedPID.fdb = motor->FdbData.rpm;
	PID_Calc(&motor->speedPID);
}


/**************************************************************
 * @brief 线速度m/s转转速rpm
 * 
 * @param velocity 
 * @return float 
 */
float mps_to_rpm(float velocity)
{
    return ((velocity * 60.0f) / (2.0f * 3.1415926) );
}

void float_to_char(float value, char* buffer, int decimals) {
    int integer = (int)value;
    int decimal = (int)((value - integer) * pow(10, decimals));  // 提取小数部分
    sprintf(buffer, "%d.%02d", integer, abs(decimal));  // 格式化为字符串
}
