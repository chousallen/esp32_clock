#ifndef _OLED_H_
#define _OLED_H_
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "ssd1306.h"

extern SSD1306_t dev;
extern uint8_t screen_buff[128*8];

typedef struct
{
    uint8_t x;
    uint8_t y: 7;
    uint8_t color: 1;
}Pixel_t;

void printPixel(const Pixel_t *pixel);

void Oled_Init(uint8_t contrast);
void Oled_Contrast(uint8_t contrast);
void Oled_Show();
void Oled_Clear(uint8_t style);
void Oled_setScreen(const uint8_t *buff);
const uint8_t* Oled_getScreen();

void Oled_Draw_Pixel(const Pixel_t *pixel);
void Oled_Draw_Line(const Pixel_t *start, const Pixel_t *end, uint8_t style);
void Oled_Draw_Bitmap(const uint8_t *bitmap, Pixel_t position, uint8_t width, uint8_t height);
void Oled_Draw_Circle(const Pixel_t *center, uint8_t radius, uint8_t size);

#endif //_OLED_H_