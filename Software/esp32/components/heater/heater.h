#ifndef heater_H
#define heater_H

#include "esp_log.h"
#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "timer.h"
#include "adc.h"
#include "firetool_PID_adaptor.h"

#define HEATER_TIMER              LEDC_TIMER_1
#define HEATER_OUTPUT_IO          (13) // Define the output GPIO
#define HEATER_FREQUENCY          (2000)
#define HEATER_ChANNEL            LEDC_CHANNEL_1
#define HEATER_DUTY               (4095) // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
#define HEATER_FREQUENCY          (2000)


typedef struct {
    float Kp;           // 比例系数
    float Ki;           // 积分系数
    float Kd;           // 微分系数
    float integral;     // 积分项累积值
    float prev_error;   // 上一次误差
    float output_min;   // 输出最小值（限幅）
    float output_max;   // 输出最大值（限幅）
    uint32_t Sample_time; // 采样时间（ms）
    float target_value; // 目标值
} pid_controller_t;


void heater_init(void);
void heater_output(uint16_t pwm);
void pid_set_param(pid_controller_t *pid, float Kp, float Ki, float Kd);
void pid_set_sample_time(pid_controller_t *pid, uint32_t Sample_time);
void pid_set_output_limits(pid_controller_t *pid, float output_min, float output_max);
uint16_t pid_calculate(pid_controller_t *pid, float setpoint, float measured_value);
void pid_set_target_value(pid_controller_t *pid, float target_value);
void Temperature_Control_task(void);

extern bool en_pid;
extern uint16_t output_pwm;
extern bool PWM_state;


#endif