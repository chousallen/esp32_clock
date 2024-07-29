#ifndef _ENCODER_H_
#define _ENCODER_H_

#include <stdint.h>

#define ENCODER_CNT_MIN INT16_MIN
#define ENCODER_CNT_MAX INT16_MAX
#define ENCODER_PIN_A 16
#define ENCODER_PIN_B 17
#define ENCODER_GLITCH_TIME 1000 //in ns

typedef enum
{
    ENCODER_DISABLED,
    ENCODER_STOPPED,
    ENCODER_COUNTING,
    ENCODER_PAUSED
}ENCODER_STATE;

void Encoder_Init();
void Encoder_Start();
void Encoder_Pause();
void Encoder_Stop();
void Encoder_Delete();
int16_t Encoder_getCount();

#endif