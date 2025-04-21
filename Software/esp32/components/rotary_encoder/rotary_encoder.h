#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "astra_ui_item.h"
#include "astra_ui_core.h"
#define EXAMPLE_PCNT_HIGH_LIMIT 100
#define EXAMPLE_PCNT_LOW_LIMIT  -100

#define EXAMPLE_EC11_GPIO_A 14
#define EXAMPLE_EC11_GPIO_B 15
#define EXAMPLE_KEY_GPIO 16

typedef enum {
    IDLE,
    PRESSED,
    HOLD,
    RELEASED
  }ButtonState; // 按键状态

typedef enum{
    LEFT,
    RIGHT,
    IDLE_ENCODER
}EncoderState; // 编码器状态

typedef struct rotary_encoder_item_t
{
    bool en_torary;  //当前选中的selecter
    ButtonState key_state;
    EncoderState encoder_state;
    uint32_t last_tick; //上次按下时间
    int16_t last_encoder_value; //上次编码器值
    int16_t encoder_value; //编码器值
    ButtonState last_key_state; //上次按键状态
    uint32_t hold_tick; //按键保持时间
}rotary_encoder_item_t;

extern rotary_encoder_item_t rotatry_encoder; //编码器对象
void rotary_encoder_init(void);
void encoder_task(void *arg);

#endif