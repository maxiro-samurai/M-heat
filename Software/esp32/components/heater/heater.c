#include "heater.h"


void heater_init(void)
{
 // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t heater_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = HEATER_TIMER,
        .duty_resolution  = LEDC_TIMER_13_BIT,
        .freq_hz          = HEATER_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&heater_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t heater_channel = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = HEATER_ChANNEL,
        .timer_sel      = HEATER_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = HEATER_OUTPUT_IO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&heater_channel));
    ledc_stop(LEDC_LOW_SPEED_MODE, HEATER_ChANNEL, 0);  // 停止发声
}


static void heater_output(uint8_t pwm)
{
    
    ledcWrite(LEDC_LOW_SPEED_MODE, HEATER_ChANNEL, pwm); // 设置PWM输出值

}

static void pid_init(pid_controller_t *pid, float Kp, float Ki, float Kd, float output_min, float output_max)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->output_min = output_min;
    pid->output_max = output_max;
}


