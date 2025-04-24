#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "astra_ui_core.h"
#include "astra_ui_draw_driver.h"
#include "astra_ui_item.h"
#include "rotary_encoder.h"
#include "beep.h"
#include "adc.h"
#include "externdraw.h"
#include "firetool_PID_adaptor.h"
#include "heater.h"
#include "nvs_data.h"
#include "timer.h"
// 示例旋律（《小星星》片段）
Tone melody[] = {
  {262, 500},  // C4
  {262, 500},
  {392, 500},  // G4
  {392, 500},
  {440, 500},  // A4
  {440, 500},
  {392, 1000}  // G4
};
FireToolPIDAdaptor fireToolPidAdaptor;

bool wifi_enable = false;

void fta_start_callback(unsigned char channel)
{
    printf("start command!,channel: %d\n", channel);
}

void fta_stop_callback(unsigned char channel)
{
    printf("stop command!channel: %d\n", channel);
}

void fta_reset_callback(unsigned char channel)
{
    printf("reset command!channel: %d\n", channel);
}

void fta_targetValue_callback(unsigned char channel, int32_t targetValue)
{
    printf("targetValue: %ld, channel: %d\n", targetValue, channel);
}

void fta_periodValue_callback(unsigned char channel, int32_t targetValue)
{
    printf("periodValue: %ld, channel: %d\n", targetValue, channel);
}

void fta_PID_callback(unsigned char channel, float P, float I, float D)
{
    printf("channel: %d, PID:%f, %f, %f\n", channel, P, I, D);
}

void fta_test_send_cmd(void * pvParam)
{
    while(1){
        fta_send_start_cmd(&fireToolPidAdaptor, 1);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        fta_send_stop_cmd(&fireToolPidAdaptor, 1);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        fta_send_targetValue(&fireToolPidAdaptor, 1, 14960);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        fta_send_periodValue(&fireToolPidAdaptor, 1, 10);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        fta_send_PID(&fireToolPidAdaptor, 1, 0.05, 0, 0);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        fta_send_actualValue(&fireToolPidAdaptor, 1, 45);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
gptimer_handle_t gptimer = NULL;  
QueueHandle_t queue = NULL;

void init_temp_plot(void){

  queue = xQueueCreate(10, sizeof(uint64_t)); // 创建队列，用于接收定时器事件
  
  timer_init(&gptimer, queue); // 初始化定时器
  // timer_set_alarm(gptimer, 500000); // 设置定时器触发值为100ms
  // timer_start(gptimer); // 启动定时器
  // 初始化温度数据为初始值
  for (int i = 0; i < SCREEN_WIDTH; i++) {
    temperature_data[i] = 25.0;
  }

}


void temp_plot_quit(void){

  oled_clear_buffer(); // 清除OLED显示

}




void my_test_task(void *arg) {

  

    while (1) {
      

      
      oled_clear_buffer();
      // Draw_Num_Bar(0.5, 0, 1, 0, 0, 128, 64, 1);
      System_UI();
      astra_ui_main_core();
      astra_ui_widget_core();
      // // test_user_item_loop_function();
      oled_send_buffer();
      vTaskDelay(pdMS_TO_TICKS(10)); // 延时 10 毫秒
    }
}

void beep_task(void *arg) {
  while (1) {
    
    play_melody(melody, sizeof(melody)/sizeof(Tone));
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void app_main(void) {
  rotary_encoder_init();
  beep_init();
  astra_ui_driver_init(); //初始化I2C驱动

  astra_init_core(); //初始化UI核心
  heater_init(); //初始化加热器
  nvs_init(); //初始化NVS
  astra_list_item_t* setting_list_item = astra_new_list_item("Setup");

  astra_list_item_t* wifi_list_item = astra_new_list_item("Wifi");
  astra_list_item_t* ble_list_item = astra_new_list_item("Bluetooth");
  astra_list_item_t* temp_control_item = astra_new_list_item("Temperature Control"); 
  astra_list_item_t* exit_list_item = astra_new_list_item("exit");
  astra_push_item_to_list(astra_get_root_list(), setting_list_item);
  astra_push_item_to_list(setting_list_item, wifi_list_item);
  astra_push_item_to_list(setting_list_item, ble_list_item);
  astra_push_item_to_list(astra_get_root_list(), temp_control_item);
  astra_push_item_to_list(astra_get_root_list(), exit_list_item);
  astra_push_item_to_list(wifi_list_item, astra_new_switch_item("Enable wifi",&wifi_enable ));
  astra_push_item_to_list(temp_control_item, astra_new_slider_item("Temperature", &ADC.set_temp,10,10,300));


  astra_push_item_to_list(temp_control_item,astra_new_user_item("Temp plot",init_temp_plot,temp_plot,temp_plot_quit));


  BaseType_t xReturned = xTaskCreate(my_test_task, "my_test_task", 4096+2048, NULL, 5, NULL);
  if(xReturned == pdPASS) {
    ESP_LOGI("Task", "Task created successfully");
  } else {
    ESP_LOGE("Task", "Failed to create task");
  }
  BaseType_t xReturned2 = xTaskCreate(encoder_task, "encoder_task", 2048, &rotatry_encoder, 4, NULL);
  if(xReturned2 == pdPASS) {
    ESP_LOGI("Task", "Task created successfully");
  } else {
    ESP_LOGE("Task", "Failed to create task");
  }

  BaseType_t xReturned3 = xTaskCreate(adc_oneshot_read_task, "adc_oneshot_read_task", 2048, NULL, 4, NULL);

  //  /**调用初始化函数之前必须先调用设置回调函数函数！！！！！！**/
  //  fta_set_received_start_cb(&fireToolPidAdaptor, fta_start_callback);
  //  fta_set_received_stop_cb(&fireToolPidAdaptor, fta_stop_callback);
  //  fta_set_received_reset_cb(&fireToolPidAdaptor, fta_reset_callback);
  //  fta_set_received_targetValue(&fireToolPidAdaptor, fta_targetValue_callback);
  //  fta_set_received_periodValue(&fireToolPidAdaptor, fta_periodValue_callback);
  //  fta_set_received_PID(&fireToolPidAdaptor, fta_PID_callback);

  //  fta_init(&fireToolPidAdaptor, 115200, 18, 17, UART_NUM_1, 1024);
  // BaseType_t xReturned4 = xTaskCreate(fta_test_send_cmd, "fta_test_send_cmd", 2048, NULL, 4, NULL);

}