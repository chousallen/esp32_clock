#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "oled.h"
#include "timer.h"
#include "encoder.h"

#include "logo_bitmap.h"
#include "numbers_bitmap.h"
#include "small_numbers_bitmap.h"
#include "digital_clock.h"

#define PI 3.14159265

typedef enum
{
    BUTTON_CLICK,
    BUTTON_PRESS
}BUTTON_STATE;

typedef enum
{
    SHOW_LOGO,
    DIGITAL_CLOCK, 
    DIGITAL_CLOCK_SET_SECOND,
    DIGITAL_CLOCK_SET_MINUTE,
    DIGITAL_CLOCK_SET_HOUR,
    ANALOG_CLOCK
}SCREEN_STATE;

typedef enum
{
    HOUR,
    MINUTE,
    SECOND
}Hand_type_t;

typedef double rad_angle;

struct Hands
{
    Hand_type_t type: 2;
    uint8_t len: 7;
    uint8_t size: 3;
    uint8_t style;
    rad_angle angle;
    Pixel_t position;
};

QueueHandle_t button_queue = 0;

SCREEN_STATE screen_state = DIGITAL_CLOCK;
BUTTON_STATE button = 0;
Pixel_t center = {97, 31, 1};
uint8_t radius = 30;
Time_t time = {0, 0, 0, 0};
Time_t ori_time = {16, 30, 0, 0};
struct Hands hour_hand = {.type = HOUR, .len = 15, .size = 2, .style = 0xff, .angle = PI/2, .position = {97, 31, 1}};
struct Hands minute_hand = {.type = MINUTE, .len = 25, .size = 1, .style = 0xff, .angle = PI/2, .position = {97, 31, 1}};
struct Hands second_hand = {.type = SECOND, .len = 30, .size = 1, .style = 0b11001100, .angle = PI/2, .position = {97, 31, 1}};

Time_t getTime(int hour, int minute, int second)
{
    int total_second = hour*3600 + minute*60 + second;
    for(; total_second < 0; total_second += 86400);
    for(; total_second >= 86400; total_second -= 86400);
    Time_t tmp = {total_second/3600%24, (total_second/60)%60, total_second%60, 0};
    return tmp;
}

void updateTime()
{
    time = getTime((Timer_Seconds()/3600)%24 + ori_time.hour, (Timer_Seconds()/60)%60 + ori_time.minute, (Timer_Seconds()%60 + ori_time.second));
    time.millisecond = (Timer_Millis()%1000 + ori_time.millisecond)%1000;
    //time.second = (Timer_Seconds()%60 + ori_time.second)%60;
    //time.minute = ((Timer_Seconds()/60)%60 + ori_time.minute)%60;
}

void buttonInit()
{
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_NUM_4, GPIO_PULLUP_ONLY);
    gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_NUM_0, GPIO_PULLUP_ONLY);
}

