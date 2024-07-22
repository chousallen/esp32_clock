#include "oled_draw.h"

void oled_draw_pixel(SSD1306_t *dev, uint8_t x, uint8_t y, uint8_t color)
{
    uint8_t page = y / 8, bit = y % 8, *buff;
    buff = calloc(128*64/8, sizeof(uint8_t));
    ssd1306_get_buffer(dev, buff);
    buff[x + page * 128] |= 0b00000001 << bit;
    ssd1306_set_buffer(dev, buff);
    free(buff);

}