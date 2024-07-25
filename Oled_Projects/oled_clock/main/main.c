#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "oled.h"
#include "timer.h"

//#include "clock.h"

#define PI 3.14159265

typedef struct
{
    uint8_t hour: 6;
    uint8_t minute: 7;
    uint8_t second: 7;
    uint16_t millisecond: 11;
}Time_t;

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

Pixel_t center = {97, 31, 1};
uint8_t radius = 30;
Time_t time = {0, 0, 0, 0};
const Time_t ori_time = {17, 10, 0, 0};
struct Hands hour_hand = {.type = HOUR, .len = 15, .size = 2, .style = 0xff, .angle = PI/2, .position = {97, 31, 1}};
struct Hands minute_hand = {.type = MINUTE, .len = 25, .size = 1, .style = 0xff, .angle = PI/2, .position = {97, 31, 1}};
struct Hands second_hand = {.type = SECOND, .len = 30, .size = 1, .style = 0b11001100, .angle = PI/2, .position = {97, 31, 1}};

void updateTime()
{
    time.millisecond = (Timer_Millis()%1000 + ori_time.millisecond)%1000;
    time.second = (Timer_Seconds()%60 + ori_time.second)%60;
    time.minute = ((Timer_Seconds()/60)%60 + ori_time.minute)%60;
    time.hour = ((Timer_Seconds()/3600)%24 + ori_time.hour)%24;
}

void setHandAngle(struct Hands *hand, const Time_t *time)
{
    rad_angle tmp_angle = PI/2;
    switch(hand->type)
    {
        case HOUR:
            tmp_angle -= 2*PI*time->hour/12 + 2*PI*time->minute/720 + 2*PI*time->second/43200;
            break;
        case MINUTE:
            tmp_angle -= 2*PI*time->minute/60 + 2*PI*time->second/3600;
            break;
        case SECOND:
            tmp_angle -= 2*PI*time->second/60;
            break;
        default:
            break;
    }
    if(tmp_angle < -PI)
    {
        tmp_angle += 2*PI;
    }
    hand->angle = tmp_angle;
}

void setHandPosition(struct Hands *hand)
{
    hand->position.x = center.x + round(hand->len*cos(hand->angle));
    hand->position.y = center.y - round(hand->len*sin(hand->angle));
}

void app_main(void)
{
    Timer_Init(1000000);
    Timer_Start();
    Oled_Init(0x00);
    Oled_Clear(0x00);
    Oled_Draw_Circle(&center, radius, 2);
    Oled_Show();
    while(1)
    {
        Oled_Clear(0x00);
        Oled_Draw_Circle(&center, radius, 2);
        updateTime();
        setHandAngle(&hour_hand, &time);
        setHandAngle(&minute_hand, &time);
        setHandAngle(&second_hand, &time);
        setHandPosition(&hour_hand);
        setHandPosition(&minute_hand);
        setHandPosition(&second_hand);
        Oled_Draw_Line(&center, &second_hand.position, second_hand.style);
        Oled_Draw_Line(&center, &minute_hand.position, minute_hand.style);
        Oled_Draw_Line(&center, &hour_hand.position, hour_hand.style);
        Oled_Show();
        vTaskDelay(pdMS_TO_TICKS(100));
    }

}
