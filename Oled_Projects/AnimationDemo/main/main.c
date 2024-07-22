#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "ssd1306.h"

#include "monkey.h"
/*
 You have to set this config value with menuconfig
 CONFIG_INTERFACE

 for i2c
 CONFIG_MODEL
 CONFIG_SDA_GPIO
 CONFIG_SCL_GPIO
 CONFIG_RESET_GPIO

 for SPI
 CONFIG_CS_GPIO
 CONFIG_DC_GPIO
 CONFIG_RESET_GPIO
*/

#define TAG "SSD1306"

// https://www.mischianti.org/2021/07/14/ssd1306-oled-display-draw-images-splash-and-animations-2/



void app_main(void)
{
	SSD1306_t mydisplay;

#if CONFIG_I2C_INTERFACE
	ESP_LOGI(TAG, "INTERFACE is i2c");
	ESP_LOGI(TAG, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
	ESP_LOGI(TAG, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
	ESP_LOGI(TAG, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
	i2c_master_init(&mydisplay, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
#endif // CONFIG_I2C_INTERFACE

#if CONFIG_SPI_INTERFACE
	ESP_LOGI(TAG, "INTERFACE is SPI");
	ESP_LOGI(TAG, "CONFIG_MOSI_GPIO=%d",CONFIG_MOSI_GPIO);
	ESP_LOGI(TAG, "CONFIG_SCLK_GPIO=%d",CONFIG_SCLK_GPIO);
	ESP_LOGI(TAG, "CONFIG_CS_GPIO=%d",CONFIG_CS_GPIO);
	ESP_LOGI(TAG, "CONFIG_DC_GPIO=%d",CONFIG_DC_GPIO);
	ESP_LOGI(TAG, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
	spi_master_init(&mydisplay, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO);
#endif // CONFIG_SPI_INTERFACE

#if 0
#if CONFIG_FLIP
	mydisplay._flip = true;
	ESP_LOGW(TAG, "Flip upside down");
#endif
#endif

#if CONFIG_SSD1306_128x64
	ESP_LOGI(TAG, "Panel is 128x64");
	ssd1306_init(&mydisplay, 128, 64);
#endif // CONFIG_SSD1306_128x64
#if CONFIG_SSD1306_128x32
	ESP_LOGE(TAG, "Panel is 128x32. This demo cannot be run.");
	while(1) { vTaskDelay(1); }
#endif // CONFIG_SSD1306_128x32

	ssd1306_contrast(&mydisplay, 0xff);
	ssd1306_clear_screen(&mydisplay, false);
	int count = 9;
	uint8_t tmp = 0b11111100;
	printf("ssd1306_rotate_byte(%#x)=%#x\n", tmp, ssd1306_rotate_byte(tmp));
	uint8_t segs[128];
	while(1) {
		TickType_t startTick = xTaskGetTickCount();
		// 1Ticks required
		//for (int page=0;page<8;page++) 
		for (int page=8;page>0;page--)
		{
			for (int seg=0;seg<128;seg++) {
				segs[seg] =  ssd1306_rotate_byte(monkeyAnimation[count][seg*8+page]);
				//segs[seg] =  (monkeyAnimation[count][seg*8+page]);
			}
			ssd1306_display_image(&mydisplay, page, 0, segs, 128);
			
		}
		TickType_t endTick = xTaskGetTickCount();
		ESP_LOGD(TAG, "diffTick=%"PRIu32, endTick - startTick);
		count--;
		if (count<0) count = 9;
		//vTaskDelay(5000 / portTICK_PERIOD_MS);
		vTaskDelay(4);
	}
	vTaskDelay(2000 / portTICK_PERIOD_MS);
}
