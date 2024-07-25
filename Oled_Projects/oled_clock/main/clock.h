#ifndef _CLOCK_H_
#define _CLOCK_H_

#include "timer.h"

void clockHandle()
{
    Timer_Init(1000000);
}

#endif //_CLOCK_H_