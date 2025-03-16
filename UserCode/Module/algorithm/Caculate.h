#ifndef _CACULATE_H__
#define _CACULATE_H__
#ifdef __cplusplus
extern "C"
{
#endif
#include "DJI.h"
void PosePID_Calc(PID_t *pid);
void positionServo(float ref, DJI_t * motor);

void speedServo(float ref, DJI_t * motor);
float mps_to_rpm(float velocity);

#ifdef __cplusplus
}
#endif


#endif
