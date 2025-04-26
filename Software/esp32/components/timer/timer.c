#include "timer.h"

/**
 * * @brief 初始化定时器
 * * @param re_gptimer 定时器句柄
 * * @param queue 队列句柄
*/
void timer_init(gptimer_handle_t *re_gptimer, QueueHandle_t queue)
{
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick=1us
    };

    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    // gptimer_event_callbacks_t cbs = {
    //     .on_alarm = timer_on_alarm_cb,
    // };

    // ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, queue));

    gptimer_alarm_config_t alarm_config = {
        .alarm_count =1000, // period = 1ms
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };

    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
    //返回定时器句柄
    *re_gptimer = gptimer;

}

/**
 * * @brief 设定定时器的触发值
 * * @param gptimer 定时器句柄
 * * @param alarm_count 定时器触发值
*/ 
void timer_set_alarm(gptimer_handle_t gptimer, uint64_t alarm_count)
{
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = alarm_count, // 分辨率为1MHz，100ms=100000
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };
    //先关闭再设置
    gptimer_stop(gptimer);
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}
