/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "rotary_encoder.h"
 
static const char *TAG = "example";
QueueHandle_t encoder_queue = NULL;
QueueHandle_t buton_queue = NULL;
pcnt_unit_handle_t pcnt_unit = NULL;

rotary_encoder_item_t rotatry_encoder = {
    .en_torary = false, //禁用旋转编码器
    .key_state = IDLE,
    .encoder_state = IDLE_ENCODER,
    .last_tick = 0,
    .last_encoder_value = 0,
    .encoder_value = 0,
    .hold_tick = 0,
    .last_key_state = IDLE,
};
// rotary_encoder_item_t rotatry_encoder = {};

static bool example_pcnt_on_reach(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx)
 {
     BaseType_t high_task_wakeup;
     QueueHandle_t queue = (QueueHandle_t)user_ctx;
     // send event data to queue, from this interrupt callback
     // ESP_LOGI(TAG, "Watch point event, count: %d", edata->watch_point_value);
     xQueueSendFromISR(queue, &(edata->watch_point_value), &high_task_wakeup);
     pcnt_unit_clear_count(pcnt_unit); //达到计数最大/小值清零
     return (high_task_wakeup == pdTRUE);
 }
 
//  // 按键中断处理（可选）
//  static void IRAM_ATTR Key_isr(void* arg) {
//     static uint32_t last_press = 0;
//     uint32_t now = xTaskGetTickCountFromISR();

//     // 消抖处理（200ms内不重复触发）
//     if (now - last_press < pdMS_TO_TICKS(200)) return;
//     last_press = now;

//     // 发送按键事件到队列
//     int8_t btn_event = 0;
//     xQueueSendFromISR(buton_queue, &btn_event, NULL);
// }



static void pcnt_init(void)
{
    ESP_LOGI(TAG, "install pcnt unit");
     pcnt_unit_config_t unit_config = {
         .high_limit = EXAMPLE_PCNT_HIGH_LIMIT,
         .low_limit = EXAMPLE_PCNT_LOW_LIMIT,
     };
     
     ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit)); //新建pcnt单元
 
     ESP_LOGI(TAG, "set glitch filter");
     pcnt_glitch_filter_config_t filter_config = {
         .max_glitch_ns = 10000,
     };
     ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config)); //滤波器配置
 
     ESP_LOGI(TAG, "install pcnt channels");
     pcnt_chan_config_t chan_a_config = {
         .edge_gpio_num = EXAMPLE_EC11_GPIO_A,
         .level_gpio_num = EXAMPLE_EC11_GPIO_B,
     };
     pcnt_channel_handle_t pcnt_chan_a = NULL;
     ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));//A通道配置
     pcnt_chan_config_t chan_b_config = {
         .edge_gpio_num = EXAMPLE_EC11_GPIO_B,
         .level_gpio_num = EXAMPLE_EC11_GPIO_A,
     };
     pcnt_channel_handle_t pcnt_chan_b = NULL;
     ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_b_config, &pcnt_chan_b));//B通道配置
 
     ESP_LOGI(TAG, "set edge and level actions for pcnt channels");
     ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
     ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
     ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
     ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
 
     ESP_LOGI(TAG, "add watch points and register callbacks");
     int watch_points[] = {EXAMPLE_PCNT_LOW_LIMIT, EXAMPLE_PCNT_HIGH_LIMIT};
     for (size_t i = 0; i < sizeof(watch_points) / sizeof(watch_points[0]); i++) {
         ESP_ERROR_CHECK(pcnt_unit_add_watch_point(pcnt_unit, watch_points[i]));
     }
     pcnt_event_callbacks_t cbs = {
         .on_reach = example_pcnt_on_reach,
     };


     encoder_queue = xQueueCreate(10, sizeof(int16_t));
    

     ESP_ERROR_CHECK(pcnt_unit_register_event_callbacks(pcnt_unit, &cbs, encoder_queue));
 
     ESP_LOGI(TAG, "enable pcnt unit");
     ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
     ESP_LOGI(TAG, "clear pcnt unit");
     ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
     ESP_LOGI(TAG, "start pcnt unit");
     ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));
}

static void Key_init(void)
{
    ESP_LOGI(TAG, "install GPIO interrupt");
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL <<EXAMPLE_KEY_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    
    // ESP_LOGI(TAG, "install GPIO interrupt");
    // buton_queue = xQueueCreate(10, sizeof(int8_t));
    // ESP_ERROR_CHECK(gpio_install_isr_service(0));
    // ESP_ERROR_CHECK(gpio_isr_handler_add(EXAMPLE_KEY_GPIO, Key_isr, (void*)EXAMPLE_KEY_GPIO));
    

}

void rotary_encoder_init(void)
{
    pcnt_init();
    Key_init();
}

/*

@ 编码器状态扫描

*/

