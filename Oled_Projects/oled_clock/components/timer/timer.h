#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdbool.h>
#include <stdio.h>

#include "driver/gptimer.h"
#include "driver/gptimer_types.h"

/*
Expected state transition: 
UNABLED -> STOPPED -> RUNNING -> PAUSED -> STOPPED -> RUNNING -> ... -> STOPPED -> UNABLED
*/

typedef enum
{
    UNABLED,
    STOPPED,
    RUNNING,
    PAUSED
}TIMER_STATE;

extern gptimer_handle_t htim;
extern TIMER_STATE TIM_STATE;

TIMER_STATE Timer_getState();
bool Timer_Init(uint32_t freq);
bool Timer_Start();
bool Timer_Pause();
bool Timer_Resume();
bool Timer_Stop();
bool Timer_Delete();
uint64_t Timer_getCount();
uint64_t Timer_Micros();
uint64_t Timer_Millis();
uint64_t Timer_Seconds();
void Timer_printState();

#endif //_CLOCK_H_