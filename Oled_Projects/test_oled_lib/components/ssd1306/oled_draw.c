#include "oled_draw.h"

void oled_draw_pixel(SSD1306_t *dev, uint8_t x, uint8_t y, uint8_t color)
{
    uint8_t page = y / 8, bit = y % 8, *buff;
    buff = calloc(128*64/8, sizeof(uint8_t));
    ssd1306_get_buffer(dev, buff);
    if(color)
        buff[x + page * 128] |= 0b00000001 << bit;
    else
        buff[x + page * 128] &= ~(0b00000001 << bit);
    ssd1306_set_buffer(dev, buff);
    free(buff);

}

void oled_draw_line(SSD1306_t *dev, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t style)
{
    int16_t dx, dy, sx, sy, err, e2;
    dx = abs(x1 - x0);
    dy = abs(y1 - y0);
    sx = x0 < x1 ? 1 : -1;
    sy = y0 < y1 ? 1 : -1;
    err = (dx > dy ? dx : -dy) / 2;
    for (;;)
    {
        oled_draw_pixel(dev, x0, y0, style);
        if (x0 == x1 && y0 == y1)
            break;
        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }
}