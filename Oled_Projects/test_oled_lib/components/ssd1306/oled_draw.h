#ifndef _OLED_DRAW_H_
#define _OLED_DRAW_H_

#include <stdint.h>
#include <math.h>

#include "ssd1306.h"


void oled_draw_pixel(SSD1306_t *dev, uint8_t x, uint8_t y, uint8_t color);
void oled_draw_line(SSD1306_t *dev, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t style);

#endif //oled_draw.h