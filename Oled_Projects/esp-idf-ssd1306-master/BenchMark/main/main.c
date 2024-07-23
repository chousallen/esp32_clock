#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "rom/ets_sys.h" // ets_get_cpu_frequency()

#include "monkey.h"
#include "ssd1306.h"

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
	SSD1306_t dev;

#if CONFIG_I2C_INTERFACE
	ESP_LOGI(TAG, "INTERFACE is i2c");
	ESP_LOGI(TAG, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
	ESP_LOGI(TAG, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
	ESP_LOGI(TAG, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
#endif // CONFIG_I2C_INTERFACE

#if CONFIG_SPI_INTERFACE
	ESP_LOGI(TAG, "INTERFACE is SPI");
	ESP_LOGI(TAG, "CONFIG_MOSI_GPIO=%d",CONFIG_MOSI_GPIO);
	ESP_LOGI(TAG, "CONFIG_SCLK_GPIO=%d",CONFIG_SCLK_GPIO);
	ESP_LOGI(TAG, "CONFIG_CS_GPIO=%d",CONFIG_CS_GPIO);
	ESP_LOGI(TAG, "CONFIG_DC_GPIO=%d",CONFIG_DC_GPIO);
	ESP_LOGI(TAG, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
	//int speed = 1000000; // 1MHz
	//int speed = 2000000; // 2MHz
	//int speed = 4000000; // 4MHz
	//int speed = 6000000; // 6MHz
	//int speed = 8000000; // 8MHz
	int speed = 10000000; // 10MHz
	spi_clock_speed(speed);
	spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO);
#endif // CONFIG_SPI_INTERFACE

#if CONFIG_SSD1306_128x64
	ESP_LOGI(TAG, "Panel is 128x64");
	ssd1306_init(&dev, 128, 64);
#endif // CONFIG_SSD1306_128x64
#if CONFIG_SSD1306_128x32
	ESP_LOGE(TAG, "Panel is 128x32. This demo cannot be run.");
	while(1) { vTaskDelay(1); }
#endif // CONFIG_SSD1306_128x32

	printf("SSD1306 OLED Display\n");
	ssd1306_init(&dev, 128, 64);
	printf("end init\n");
	ssd1306_contrast(&dev, 0xff);
	ssd1306_clear_screen(&dev, false);
	uint8_t segs[128];
	int64_t startTime = esp_timer_get_time(); // Get time in microseconds since boot
	ESP_LOGI(TAG, "startTime=%lld us", startTime);
	int frameCount = 0;
	for (int loop=0;loop<10;loop++) {
		for (int count=9;count>=0;count--) {
			for (int page=0;page<8;page++) {
				for (int seg=0;seg<128;seg++) {
					segs[seg] =  ssd1306_rotate_byte(monkeyAnimation[count][seg*8+page]);
				}
				ssd1306_display_image(&dev, page, 0, segs, 128);
			}
			frameCount++;
		}
	} // end for
	int64_t endTime = esp_timer_get_time(); // Get time in microseconds since boot
	ESP_LOGI(TAG, "endTime=%lld us", endTime);
	int32_t diffTime = endTime - startTime;
	ESP_LOGI(TAG, "diffTime=%"PRIi32" us", diffTime);
	float frameRate = (float)diffTime / (float)frameCount;
	frameRate = frameRate / 1000000.0;
	frameRate = 1.0 / frameRate;

#ifdef CONFIG_IDF_TARGET_ESP32
	ESP_LOGI(TAG, "Core is ESP32@%"PRIu32"Mhz", ets_get_cpu_frequency());
#elif defined CONFIG_IDF_TARGET_ESP32S2
	ESP_LOGI(TAG, "Core is ESP32S2@%"PRIu32"Mhz", ets_get_cpu_frequency());
#elif defined CONFIG_IDF_TARGET_ESP32S3
	ESP_LOGI(TAG, "Core is ESP32S3@%"PRIu32"Mhz", ets_get_cpu_frequency());
#elif defined CONFIG_IDF_TARGET_ESP32C2
	ESP_LOGI(TAG, "Core is ESP32C2@%"PRIu32"Mhz", ets_get_cpu_frequency());
#elif defined CONFIG_IDF_TARGET_ESP32C3
	ESP_LOGI(TAG, "Core is ESP32C3@%"PRIu32"Mhz", ets_get_cpu_frequency());
#elif defined CONFIG_IDF_TARGET_ESP32C6
	ESP_LOGI(TAG, "Core is ESP32C6@%"PRIu32"Mhz", ets_get_cpu_frequency());
#elif defined CONFIG_IDF_TARGET_ESP32H2
	ESP_LOGI(TAG, "Core is ESP32H2@%"PRIu32"Mhz", ets_get_cpu_frequency());
#endif
	ESP_LOGI(TAG, "diffTime=%"PRIi32" us. Frame Rate=%4.1f fps", diffTime, frameRate);

	while(1) {
		for (int count=9;count>=0;count--) {
			for (int page=0;page<8;page++) {
				for (int seg=0;seg<128;seg++) {
					segs[seg] =  ssd1306_rotate_byte(monkeyAnimation[count][seg*8+page]);
				}
				ssd1306_display_image(&dev, page, 0, segs, 128);
			}
		}
	} // end while
}
