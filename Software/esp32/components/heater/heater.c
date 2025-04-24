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

/** 
    * @brief 初始化串口
    * @param pwm 占空比值，范围0-8191  (13位分辨率)
    * @note 该函数用于设置加热器的PWM输出值，范围为0-8191（13位分辨率）。
    */
 void heater_output(uint16_t pwm)
{
    
    
    ledc_set_duty(LEDC_LOW_SPEED_MODE, HEATER_ChANNEL, pwm);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, HEATER_ChANNEL);

}

/** 
    * @brief PID控制器初始化函数
    * @param pid PID控制器结构体指针
    * @param Kp 比例系数
    * @param Ki 积分系数    
    * @param Kd 微分系数
    * @param output_min 输出最小值（限幅）
    * @param output_max 输出最大值（限幅）
    * @param Sample_time 采样时间（ms）
    * @note 该函数用于初始化PID控制器的参数。
    */
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

/**
 * * @brief 设置PID参数
 * * @param pid PID控制器结构体指针
 * * @param Kp 比例系数
 * * @param Ki 积分系数
 * * @param Kd 微分系数
 * * @note 该函数用于设置PID控制器的参数。
 */
void pid_set_param(pid_controller_t *pid, float Kp, float Ki, float Kd)
{

    if(pid->Kp != Kp){
        pid->Kp = Kp;
    }else if (pid->Ki != Ki) {
        pid->Ki = Ki;
    }else if ( pid->Ki != Ki) {
    
        pid->Ki = Ki;
        
    }
    
    
}
/** 
 *  @brief 设置采样时间
 *  @param pid PID控制器结构体指针
 *  @param Sample_time 采样时间（ms）
 * *  @note 该函数用于设置PID控制器的采样时间。
 */
void pid_set_sample_time(pid_controller_t *pid, uint32_t Sample_time)
{
    pid->Sample_time = Sample_time;
}

/**
 * @brief 设置输出限幅
 * @param pid PID控制器结构体指针
 * @param output_min 输出最小值（限幅）
 * @param output_max 输出最大值（限幅）
 * @note 该函数用于设置PID控制器的输出限幅。
 */
void pid_set_output_limits(pid_controller_t *pid, float output_min, float output_max)
{
    pid->output_min = output_min;
    pid->output_max = output_max;
}
/**
 * @brief 计算PID控制器输出值
 * @param pid PID控制器结构体指针
 * @param setpoint 设定值
 * @param measured_value 测量值
 * @return PID控制器输出值
 * @note 该函数用于计算PID控制器的输出值。
 */
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
/** 
 * * @brief 设置目标值
 * * @param pid PID控制器结构体指针
 * * @param target_value 目标值
 * * @note 该函数用于设置PID控制器的目标值。
 */
void pid_set_target_value(pid_controller_t *pid, float target_value)
{
    pid->target_value = target_value;
}