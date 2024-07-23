#ifndef _OLED_H_
#define _OLED_H_
#include <stdint.h>

#include "ssd1306.h"

static SSD1306_t dev;
static uint8_t screen_buff[128*8];

typedef struct
{
    uint8_t x: 7;
    uint8_t y: 6;
}XY_COOR;


void Oled_Init(uint8_t contrast);
void Oled_Show();
void Oled_setScreen(const uint8_t *buff);
const uint8_t* Oled_getScreen();

void Oled_Draw_Pixel();

void Oled_Init(uint8_t contrast)
{
    i2c_master_init(&dev, 21, 22, -1);
    ssd1306_init(&dev, 128, 64);
    ssd1306_contrast(&dev, contrast);
}

#endif //_OLED_H_