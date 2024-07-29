#ifndef _DIGITAL_CLOCK_H_
#define _DIGITAL_CLOCK_H_

#include "oled.h"
#include "timer.h"

enum
{
    DIGITAL_SET_SECOND,
    DIGITAL_SET_MINUTE,
    DIGITAL_SET_HOUR
};

Pixel_t fisrt_pos = {4, (OLED_HEIGHT-BITMAP_NUMBERS_HEIGHT)/2, 1}, curr_pos;
uint8_t colon_y_pos = (OLED_HEIGHT-BITMAP_COLON_HEIGHT)/2;
int8_t offset = -5, small_offset = -2, space_for_colon = 2;

void displayDigitalClock(Time_t* time);
void drawDigitalUnderline(uint8_t set_type);

void displayDigitalClock(Time_t* time)
{
    Pixel_t curr_pos = fisrt_pos;
    Oled_Draw_Bitmap(bitmap_numbers[time->hour/10%10], curr_pos, BITMAP_NUMBERS_WIDTH, BITMAP_NUMBERS_HEIGHT);
    curr_pos.x += BITMAP_NUMBERS_WIDTH + offset;
    Oled_Draw_Bitmap(bitmap_numbers[time->hour%10], curr_pos, BITMAP_NUMBERS_WIDTH, BITMAP_NUMBERS_HEIGHT);
    curr_pos.x += BITMAP_NUMBERS_WIDTH + offset + space_for_colon;
    Oled_Draw_Bitmap(bitmap_colon, (Pixel_t){curr_pos.x, colon_y_pos, 1}, BITMAP_COLON_WIDTH, BITMAP_COLON_HEIGHT);
    curr_pos.x += BITMAP_COLON_WIDTH + offset + space_for_colon;
    Oled_Draw_Bitmap(bitmap_numbers[time->minute/10%10], curr_pos, BITMAP_NUMBERS_WIDTH, BITMAP_NUMBERS_HEIGHT);
    curr_pos.x += BITMAP_NUMBERS_WIDTH + offset;
    Oled_Draw_Bitmap(bitmap_numbers[time->minute%10], curr_pos, BITMAP_NUMBERS_WIDTH, BITMAP_NUMBERS_HEIGHT);
    curr_pos.x += BITMAP_NUMBERS_WIDTH + offset;
    curr_pos.y += BITMAP_NUMBERS_HEIGHT/2;
    Oled_Draw_Bitmap(bitmap_small[time->second/10%10], curr_pos, BITMAP_SMALL_NUMBERS_WIDTH, BITMAP_SMALL_NUMBERS_HEIGHT);
    curr_pos.x += BITMAP_SMALL_NUMBERS_WIDTH + small_offset;
    Oled_Draw_Bitmap(bitmap_small[time->second%10], curr_pos, BITMAP_SMALL_NUMBERS_WIDTH, BITMAP_SMALL_NUMBERS_HEIGHT);
    Oled_Show();
    Oled_Clear(0x00);
}

void drawDigitalUnderline(uint8_t set_type)
{
    static Pixel_t tmp = {0, 0, 1};
    static uint8_t style = 0xff;
    style = ~style;
    switch(set_type)
    {
        case DIGITAL_SET_SECOND:
            tmp.x = fisrt_pos.x + 4*BITMAP_NUMBERS_WIDTH + BITMAP_COLON_WIDTH + 5*offset + 2*space_for_colon;
            tmp.y = fisrt_pos.y + BITMAP_NUMBERS_HEIGHT;
            Oled_Draw_Line(&tmp, &((Pixel_t){tmp.x + 2*BITMAP_SMALL_NUMBERS_WIDTH + 3*small_offset, tmp.y, 1}), style);
            break;
        case DIGITAL_SET_MINUTE:
            tmp.x = fisrt_pos.x + 2*BITMAP_NUMBERS_WIDTH + BITMAP_COLON_WIDTH + 3*offset + 2*space_for_colon;
            tmp.y = fisrt_pos.y + BITMAP_NUMBERS_HEIGHT;
            Oled_Draw_Line(&tmp, &((Pixel_t){tmp.x + 2*BITMAP_NUMBERS_WIDTH + 3*offset, tmp.y, 1}), style);
            break;
        case DIGITAL_SET_HOUR:
            tmp.x = fisrt_pos.x;
            tmp.y = fisrt_pos.y + BITMAP_NUMBERS_HEIGHT;
            Oled_Draw_Line(&tmp, &((Pixel_t){tmp.x + 2*BITMAP_NUMBERS_WIDTH + 3*offset, tmp.y, 1}), style);
            break;
        default:
            break;
    }
}

#endif //_DIGITAL_CLOCK_H_