void encoder_state_detection(rotary_encoder_item_t *encoder) {
   
    // static int16_t last_encoder_val = 0; //上一次读取到的值
    // static int32_t last_tick = 0; //上次读取时间
    // static ButtonState button_state = IDLE; // 按键状态

    // int16_t encoder_val = 0; 

    pcnt_unit_get_count(pcnt_unit,&encoder->encoder_value); //读取编码器值
    int GPIO_LEVEL ; //读取GPIO电平
    uint32_t current_tick = xTaskGetTickCount() * portTICK_PERIOD_MS;  //获取当前时间
    
    
    // ESP_LOGI(TAG, "系统时间: %lu ms", current_tick);
    
    // 处理旋转方向
    if (encoder->encoder_value != encoder->last_encoder_value) {
        if (encoder->encoder_value > encoder->last_encoder_value + 3) {
            // ESP_LOGI(TAG, "编码器左旋转,值: %d", encoder->encoder_value);
            encoder->encoder_state = LEFT;
        } else if(encoder->encoder_value < encoder->last_encoder_value - 3) {
            // ESP_LOGI(TAG, "编码器右旋转,值: %d", encoder->encoder_value);
            encoder->encoder_state = RIGHT;
        }
        encoder->last_encoder_value = encoder->encoder_value; 

    } else {
        // ESP_LOGI(TAG, "编码器未动作");
        encoder->encoder_state = IDLE_ENCODER; // 编码器未动作
    }
   
    GPIO_LEVEL = gpio_get_level(EXAMPLE_KEY_GPIO);
    
        switch (encoder->key_state) // 按键状态机
        {
        case IDLE:
            if (GPIO_LEVEL == 0) { // 按键按下
                encoder->key_state = PRESSED;
                encoder->last_tick = current_tick; // 记录按下时间
                encoder->last_key_state = IDLE; // 记录上次按键状态
                // ESP_LOGI(TAG, "按键按下");
            }
            break;
        
        case PRESSED:
            if (current_tick - encoder->last_tick > pdMS_TO_TICKS(50)) { // 按键保持超过200ms
                if (GPIO_LEVEL == 0){
                    encoder->key_state = HOLD;
                    // ESP_LOGI(TAG, "按键保持超过50ms");

                    encoder->last_tick = current_tick; // 更新按下时间
                } else {
                    encoder->key_state = RELEASED; // 按键释放
                
                }
                encoder->last_key_state = PRESSED; // 记录上次按键状态
            } 
            break;
        
        case HOLD:
            if (GPIO_LEVEL == 1) { // 释放
                encoder->key_state = RELEASED;
                encoder->last_tick = current_tick; // 记录按下时间
            } else {
                if (current_tick - encoder->last_tick > pdMS_TO_TICKS(100)) { // 按键保持超过200ms
                    // ESP_LOGI(TAG, "按键保持%lums",current_tick - encoder->last_tick);
                    // encoder->last_tick = current_tick; // 记录按下时间
                    encoder->hold_tick = current_tick - encoder->last_tick; // 记录按下时间 
                }
            } 
            encoder->last_key_state = HOLD; // 记录上次按键状态
            break;

        case RELEASED:
            if (GPIO_LEVEL == 1) { // 按键释放
                // if (GPIO_LEVEL == 1 )

                encoder->key_state = IDLE; // 重新进入空闲状态
                // ESP_LOGI(TAG, "按键释放");
                encoder->last_key_state = RELEASED; // 记录上次按键状态
            } 
            break;
            
            

        }
    // } 
}

/*

编码器或按钮键反应选择对应item

*/
void encoder_select_item(rotary_encoder_item_t *encoder){

    if (!in_astra) return; 
    if (encoder->encoder_state == LEFT) {
        ESP_LOGI(TAG, "选择上一个item");
        astra_selector_go_prev_item(); //选择上一个item
    } else if (encoder->encoder_state == RIGHT) {
        ESP_LOGI(TAG, "选择下一个item");
        astra_selector_go_next_item(); //选择下一个item
    } 
    
    if (encoder->key_state == RELEASED && encoder->last_key_state == PRESSED) {
        ESP_LOGI(TAG, "确认选择的item");
        astra_selector_jump_to_selected_item(); //确认选择的item
    } else if (encoder->key_state == RELEASED && encoder->last_key_state == HOLD) {
        if ( encoder->hold_tick < 2000) {
           
        
            ESP_LOGI(TAG, "长按退出选择的item");
            astra_selector_exit_current_item(); //长按确认选择的item
            
        }
        encoder->key_state = IDLE;
    }
    


}

void encoder_task(void *arg) {
    
    while (1) {
        
        encoder_state_detection(arg);
        encoder_select_item(arg);
        vTaskDelay(400 / portTICK_PERIOD_MS); // 延时10ms
    }
}

 