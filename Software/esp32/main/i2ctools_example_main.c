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


bool wifi_enable = false;
bool ble_state = false;


void init_temp_plot(void){


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
  heater_output(2000);
  nvs_init(); //初始化NVS
  astra_list_item_t* setting_list_item = astra_new_list_item("Setup");

  astra_list_item_t* wifi_list_item = astra_new_list_item("Wifi");
  astra_list_item_t* ble_list_item = astra_new_list_item("Bluetooth");
  astra_list_item_t* temp_control_item = astra_new_list_item("Temperature Control"); 
  astra_list_item_t* about_list_item = astra_new_list_item("关于");

  astra_push_item_to_list(astra_get_root_list(), setting_list_item);
  astra_push_item_to_list(setting_list_item, wifi_list_item);
  astra_push_item_to_list(setting_list_item, ble_list_item);
  astra_push_item_to_list(astra_get_root_list(), temp_control_item);
  astra_push_item_to_list(astra_get_root_list(), about_list_item);
  astra_push_item_to_list(wifi_list_item, astra_new_switch_item("Enable wifi",&wifi_enable ));
  astra_push_item_to_list(ble_list_item, astra_new_switch_item("Enable BLE",&ble_state ));
  astra_push_item_to_list(temp_control_item, astra_new_slider_item("Temperature", &ADC.set_temp,10,10,250));


  astra_push_item_to_list(temp_control_item,astra_new_user_item("Temp plot",init_temp_plot,temp_plot,temp_plot_quit));


  BaseType_t xReturned = xTaskCreate(my_test_task, "my_test_task", 4096+2048, NULL, 5, NULL);
  if(xReturned == pdPASS) {
    ESP_LOGI("Task", "Task created successfully");
  } else {
    ESP_LOGE("Task", "Failed to create task");
  }
  BaseType_t xReturned2 = xTaskCreate(encoder_task, "encoder_task", 2048, &rotatry_encoder, 3, NULL);
  if(xReturned2 == pdPASS) {
    ESP_LOGI("Task", "Task created successfully");
  } else {
    ESP_LOGE("Task", "Failed to create task");
  }

  BaseType_t xReturned3 = xTaskCreate(adc_oneshot_read_task, "adc_oneshot_read_task", 2048, NULL, 3, NULL);

  // xTaskCreate(Temperature_Control_task, "Temperature_Control_task", 2048, NULL, 4, NULL);


}