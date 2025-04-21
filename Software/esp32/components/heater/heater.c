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
    ledc_stop(LEDC_LOW_SPEED_MODE, HEATER_ChANNEL, 0);  // 停止加热
}
void heater_stop(void){

    ledc_stop(LEDC_LOW_SPEED_MODE, HEATER_ChANNEL, 0);  // 停止加热
}

 void heater_output(uint16_t pwm)
{
    
    
    ledc_set_duty(LEDC_LOW_SPEED_MODE, HEATER_ChANNEL, pwm);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, HEATER_ChANNEL);

}

static void pid_init(pid_controller_t *pid, float Kp, float Ki, float Kd, float output_min, float output_max,uint32_t Sample_time )
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->output_min = output_min;
    pid->output_max = output_max;
    pid->Sample_time = 100; // 采样时间为100ms
}

void pid_set_param(pid_controller_t *pid, float Kp, float Ki, float Kd)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
}

void pid_set_sample_time(pid_controller_t *pid, uint32_t Sample_time)
{

    pid->Sample_time = Sample_time;
}


void pid_set_output_limits(pid_controller_t *pid, float output_min, float output_max)
{
    pid->output_min = output_min;
    pid->output_max = output_max;
}

uint16_t pid_calculate(pid_controller_t *pid, float setpoint, float measured_value)
{
    // 计算误差
    float error = setpoint - measured_value;
    // 计算积分项
   
    float SampleTimeInSeconds = pid->Sample_time / 1000.0; // Convert milliseconds to seconds

    // 计算积分项
    pid->integral += error * SampleTimeInSeconds; 
    // 限幅处理
    if (pid->integral > pid->output_max) {
        pid->integral = pid->output_max;
    } else if (pid->integral < pid->output_min) {
        pid->integral = pid->output_min;
    }

    // 计算微分项
    float derivative = (error - pid->prev_error) / SampleTimeInSeconds;
    // 计算输出值
    float output = (pid->Kp * error) + (pid->Ki * pid->integral) + (pid->Kd * derivative);
    // 限幅处理
    if (output > pid->output_max) {
        output = pid->output_max;
    } else if (output < pid->output_min) {
        output = pid->output_min;
    }
    // 更新上一次误差
    pid->prev_error = error;
    
    return (uint16_t)(output);


}