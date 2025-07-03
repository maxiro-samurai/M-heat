#include "heater.h"

uint8_t sample_time_list[3]={200,100,50};//采样时间列表
gptimer_handle_t gptimer = NULL;  
QueueHandle_t queue = NULL;
bool PWMOutput_Lock = false;

static const char *TAG = "PID";
bool PWM_state = true;

/**
 * * @brief 定时器中断回调函数
 * * * @param timer 定时器句柄
 * * * @param edata 定时器事件数据
 * * * @param user_data 用户数据(一般用作传递队列句柄)
 * * * @return true: 需要在ISR结束时切换上下文，false: 不需要
 */
static bool IRAM_ATTR timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)

{
    static event_sign_t event_sign ;
    event_sign_t *event_sign_ptr = &event_sign;
    BaseType_t high_task_awoken = pdFALSE;
    QueueHandle_t queue = (QueueHandle_t)user_data;
    event_sign.alarm_value = edata->alarm_value; // 获取定时器触发值
    event_sign.pid_sample_count++;
    // 中断函数尽量不要写太多的代码，避免影响中断响应时间
    // 这里我们只是将定时器的触发值发送到队列中，用来计算定时器间隔
    xQueueSendFromISR(queue, &event_sign_ptr, &high_task_awoken);
    // return whether we need to yield at the end of ISR
    return (high_task_awoken == pdTRUE);
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
    if(PWMOutput_Lock)
    pwm = 0;
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
    pid->Sample_time = Sample_time; // 单位为ms

    /*设置时间间隔,即设置报警计数器*/
    timer_set_alarm(gptimer,(uint64_t)Sample_time*1000);
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
    static uint32_t last_sample_time;

    pid->Sample_time = Sample_time;
    
    if (last_sample_time != Sample_time)
    timer_set_alarm(gptimer,(uint64_t)Sample_time*1000);


    last_sample_time = Sample_time; 

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

void heater_init(void)
{
    /*PWM驱动初始化*/
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


    /*初始化定时器，用于PID采样*/
    queue = xQueueCreate(5, sizeof(void*)); // 创建队列，用于接收定时器事件
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
        .alarm_count =1000, // period = 1ms
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };

    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}

bool en_pid= true;
uint16_t output_pwm;


// FireToolPIDAdaptor fireToolPidAdaptor;
// /**野火PID调试助手 */
// void fta_start_callback(unsigned char channel)
// {
//     printf("start command!,channel: %d\n", channel);
// }

// void fta_stop_callback(unsigned char channel)
// {
//     printf("stop command!channel: %d\n", channel);
// }

// void fta_reset_callback(unsigned char channel)
// {
//     printf("reset command!channel: %d\n", channel);
// }

// void fta_targetValue_callback(unsigned char channel, int32_t targetValue)
// {
//     printf("targetValue: %ld, channel: %d\n", targetValue, channel);
// }

// void fta_periodValue_callback(unsigned char channel, int32_t targetValue)
// {
//     printf("periodValue: %ld, channel: %d\n", targetValue, channel);
// }

// void fta_PID_callback(unsigned char channel, float P, float I, float D)
// {
//     printf("channel: %d, PID:%f, %f, %f\n", channel, P, I, D);
// }


pid_controller_t pid ;
heater_status_t heater_status = {0}; // 初始化加热器状态结构体
void Temperature_Control_task(void){
    /*初始化PID控制器，设置初始参数 */
    
    heater_init();
    pid_init(&pid, 100, 10, 30, 0, 8191, 200); // 初始化PID控制器
    float Temperature_gap ;
    void *pid_event;
    while (1)
    {   
        //定时器设置PID采样时间
        if (xQueueReceive(queue,&pid_event,portMAX_DELAY))
        {
            Temperature_gap = abs(ADC.now_temp-ADC.set_temp);

            //根据温差设置PID间隔 50ms~200ms
            if (Temperature_gap > 150) pid_set_sample_time(&pid,sample_time_list[0]);
            else if (Temperature_gap > 50)pid_set_sample_time(&pid,sample_time_list[1]);
            else pid_set_sample_time(&pid,sample_time_list[2]);
            
            //如果处于PID模式
            if (en_pid)
            {
                output_pwm =  pid_calculate(&pid,ADC.set_temp,ADC.now_temp);
                // ESP_LOGI(TAG, "PID输出值: %d", output_pwm);


                // 加热工作
                if (PWM_state && output_pwm>0 && heater_status.error_state == false)
                {
                    heater_output(output_pwm);
                    //首次记录加热时间
                    if (heater_status.first_work == false)
                    {
                        heater_status.heater_timer = xTaskGetTickCount() ;
                        heater_status.first_work = true;
                    }
                    uint32_t now_time = xTaskGetTickCount() ;
                    //在5分钟内
                    if (now_time - heater_status.heater_timer > pdMS_TO_TICKS(HEATER_TIMEOUT * 1000) && ADC.now_temp < ADC.set_temp-5)
                    {
                        heater_status.error_state = true;
                        
                    } 

                }
                //加热器非正常工作状态
                else {
                    output_pwm = 0;
                    heater_status.first_work = false;//重置加热器计时器
                    heater_output(output_pwm);

                }
                

                // else 
                // {
                //     // output_pwm = 0;
                //     // heater_output(0);
                // }
            }


        }

    }
    

}