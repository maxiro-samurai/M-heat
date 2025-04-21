#ifndef heater_H
#define heater_H

#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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
} pid_controller_t;







#endif