#include <stdint.h>
#include <string.h>
#include <math.h>

#include "ssd1306.h"

#include "oled.h"


SSD1306_t dev;
uint8_t screen_buff[128*8] = {0};

void printPixel(const Pixel_t *pixel)
{
    printf("{x: %d, y: %d, color: %d}", pixel->x, pixel->y, pixel->color);
}

void Oled_Init(uint8_t contrast)
{
    i2c_master_init(&dev, 21, 22, -1);
    ssd1306_init(&dev, 128, 64);
    ssd1306_contrast(&dev, contrast);
    memset(screen_buff, 0x00, sizeof(screen_buff));
}

void Oled_Contrast(uint8_t contrast)
{
    ssd1306_contrast(&dev, contrast);
}

void Oled_Show()
{
    ssd1306_set_buffer(&dev, screen_buff);
    ssd1306_show_buffer(&dev);
}

void Oled_Clear(uint8_t style)
{
    memset(screen_buff, style, sizeof(screen_buff));
}

void Oled_setScreen(const uint8_t *buff)
{
    memcpy(screen_buff, buff, sizeof(screen_buff));
}

const uint8_t* Oled_getScreen()
{
    return screen_buff;
}
