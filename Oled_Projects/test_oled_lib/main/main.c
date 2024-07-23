#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "ssd1306.h"
#include "oled_draw.h"

#include "timer.h"
#include "bitmap.h"

#define REPEAT_8_TIMES(x) x x x x x x x x

SSD1306_t dev;
uint8_t myBuffer[128*64/8] = {0xff, 0x00, 0b10101010, 0b01010101};

uint8_t contrast = 0;
bool button = 0;

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
    while(1)
    {
        button = !gpio_get_level(0);
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

void contrastHandle()
{}

void timeOled()
{
    for(int cnt = 1; cnt <= 10; cnt++)
    {

    }
}


void app_main(void)
{
    Timer_Init(40000000);
    Timer_Start();
    printf("start count %llu\n", Timer_getCount());
    printf("start at %lluus\n", Timer_Micros());
    oledInit();
    
    ssd1306_contrast(&dev, 0x00);
    ssd1306_clear_screen(&dev, 0);

    xTaskCreate(buttonHandle, "buttonHandle", 2048, NULL, 1, NULL);

    for(int x=0; x<128; x++)
    {
        _ssd1306_pixel(&dev, x, round(x/2), 0);
    }
    _ssd1306_line(&dev, 127, 0, 0, 63, 0);
    ssd1306_show_buffer(&dev);
    printf("show once time: %lluus\n", Timer_Micros());

    int background = 0;
    REPEAT_8_TIMES(ssd1306_clear_screen(&dev, (background = !background));)
    printf("clear 8 times: %lluus\n", Timer_Micros());

    REPEAT_8_TIMES(ssd1306_show_buffer(&dev);)
    printf("show 8 times: %lluus\n", Timer_Micros());


    int contrast = 0;
    REPEAT_8_TIMES(ssd1306_contrast(&dev, (contrast = 0xff - contrast));)
    printf("contrast 8 times: %lluus\n", Timer_Micros());
    printf("app_main: %lluus\n", Timer_Micros());
    printf("end count %llu\n", Timer_getCount());

    uint64_t last_count = Timer_getCount();
    while(Timer_getCount() > last_count)
    {
        last_count = Timer_getCount();
        printf("current count: %#llx\n", last_count);
        vTaskDelay(1);
    }

}
