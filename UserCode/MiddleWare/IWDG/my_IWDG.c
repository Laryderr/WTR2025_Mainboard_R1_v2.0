/**
 * @file IWDG.c
 * @author Lary (you@domain.com)
 * @brief  独立看门狗实现远程复位
 * @version 0.1
 * @date 2025-04-06
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include"my_IWDG.h"

osThreadId_t IWDG_MonitorHandle;
const osThreadAttr_t IWDG_Monitor_attributes = {
  .name = "IWDG_Monitor_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};

void my_IWDG_TaskStart(void)
{
  IWDG_MonitorHandle = osThreadNew(my_IWDG_Task,NULL,&IWDG_Monitor_attributes);
}

void my_IWDG_Task(void *argument)
{
  for(;;)
  {
    osDelay(1);
    //HAL_IWDG_Refresh(&hiwdg); 
  }
}