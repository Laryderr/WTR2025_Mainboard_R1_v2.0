#ifndef __USERMAIN_H
#define __USERMAIN_H
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stdbool.h"
#include "cmsis_os2.h"

#include "encoder.h"
#include "circular_buffer.h"

#include "remotectrl.h"
#include "chassis_ctrl.h"
#include "Unitree.h"
#include "odom.h"
#include "gyro.h"
#include "my_IWDG.h"
#include "NUC_msg.h"
#include "Laser.h"



#include "Handle_task.h"
#include "Handle_Patball_task.h"
#include "Handle_Dunk_task.h"
#include "Handle_Intercept_task.h"
#include "Handle_Shoot_task.h"


#include "userglobal_config.h"
#include "userdebug.h"
#include "usermain_tasks.h"






#ifdef __cplusplus
}
#endif
#endif