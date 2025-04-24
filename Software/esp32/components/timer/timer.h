#ifndef timer_h
#define timer_h

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "esp_log.h"


void timer_set_alarm(gptimer_handle_t gptimer, uint32_t alarm_count);
void timer_init(gptimer_handle_t *re_gptimer, QueueHandle_t queue);
void timer_start(gptimer_handle_t gptimer);
#endif // timer_h