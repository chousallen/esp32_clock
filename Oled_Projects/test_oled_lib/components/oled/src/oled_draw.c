#include <stdint.h>
#include <string.h>
#include <math.h>

#include "ssd1306.h"

#include "oled.h"


void Oled_Draw_Pixel(const Pixel_t *pixel)
{
    if(pixel->x >= 128 || pixel->y >= 64)
        return;
    uint8_t page = pixel->y / 8, bit = pixel->y % 8;
    if(pixel->color)
    {
        uint8_t write = 0b00000001 << bit;
        screen_buff[pixel->x + page * 128] |= write;
    }
    else
    {
        uint8_t write = ~(0b00000001 << bit);
        screen_buff[pixel->x + page * 128] &= write;
    }
}

void Oled_Draw_Line(const Pixel_t *start, const Pixel_t *end, uint8_t style)
{
    uint8_t style_mask[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
    Pixel_t tmp, tmp_start, tmp_end;
    uint8_t dx = abs(end->x - start->x), dy = abs(end->y - start->y);
    if(dx >= dy)
    {
        if(start->x > end->x)
        {
            tmp_start = *end;
            tmp_end = *start;
        }
        else
        {
            tmp_start = *start;
            tmp_end = *end;
        }

        if(dy == 0)
        {
            for(tmp.x = tmp_start.x, tmp.y = tmp_start.y; tmp.x < tmp_end.x; tmp.x++)
            {
                tmp.color = (style & style_mask[(tmp.x-tmp_start.x) % 8]) != 0;
                Oled_Draw_Pixel(&tmp);
            }
            tmp.x = tmp_end.x;
            tmp.color = (style & style_mask[(tmp.x-tmp_start.x) % 8]) != 0;
            Oled_Draw_Pixel(&tmp);
        }
        else
        {
            for(tmp.x = tmp_start.x; tmp.x < tmp_end.x; tmp.x++)
            {
                tmp.y = round(tmp_start.y + (double)(tmp_end.y - tmp_start.y) * (tmp.x - tmp_start.x) / (tmp_end.x - tmp_start.x));
                tmp.color = (style & style_mask[(tmp.x-tmp_start.x) % 8]) != 0;
                Oled_Draw_Pixel(&tmp);
            }
            tmp.x = tmp_end.x;
            tmp.y = round(tmp_start.y + (double)(tmp_end.y - tmp_start.y) * (tmp.x - tmp_start.x) / (tmp_end.x - tmp_start.x));
            tmp.color = (style & style_mask[(tmp.x-tmp_start.x) % 8]) != 0;
            Oled_Draw_Pixel(&tmp);
        }
    }
    else
    {
        if(start->y > end->y)
        {
            tmp_start = *end;
            tmp_end = *start;
        }
        else
        {
            tmp_start = *start;
            tmp_end = *end;
        }

        if(dx == 0)
        {
            for(tmp.y = tmp_start.y, tmp.x = tmp_start.x; tmp.y < tmp_end.y; tmp.y++)
            {
                tmp.color = (style & style_mask[(tmp.y-tmp_start.y) % 8]) != 0;
                Oled_Draw_Pixel(&tmp);
            }
            tmp.y = tmp_end.y;
            tmp.color = (style & style_mask[(tmp.y-tmp_start.y) % 8]) != 0;
            Oled_Draw_Pixel(&tmp);
        }
        else
        {
            for(tmp.y = tmp_start.y; tmp.y < tmp_end.y; tmp.y++)
            {
                tmp.x = round(tmp_start.x + (double)(tmp_end.x - tmp_start.x) * (tmp.y - tmp_start.y) / (tmp_end.y - tmp_start.y));
                tmp.color = (style & style_mask[(tmp.y-tmp_start.y) % 8]) != 0;
                Oled_Draw_Pixel(&tmp);
            }
            tmp.y = tmp_end.y;
            tmp.x = round(tmp_start.x + (double)(tmp_end.x - tmp_start.x) * (tmp.y - tmp_start.y) / (tmp_end.y - tmp_start.y));
            tmp.color = (style & style_mask[(tmp.y-tmp_start.y) % 8]) != 0;
            Oled_Draw_Pixel(&tmp);
        }
    }
}

void Oled_Draw_Bitmap(const uint8_t *bitmap, Pixel_t position, uint8_t width, uint8_t height)
{
    Pixel_t tmp;
    for(int y = 0; y < height; y++)
    {
        tmp.y = position.y + y;
        for(int x = 0; x < width; x++)
        {
            tmp.x = position.x + x;
            tmp.color = (bitmap[x + y / 8 * width] & (0b00000001 << (y % 8))) != 0;
            if(tmp.color)
                Oled_Draw_Pixel(&tmp);
        }
    }
}

void Oled_Draw_Circle(const Pixel_t *center, uint8_t radius, uint8_t size)
{
    for(int x = center->x - radius - size; x <= center->x + radius + size; x++)
    {
        for(int y = center->y - radius - size; y <= center->y + radius + size; y++)
        {
            if(fabs(sqrt(pow(x - center->x, 2) + pow(y - center->y, 2))-radius) < 0.5*size)
            {
                Pixel_t tmp = {x, y, 1};
                Oled_Draw_Pixel(&tmp);
            }
        }
    }
}