void buttonHandle()
{
    uint16_t press_time = 0;
    uint8_t last = 0, curr = 0;
    while(1)
    {
        curr = !gpio_get_level(GPIO_NUM_4);
        if(!last && curr == 1)
        {
            press_time = 0;
        }
        else if(last && curr)
        {
            press_time += 100;
        }
        else if (last && !curr)
        {
            if(press_time >= 1000)
            {
                button = BUTTON_PRESS;
                xQueueSend(button_queue, &button, 0);
            }
            else
            {
                button = BUTTON_CLICK;
                xQueueSend(button_queue, &button, 0);
            }
        }
        
        last = curr;
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void setTime(void *tmp)
{
    SCREEN_STATE *state = (SCREEN_STATE*)tmp;
    Time_t original_time = ori_time;
    Encoder_Start();
    
    while(1)
    {
        switch(*state)
        {
            case DIGITAL_CLOCK_SET_SECOND:
                ori_time = getTime(original_time.hour, original_time.minute, original_time.second + Encoder_getCount());
                break;
            case DIGITAL_CLOCK_SET_MINUTE:
                ori_time = getTime(original_time.hour, original_time.minute + Encoder_getCount(), original_time.second);
                break;
            case DIGITAL_CLOCK_SET_HOUR:
                ori_time = getTime(original_time.hour + Encoder_getCount(), original_time.minute, original_time.second);
                break;
            default:
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void stateHandle()
{
    BUTTON_STATE tmp;
    TaskHandle_t hSetTime = NULL;
    while(1)
    {
        if(!gpio_get_level(GPIO_NUM_0))
        {
            screen_state = SHOW_LOGO;
            vTaskDelay(pdMS_TO_TICKS(500));
            continue;
        }
        if(screen_state == SHOW_LOGO)
        {
            screen_state = DIGITAL_CLOCK;
        }
        if(xQueueReceive(button_queue, &tmp, 0))
        {
            if(tmp == BUTTON_CLICK)
            {
                switch(screen_state)
                {
                    case DIGITAL_CLOCK_SET_SECOND:
                        vTaskDelete(hSetTime);
                        screen_state = DIGITAL_CLOCK_SET_MINUTE;
                        xTaskCreate(setTime, "setTime", 2048, &screen_state, 5, &hSetTime);
                        break;
                    case DIGITAL_CLOCK_SET_MINUTE:
                        vTaskDelete(hSetTime);
                        screen_state = DIGITAL_CLOCK_SET_HOUR;
                        xTaskCreate(setTime, "setTime", 2048, &screen_state, 5, &hSetTime);
                        break;
                    case DIGITAL_CLOCK_SET_HOUR:
                        vTaskDelete(hSetTime);
                        screen_state = DIGITAL_CLOCK;
                        break;
                    default:
                        break;
                }
            }
            if(tmp == BUTTON_PRESS)
            {
                switch(screen_state)
                {
                    case DIGITAL_CLOCK:
                        screen_state = DIGITAL_CLOCK_SET_SECOND;
                        xTaskCreate(setTime, "setTime", 2048, &screen_state, 5, &hSetTime);
                        break;
                    default:
                        break;
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void displayCurrMode(SCREEN_STATE mode)
{
    switch(mode)
    {
        case SHOW_LOGO:
            Oled_Draw_Bitmap(bitmap_logo, (Pixel_t){0, 0, 1}, BITMAP_LOGO_WIDTH, BITMAP_LOGO_HEIGHT);
            Oled_Show();
            Oled_Clear(0x00);
            break;
        case DIGITAL_CLOCK:
            displayDigitalClock(&time);
            break;
        case DIGITAL_CLOCK_SET_SECOND:
            drawDigitalUnderline(DIGITAL_SET_SECOND);
            displayDigitalClock(&time);
            break;
        case DIGITAL_CLOCK_SET_MINUTE:
            drawDigitalUnderline(DIGITAL_SET_MINUTE);
            displayDigitalClock(&time);
            break;
        case DIGITAL_CLOCK_SET_HOUR:
            drawDigitalUnderline(DIGITAL_SET_HOUR);
            displayDigitalClock(&time);
            break;
        case ANALOG_CLOCK:
            printf("Analog Clock\n");
            break;
        
        default:
            break;
    }
}

void app_main(void)
{
    Timer_Init(1000000);
    Timer_Start();
    Oled_Init(0x00);
    Oled_Clear(0x00);
    buttonInit();
    Encoder_Init();
    button_queue = xQueueCreate(1, sizeof(BUTTON_STATE));
    xTaskCreate(buttonHandle, "buttonHandle", 2048, NULL, 5, NULL);
    xTaskCreate(stateHandle, "stateHandle", 2048, NULL, 6, NULL);

    Oled_Draw_Bitmap(bitmap_logo, (Pixel_t){0, 0, 1}, BITMAP_LOGO_WIDTH, BITMAP_LOGO_HEIGHT);
    Oled_Show();
    Oled_Clear(0x00);
    vTaskDelay(pdMS_TO_TICKS(5000));

    while (1)
    {
        updateTime();
        displayCurrMode(screen_state);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    

}
