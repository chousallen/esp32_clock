#include <stdio.h>

#include "driver/gpio.h"

#include "ssd1306.h"
#include "oled_draw.h"

#include "bitmap.h"

#define REPEAT_8_TIMES(x) x x x x x x x x

SSD1306_t dev;
uint8_t myBuffer[128*64/8] = {0xff, 0x00, 0b10101010, 0b01010101};

uint8_t contrast = 0;

void oledInit()
{
    i2c_master_init(&dev, 21, 22, -1);
    ssd1306_init(&dev, 128, 64);
    ssd1306_contrast(&dev, 0xff);
}

void buttonHandle()
{
    gpio_set_direction(0, GPIO_MODE_INPUT);
    gpio_set_pull_mode(0, GPIO_PULLUP_ONLY);
    gpio_set_direction(2, GPIO_MODE_OUTPUT);
    gpio_set_level(2, 0);
}


void app_main(void)
{
    oledInit();
    printf("init done\n");
    vTaskDelay(1000/portTICK_PERIOD_MS);
    
    ssd1306_clear_screen(&dev, 0x00);
    printf("clear done\n");
    vTaskDelay(1000/portTICK_PERIOD_MS);

    for(uint8_t x = 0; x<128; x++)
    {
        for(uint8_t y = 0; y<64; y++)
        {
            oled_draw_pixel(&dev, x, y, 1);
            //vTaskDelay(100/portTICK_PERIOD_MS);
        }
    }
    printf("draw done\n");
    vTaskDelay(1000/portTICK_PERIOD_MS);
    
    ssd1306_show_buffer(&dev);
    printf("show done\n");
    
    ssd1306_clear_screen(&dev, 0xff);

    while (1)
    {
        for(uint8_t contrast = 0x00; contrast != 0xff; contrast++)
        {
            ssd1306_contrast(&dev, contrast);
            vTaskDelay(10/portTICK_PERIOD_MS);
        }
    }
    
    

}
