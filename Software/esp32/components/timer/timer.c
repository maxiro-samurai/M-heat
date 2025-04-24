#include "timer.h"
/**
 * * @brief 定时器中断回调函数
 * * * @param timer 定时器句柄
 * * * @param edata 定时器事件数据
 * * * @param user_data 用户数据(一般用作传递队列句柄)
 * * * @return true: 需要在ISR结束时切换上下文，false: 不需要
 */
static bool IRAM_ATTR timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)

{
    
    BaseType_t high_task_awoken = pdFALSE;
    QueueHandle_t queue = (QueueHandle_t)user_data;
    // 中断函数尽量不要写太多的代码，避免影响中断响应时间
    // 这里我们只是将定时器的触发值发送到队列中，用来计算定时器间隔
    xQueueSendFromISR(queue, &edata->alarm_value, &high_task_awoken);
    // return whether we need to yield at the end of ISR
    return (high_task_awoken == pdTRUE);
}

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

    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_on_alarm_cb,
    };

    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, queue));

    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 500000, // period = 100ms
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
void timer_set_alarm(gptimer_handle_t gptimer, uint32_t alarm_count)
{
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = alarm_count, // 分辨率为1MHz，100ms=100000
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
}

void timer_start(gptimer_handle_t gptimer)
{
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}