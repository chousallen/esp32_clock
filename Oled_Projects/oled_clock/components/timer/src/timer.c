
#include <stdbool.h>
#include <stdio.h>

#include "driver/gptimer.h"
#include "driver/gptimer_types.h"

#include "timer.h"

gptimer_handle_t htim;
TIMER_STATE TIM_STATE = UNABLED;

TIMER_STATE Timer_getState()
{
    return TIM_STATE;
}

bool Timer_Init(uint32_t freq)
{
    if(TIM_STATE != UNABLED)
    {
        printf("Warning\tclock.h\tTimer_Init:\talready enabled\n");
        return 0;
    }
    freq = freq==0 ? 1000000 : freq;
    gptimer_config_t timconfig = 
    {
        .clk_src = GPTIMER_CLK_SRC_APB,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = freq
    };
    esp_err_t err = 0;
    err = gptimer_new_timer(&timconfig, &htim);
    err += gptimer_enable(htim);
    ESP_ERROR_CHECK(err);
    if(err != ESP_OK)
    {
        TIM_STATE = UNABLED;
        printf("Error\tclock.h\tTimer_Init:\tfail to init timer\n");
        return 0;
    }
    
    TIM_STATE = STOPPED;
    return 1;
}

bool Timer_Start()
{
    switch(TIM_STATE)
    {
        case UNABLED:
            printf("Warning\tclock.h\tTimer_Start:\ttimer has not been enabled\n");
            printf("Init timer in default freq\n");
            Timer_Init(0);
            break;
        case STOPPED:
            break;
        case RUNNING:
            printf("Warning\tclock.h\tTimer_Start:\ttimer is running\n");
            printf("Restart timer\n");
            Timer_Stop();
            break;
        case PAUSED:
            printf("Warning\tclock.h\tTimer_Start:\ttimer is paused\n");
            printf("Restart timer\n");
            Timer_Stop();
            break;
        default:
            printf("Error\tclock.h\tTimer_Start:\tUnknown state\n");
            return 0;
            break;
    }
    esp_err_t err = gptimer_set_raw_count(htim, 0);
    err += gptimer_start(htim);
    ESP_ERROR_CHECK(err);
    if(err != ESP_OK)
    {
        printf("Error\tclock.h\tTimer_Start:\tfail to start timer\n");
        return 0;
    }
    TIM_STATE = RUNNING;
    return 1;
} 

bool Timer_Pause()
{
    switch(TIM_STATE)
    {
        case UNABLED:
            printf("Error\tclock.h\tTimer_Pause:\ttimer has not been enabled\n");
            return 0;
            break;
        case STOPPED:
            printf("Warning\tclock.h\tTimer_Pause:\ttimer is already stopped\n");
            return 1;
            break;
        case RUNNING:
            break;
        case PAUSED:
            printf("Warning\tclock.h\tTimer_Pause:\ttimer is already paused\n");
            return 1;
            break;
        default:
            printf("Error\tclock.h\tTimer_Pause:\tUnknown state\n");
            return 0;
            break;
    }
    esp_err_t err = gptimer_stop(htim);
    if(err != ESP_OK)
    {
        printf("Error\tclock.h\tTimer_Pause:\tfail to stop timer\n");
        return 0;
    }
    return 1;
}

bool Timer_Resume()
{
    switch(TIM_STATE)
    {
        case UNABLED:
            printf("Error\tclock.h\tTimer_Resume:\ttimer has not been enabled\n");
            return 0;
            break;
        case STOPPED:
            printf("Error\tclock.h\tTimer_Resume:\ttimer is stopped, cannot resume\n");
            return 0;
            break;
        case RUNNING:
            printf("Error\tclock.h\tTimer_Resume:\ttimer is running, cannot resume\n");
            return 0;
            break;
        case PAUSED:
            break;
        default:
            printf("Error\tclock.h\tTimer_Resume:\tUnknown state\n");
            return 0;
            break;
    }
    esp_err_t err = gptimer_start(htim);
    if(err != ESP_OK)
    {
        printf("Error\tclock.h\tTimer_Resume:\tfail to start timer\n");
        return 0;
    }
    TIM_STATE = RUNNING;
    return 1;    
}

