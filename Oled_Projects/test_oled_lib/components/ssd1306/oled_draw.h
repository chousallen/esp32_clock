#ifndef _OLED_DRAW_H_
#define _OLED_DRAW_H_

#include <stdint.h>

#include "ssd1306.h"


void oled_draw_pixel(SSD1306_t *dev, uint8_t x, uint8_t y, uint8_t color);

#endif //oled_draw.h