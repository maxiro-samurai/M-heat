#ifndef timer_h
#define timer_h

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "esp_log.h"
typedef struct {
    int16_t temp_plot_count; 
    int16_t pid_sample_count;
    uint64_t alarm_value; // 定时器触发值
} event_sign_t;

void timer_set_alarm(gptimer_handle_t gptimer, uint64_t alarm_count);
void timer_init(gptimer_handle_t *re_gptimer, QueueHandle_t queue);
void timer_start(gptimer_handle_t gptimer);
#endif // timer_h