bool Timer_Stop()
{
    switch(TIM_STATE)
    {
        case UNABLED:
            printf("Error\tclock.h\tTimer_Stop:\ttimer has not been enabled\n");
            return 0;
            break;
        case STOPPED:
            printf("Warning\tclock.h\tTimer_Stop:\ttimer is already stopped\n");
            return 1;
            break;
        case RUNNING:
            esp_err_t err = gptimer_stop(htim);
            if(err != ESP_OK)
            {
                printf("Error\tclock.h\tTimer_Stop:\tfail to stop timer from running state\n");
                return 0;
            }
            TIM_STATE = PAUSED;
            break;
        case PAUSED:
            break;
        default:
            printf("Error\tclock.h\tTimer_Stop:\tUnknown state\n");
            return 0;
            break;
    }
    esp_err_t err = gptimer_set_raw_count(htim, 0);
    if(err != ESP_OK)
    {
        printf("Error\tclock.h\tTimer_Stop:\tfail to set timer's count\n");
        return 0;
    }
    TIM_STATE = STOPPED;
    return 1;
}

bool Timer_Delete()
{
    switch(TIM_STATE)
    {
        case UNABLED:
            printf("Warning\tclock.h\tTimer_Delete:\ttimer has not been enabled\n");
            return 1;
            break;
        case STOPPED:
            break;
        case RUNNING:
            printf("Warning\tclock.h\tTImer_Delete:\ttimer is still running\n");
            esp_err_t err = gptimer_stop(htim);
            if(err != ESP_OK)
            {
                printf("Error\tclock.h\tTimer_Delete:\tfail to stop timer from running state\n");
                return 0;
            }
            TIM_STATE = PAUSED;
            break;
        case PAUSED:
            break;
        default:
            printf("Error\tclock.h\tTimer_Delete:\tUnknown state\n");
            return 0;
            break;
    }
    esp_err_t err = gptimer_disable(htim) + gptimer_del_timer(htim);
    if(err != ESP_OK)
    {
        printf("Error\tclock.h\tTimer_Delete:\tfail to delete timer\n");
        return 0;
    }
    TIM_STATE = UNABLED;
    return 1;
}

uint64_t Timer_getCount()
{
    if(TIM_STATE == UNABLED)
    {
        printf("ERROR\tclock.h\tTimer_getCount:\ttimer is not enabled\n");
        return 0;
    }
    uint64_t tmp;
    esp_err_t err = gptimer_get_raw_count(htim, &tmp);
    if(err != ESP_OK)
    {
        printf("ERROR\tclock.h\tTimer_getCount:\tfail to get timer's count\n");
        return 0;
    }
    return tmp;
}

uint64_t Timer_Micros()
{
    if(TIM_STATE == UNABLED)
    {
        printf("ERROR\tclock.h\tTimer_Micros:\ttimer is not enabled\n");
        return 0;
    }
    uint32_t freq;
    gptimer_get_resolution(htim, &freq);
    uint64_t tmp = (1000000.0 * (double)Timer_getCount()) / freq;
    return tmp;
}

uint64_t Timer_Millis()
{
    if(TIM_STATE == UNABLED)
    {
        printf("ERROR\tclock.h\tTimer_Millis:\ttimer is not enabled\n");
        return 0;
    }
    uint32_t freq;
    gptimer_get_resolution(htim, &freq);
    uint64_t tmp = (1000.0 * (double)Timer_getCount()) / freq;
    return tmp;
}

uint64_t Timer_Seconds()
{
    if(TIM_STATE == UNABLED)
    {
        printf("ERROR\tclock.h\tTimer_Seconds:\ttimer is not enabled\n");
        return 0;
    }
    uint32_t freq;
    gptimer_get_resolution(htim, &freq);
    uint64_t tmp = (1.0 * (double)Timer_getCount()) / freq;
    return tmp;
}

void Timer_printState()
{
    switch(TIM_STATE)
    {
        case UNABLED:
            printf("TiM_STATE:\tUNABLED\n");
            return ;
            break;
        case STOPPED:
            printf("TIM_STATE:\tSTOPPED\n");
            return ;
            break;
        case RUNNING:
            printf("TIM_STATE:\tRUNNING\n");
            return ;
            break;
        case PAUSED:
            printf("TIM_STATE:\tPAUSED\n");
            return ;
            break;
        default:
            printf("TIM_STATE:\tError\tUknown state\n");
            return ;
            break;
    }
    
}