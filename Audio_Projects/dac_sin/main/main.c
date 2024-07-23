/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <inttypes.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/dac_continuous.h"
#include "esp_adc/adc_continuous.h"
#include "esp_check.h"
#include "math.h"

static const char *TAG = "dac_audio";
dac_continuous_handle_t dac_handle;

#define CUSTOM_SAMPLE_RATE  44100

static void dac_write_data_synchronously(dac_continuous_handle_t handle, uint8_t *data, size_t data_size)
{
    ESP_LOGI(TAG, "Audio size %d bytes, played at frequency %d Hz synchronously", data_size, CUSTOM_SAMPLE_RATE);
    uint32_t cnt = 1;
    
        printf("Play count: %"PRIu32"\n", cnt++);
        ESP_ERROR_CHECK(dac_continuous_write(handle, data, data_size, NULL, -1));
        vTaskDelay(pdMS_TO_TICKS(1000));
}

void dac_sin(double freq)
{
    uint32_t len = CUSTOM_SAMPLE_RATE/freq;
    uint8_t *data = calloc(len, sizeof(uint8_t));
    printf("len: %ld\n", len);
    for(int i = 0; i < len; i++)
    {
        printf("value: %f\n", (127.5 * (cos(2 * 3.14159265 * i / CUSTOM_SAMPLE_RATE * freq))));
        data[i] = (uint8_t)(127.5 * (cos(2 * 3.14159265 * i / CUSTOM_SAMPLE_RATE * freq) + 1));
    }

    ESP_ERROR_CHECK(dac_continuous_write(dac_handle, data, len, NULL, -1));
    free(data);
    //ESP_ERROR_CHECK(dac_continuous_write_cyclically(dac_handle, data, CUSTOM_SAMPLE_RATE/freq, NULL));
    
}

void dac_const(uint8_t val)
{
    uint32_t len = 1;
    uint8_t *data = calloc(len, sizeof(uint8_t));
    printf("len: %ld\n", len);
    memset(data, val, len);

    ESP_ERROR_CHECK(dac_continuous_write(dac_handle, data, len, NULL, 100));
    free(data);

    //ESP_ERROR_CHECK(dac_continuous_write_cyclically(dac_handle, data, CUSTOM_SAMPLE_RATE/freq, NULL));
    
}


void app_main(void)
{
    ESP_LOGI(TAG, "DAC audio example start");
    ESP_LOGI(TAG, "--------------------------------------");

    dac_continuous_config_t cont_cfg = {
        .chan_mask = DAC_CHANNEL_MASK_ALL,
        .desc_num = 4,
        .buf_size = 2048,
        .freq_hz = CUSTOM_SAMPLE_RATE,
        .offset = 0,
        .clk_src = DAC_DIGI_CLK_SRC_APLL,   // Using APLL as clock source to get a wider frequency range
        /* Assume the data in buffer is 'A B C D E F'
         * DAC_CHANNEL_MODE_SIMUL:
         *      - channel 0: A B C D E F
         *      - channel 1: A B C D E F
         * DAC_CHANNEL_MODE_ALTER:
         *      - channel 0: A C E
         *      - channel 1: B D F
         */
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
    };
    /* Allocate continuous channels */
    ESP_ERROR_CHECK(dac_continuous_new_channels(&cont_cfg, &dac_handle));

    /* Enable the continuous channels */
    ESP_ERROR_CHECK(dac_continuous_enable(dac_handle));
    ESP_LOGI(TAG, "DAC initialized success, DAC DMA is ready");
    vTaskDelay(1500/portTICK_PERIOD_MS);

    dac_const(255);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    dac_const(0);
    //xTaskCreate(dac_sin, "dac_sin", 2048, (void *)1, 5, NULL);
}
