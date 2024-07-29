//include c standard library
#include <stdint.h>

//include esp internal library
#include "sdkconfig.h"
#include "driver/pulse_cnt.h"
#include "esp_log.h"

//include my header files
#include "encoder.h"

static ENCODER_STATE Encoder_State = ENCODER_DISABLED;

static int encoder_count = 0;
static const char *TAG = "Encoder";
static pcnt_unit_handle_t hpcnt = 0;

void Encoder_Init()
{
    if(Encoder_State != ENCODER_DISABLED)
    {
        ESP_LOGW(TAG, "already enabled");
        return;
    }
    //new pcnt handler
    pcnt_unit_config_t pcnt_config = 
    {
        .high_limit = ENCODER_CNT_MAX,
        .low_limit = ENCODER_CNT_MIN,
    };
    ESP_ERROR_CHECK(pcnt_new_unit(&pcnt_config, &hpcnt));

    //set filter
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = ENCODER_GLITCH_TIME,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(hpcnt, &filter_config));

    //new channel a
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = ENCODER_PIN_A,
        .level_gpio_num = ENCODER_PIN_B,
    };
    pcnt_channel_handle_t pcnt_chan_a = 0;
    ESP_ERROR_CHECK(pcnt_new_channel(hpcnt, &chan_a_config, &pcnt_chan_a));

    //new channel b
    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = ENCODER_PIN_B,
        .level_gpio_num = ENCODER_PIN_A,
    };
    pcnt_channel_handle_t pcnt_chan_b = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(hpcnt, &chan_b_config, &pcnt_chan_b));

    //set channel behavior
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_HOLD));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_HOLD));
    
    ESP_ERROR_CHECK(pcnt_unit_enable(hpcnt));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(hpcnt));

    Encoder_State = ENCODER_STOPPED;
}

void Encoder_Start()
{
    if(Encoder_State == ENCODER_DISABLED)
    {
        ESP_LOGE(TAG, "cannot start when ENCODER_DISABLED");
        return;
    }
    if(Encoder_State == ENCODER_COUNTING)
    {
        ESP_LOGW(TAG, "already counting");
        ESP_ERROR_CHECK(pcnt_unit_stop(hpcnt));
    }
    ESP_ERROR_CHECK(pcnt_unit_clear_count(hpcnt));
    ESP_ERROR_CHECK(pcnt_unit_start(hpcnt));
    Encoder_State = ENCODER_COUNTING;
}

void Encoder_Pause()
{
    if(Encoder_State == ENCODER_DISABLED)
    {
        ESP_LOGE(TAG, "cannot start when ENCODER_DISABLED");
        return;
    }
    ESP_ERROR_CHECK(pcnt_unit_stop(hpcnt));
    Encoder_State = ENCODER_PAUSED;
}

void Encoder_Stop()
{
    if(Encoder_State == ENCODER_DISABLED)
    {
        ESP_LOGE(TAG, "cannot stop when ENCODER_DISABLED");
        return;
    }
    ESP_ERROR_CHECK(pcnt_unit_stop(hpcnt));
    pcnt_unit_clear_count(hpcnt);
    Encoder_State = ENCODER_STOPPED;
}

void Encoder_Delete()
{
    if(Encoder_State == ENCODER_DISABLED)
    {
        ESP_LOGE(TAG, "cannot delete when ENCODER_DISABLED");
        return;
    }
    ESP_ERROR_CHECK(pcnt_unit_disable(hpcnt));
    ESP_ERROR_CHECK(pcnt_del_unit(hpcnt));
    Encoder_State = ENCODER_DISABLED;
}

int16_t Encoder_getCount()
{
    if(Encoder_State == ENCODER_DISABLED)
    {
        ESP_LOGE(TAG, "cannot get count when ENCODER_DISABLED");
        return 0;
    }
    pcnt_unit_get_count(hpcnt, &encoder_count);
    return encoder_count;
}