/*
 * @Description: 
 * @Author: Alex
 * @Date: 2025-03-22 10:35:23
 * @LastEditors: Alex
 * @LastEditTime: 2025-03-22 14:31:31
 */
#ifndef _HANDLE_PATBALL_TASH_H
#define _HANDLE_PATBALL_TASH_H
#ifdef __cplusplus
extern "C" {
#endif

#include "Expansion.h"
#include "usermain.h"
#include "remotectrl.h"
#include "Unitree.h"

typedef enum
{
    PATBALL_IDLE,
    PATBALL_PATING,
}PATBALLSTATE;

extern osThreadId_t Patball_TaskHandle;
extern PATBALLSTATE patballstate;
void Patball_TaskStart();
void Patball_Task(void* argument);

#ifdef __cplusplus
}
#endif
#endif // !