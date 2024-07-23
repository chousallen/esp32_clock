#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "oled.h"
#include "timer.h"
#include "bitmap.h"
#include "bitmap_tmp.h"

#define REPEAT_8_TIMES(x) x x x x x x x x

bool button = 0;

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

void app_main(void)
{
    Timer_Init(40000000);
    Timer_Start();
    Oled_Init(0x00);

    Oled_Clear(0x00);
    Oled_Show();

    Pixel_t tmp = {70, 32, 1};
    for(uint8_t s = 1; s<20; s++)
    {
        Oled_Draw_Circle(&tmp, 20, s);
        Oled_Show();
        vTaskDelay(pdMS_TO_TICKS(200));
        Oled_Clear(0x00);
    }



    xTaskCreate(buttonHandle, "buttonHandle", 2048, NULL, 1, NULL);

    printf("app_main time: %llums\n", Timer_Millis());

}
