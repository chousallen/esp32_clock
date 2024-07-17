#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "ssd1306.h"
//#include ""

#include "vector.h"
#include "clock.h"

#define I2C_MASTER_SCL_IO 22        /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 21        /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_1    /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 100000   /*!< I2C master clock frequency */

#define BUTTON_GPIO 0
#define CLOCK_CENTER_X 90
#define CLOCK_CENTER_Y 32
#define CLOCK_RADIUS 30
#define CLOCK_THIKNESS 0.9
#define SECOND_HAND_LEN 28
#define MINUTE_HAND_LEN 23
#define HOUR_HAND_LEN 17

static ssd1306_handle_t ssd1306_dev = NULL;

uint8_t button = 0;

uint8_t hours, minutes, seconds;

void test()
{
    vTaskDelete(NULL);
}

void drawClock()
{
    /*
    for(int x=CLOCK_CENTER_X-CLOCK_RADIUS; x<CLOCK_CENTER_X+CLOCK_RADIUS; x++)
    {
        ssd1306_fill_point(ssd1306_dev, x, CLOCK_CENTER_Y + sqrt(CLOCK_RADIUS*CLOCK_RADIUS-(x-CLOCK_CENTER_X)*(x-CLOCK_CENTER_X)), 0xff);
        ssd1306_fill_point(ssd1306_dev, x, CLOCK_CENTER_Y - sqrt(CLOCK_RADIUS*CLOCK_RADIUS-(x-CLOCK_CENTER_X)*(x-CLOCK_CENTER_X)), 0xff);
    }
    */
   while(1)
   {
    for(int x=CLOCK_CENTER_X-CLOCK_RADIUS; x<CLOCK_CENTER_X+CLOCK_RADIUS; x++)
    {
        for(int y=CLOCK_CENTER_Y-CLOCK_RADIUS; y<CLOCK_CENTER_Y+CLOCK_RADIUS; y++)
        {
            if((CLOCK_RADIUS-CLOCK_THIKNESS)*(CLOCK_RADIUS-CLOCK_THIKNESS) <= (x-CLOCK_CENTER_X)*(x-CLOCK_CENTER_X)+(y-CLOCK_CENTER_Y)*(y-CLOCK_CENTER_Y) 
            && (x-CLOCK_CENTER_X)*(x-CLOCK_CENTER_X)+(y-CLOCK_CENTER_Y)*(y-CLOCK_CENTER_Y) <= CLOCK_RADIUS*CLOCK_RADIUS)
            {
                ssd1306_fill_point(ssd1306_dev, x, y, 0xff);
            }
			else
				ssd1306_fill_point(ssd1306_dev, x, y, 0x00);
			
        }
    }
	   double second_angle = 90 - seconds*6;
	   double minute_angle = 90 - minutes*6 - seconds/10;
	   double hour_angle = 90 - hours*30 - minutes/2;
	   int second_x = CLOCK_CENTER_X + SECOND_HAND_LEN*cos(second_angle*3.14159265/180);
	   int second_y = CLOCK_CENTER_Y - SECOND_HAND_LEN*sin(second_angle*3.14159265/180);
	   int minute_x = CLOCK_CENTER_X + MINUTE_HAND_LEN*cos(minute_angle*3.14159265/180);
	   int minute_y = CLOCK_CENTER_Y - MINUTE_HAND_LEN*sin(minute_angle*3.14159265/180);
	   int hour_x = CLOCK_CENTER_X + HOUR_HAND_LEN*cos(hour_angle*3.14159265/180);
	   int hour_y = CLOCK_CENTER_Y - HOUR_HAND_LEN*sin(hour_angle*3.14159265/180);
	   ssd1306_draw_line(ssd1306_dev, CLOCK_CENTER_X, CLOCK_CENTER_Y, second_x, second_y);
	   ssd1306_draw_line(ssd1306_dev, CLOCK_CENTER_X, CLOCK_CENTER_Y, minute_x, minute_y);
	   ssd1306_draw_line(ssd1306_dev, CLOCK_CENTER_X, CLOCK_CENTER_Y, hour_x, hour_y);
		vTaskDelay(500/portTICK_PERIOD_MS);
   }
   vTaskDelete(NULL);
}

void updateDisplay()
{
    while (1)
    {
        ssd1306_refresh_gram(ssd1306_dev);
        vTaskDelay(4);
    }
    
}

void detectButton()
{
    printf("Button detection task started\n");
    
    gpio_config_t gpio_conf = 
    {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .pull_down_en = 0,
        .pull_up_en = 1
    };
    gpio_config(&gpio_conf);
    

    while (1)
    {
        button += !gpio_get_level(BUTTON_GPIO);
        if(!gpio_get_level(BUTTON_GPIO))
            printf("Button pressed, count: %d\n", button);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void changeDisplay()
{
    while (1)
    {
        ssd1306_clear_screen(ssd1306_dev, (button%8)*0x1f);
        vTaskDelay(10/ portTICK_PERIOD_MS);
    }
    
}

void countClock()
{
    hours = 0;
    minutes = 0;
    seconds = 0;
    while(1)
    {
        seconds ++;
        if(seconds == 60)
        {
            seconds = 0;
            minutes ++;
            if(minutes == 60)
            {
                minutes = 0;
                hours ++;
                if(hours == 24)
                {
                    hours = 0;
                }
            }
        }
		printf("Time: %d:%d:%d\n", hours, minutes, seconds);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;

    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);

    ssd1306_dev = ssd1306_create(I2C_MASTER_NUM, SSD1306_I2C_ADDRESS);
    ssd1306_clear_screen(ssd1306_dev, 0x00);

    xTaskCreate(drawClock, "drawClock", 1024 * 2, NULL, 1, NULL);
    xTaskCreate(updateDisplay, "updateDisplay", 1024 * 2, NULL, 3, NULL);
    //xTaskCreate(detectButton, "detectButton", 1024 * 2, NULL, 5, NULL);
    //xTaskCreate(changeDisplay, "changeDisplay", 1024 * 2, NULL, 6, NULL);
    xTaskCreate(test, "test", 1024 * 2, NULL, 1, NULL);
	xTaskCreate(countClock, "countClock", 1024 * 2, NULL, 1, NULL);
    

}
