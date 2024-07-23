#ifndef _TEST_H_
#define _TEST_H_

#include <stdio.h>
#include <stdint.h>

uint8_t static pri = 0;

uint8_t getter()
{
    return pri;
}

void setter(uint8_t val)
{
    pri = val;
}

void print()
{
    printf("pri: %d\n", pri);
}

#endif // _TEST_H_