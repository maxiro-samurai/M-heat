#include "M-heat.h"


bool wifi_enable = false;
bool ble_state = false;


void app_main(void) {
  rotary_encoder_init();//旋转编码器初始化
  beep_init();//蜂鸣器初始化
  beep_test(&BootSound); // 声音也有点奇怪
  astra_ui_driver_init(); //初始化I2C驱动
  astra_init_core(); //初始化UI核心
  heater_init(); //初始化加热器
  nvs_init(); //初始化NVS
  nvs_get_parameter("Kp",&pid.Kp);
  nvs_get_parameter("Ki",&pid.Ki);
  nvs_get_parameter("Kd",&pid.Kd);
  wifi_init_sta();
  // // 设置时区并启动 SNTP
  // set_timezone();
  // initialize_sntp();
  // // 等待时间同步完成
  // wait_for_time_sync();
  UI_init();
  // 等待 Wi-Fi 连接成功
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  
//   display_qrcode("M-heat");
  
  BaseType_t xReturned = xTaskCreate(UI_task, "UI_task", 8192, NULL, 4, NULL);
  if(xReturned == pdPASS) {
    ESP_LOGI("Task", "UI_task created successfully");
  } else {
    ESP_LOGE("Task", "Failed to create UI_task");
  }
  xReturned = xTaskCreate(encoder_task, "encoder_task", 2048, &rotatry_encoder, 5, NULL);
  if(xReturned == pdPASS) {
    ESP_LOGI("Task", "encoder_task created successfully");
  } else {
    ESP_LOGE("Task", "Failed to create encoder_task");
  }

  xReturned = xTaskCreate(adc_oneshot_read_task, "adc_oneshot_read_task", 2048, NULL, 2, NULL);

  xTaskCreate(Temperature_Control_task, "Temperature_Control_task", 2048, NULL, 3, NULL);

  xTaskCreate(http_long_poll_task, "http_long_poll_task", 8192, NULL, 4, NULL);


}