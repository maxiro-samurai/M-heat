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

uint32_t time_start = 0;
static int16_t y_logo = 200;
static int16_t y_version = 200;
static int16_t y_name = -200;
static int16_t y_astra = -200;
static int16_t y_box = 200;
static int16_t x_board = -200;
static int16_t y_wire_1 = 200;
static int16_t y_wire_2 = 200;

void test_user_item_init_function()
{
  time_start = get_ticks();
}

void test_user_item_loop_function()
{
  uint32_t _time = get_ticks();

  oled_set_draw_color(1);
  oled_draw_R_box(2, y_box - 1, oled_get_UTF8_width("「astraLauncher」") + 4, oled_get_str_height() + 2, 1);
  oled_set_draw_color(2);
  oled_draw_UTF8(4, y_logo - 2, "「astraLauncher」");

  oled_set_draw_color(1);
  oled_draw_str(106, y_version, "v1.0");
  oled_draw_UTF8(2, y_name, "by 无理造物.");
  oled_draw_UTF8(2, y_astra, "由「astra UI Lite」v1.0");
  oled_draw_UTF8(2, y_astra + 14, "轻量驱动.");
  oled_draw_frame(x_board, 38, 28, 20);
  oled_draw_frame(x_board + 2, 40, 24, 10);
  oled_draw_box(x_board + 2, 40, 2, 10);
  oled_draw_pixel(x_board + 25, 51);
  oled_draw_pixel(x_board + 25, 53);
  oled_draw_pixel(x_board + 25, 55);
  oled_draw_box(x_board + 21, 51, 3, 2);
  oled_draw_box(x_board + 21, 54, 3, 2);
  oled_draw_box(x_board + 17, 53, 3, 3);

  oled_draw_box(x_board + 12, 53, 4, 3);
  oled_draw_box(x_board + 7, 53, 4, 3);
  oled_draw_box(x_board + 2, 53, 4, 3);

  oled_draw_box(x_board + 7, y_wire_1, 4, 3);
  oled_draw_V_line(x_board + 9, y_wire_1 + 3, 3);
  oled_draw_V_line(x_board + 8, y_wire_1 + 6, 2);

  oled_draw_box(x_board + 12, y_wire_2, 4, 3);
  oled_draw_V_line(x_board + 14, y_wire_2 + 3, 3);
  oled_draw_V_line(x_board + 15, y_wire_2 + 6, 2);

  // if (_time - time_start > 300) animation(&y_logo, 15, 94);
  // if (_time - time_start > 350) animation(&y_version, 14, 88);
  // if (_time - time_start > 400) animation(&y_box, 2, 92);
  // if (_time - time_start > 450) animation(&y_astra, 36, 91);
  // if (_time - time_start > 500) animation(&y_name, 62, 94);
  // if (_time - time_start > 550) animation(&x_board, 102, 92);
  // if (_time - time_start > 620) animation(&y_wire_1, 56, 86);
  // if (_time - time_start > 1400 && _time - time_start < 1600) oled_draw_box(x_board + 5, 42, 19, 6);
  // if (_time - time_start > 1800 && _time - time_start < 1900) oled_draw_box(x_board + 5, 42, 19, 6);
  // if (_time - time_start > 2200) oled_draw_box(x_board + 5, 42, 19, 6);
  // if (_time - time_start > 2400) animation(&y_wire_2, 56, 86);
}

void test_user_item_exit_function()
{
  time_start = 0;
  y_logo = 200;
  y_version = 200;
  y_name = -200;
  y_astra = -200;
  y_box = 200;
  x_board = -200;
  y_wire_1 = 200;
  y_wire_2 = 200;
}


void my_test_task(void *arg) {
    while (1) {
      
      oled_clear_buffer();
      astra_ui_widget_core();
      astra_ui_main_core();
      // test_user_item_loop_function();
      oled_send_buffer();
      // vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}




void app_main(void) {
  

  rotary_encoder_init();

  astra_ui_driver_init(); //初始化I2C驱动
  // u8g2_SetFont(&u8g2, u8g2_font_my_chinese);
  astra_init_core(); //初始化UI核心
  in_astra = 1;
  astra_list_item_t* test_list_item = astra_new_list_item("关于ESP32");

  astra_list_item_t* test_list_item2 = astra_new_list_item("关于ESP32-DevKitC");
  astra_list_item_t* test_list_item3 = astra_new_list_item("关于ESP32-DevKitC-V4");
  astra_push_item_to_list(astra_get_root_list(), test_list_item);
  astra_push_item_to_list(astra_get_root_list(), test_list_item2);
  astra_push_item_to_list(astra_get_root_list(), test_list_item3);
  BaseType_t xReturned = xTaskCreate(my_test_task, "my_test_task", 4096, NULL, 5, NULL);
  if(xReturned == pdPASS) {
    ESP_LOGI("Task", "Task created successfully");
  } else {
    ESP_LOGE("Task", "Failed to create task");
  }
  BaseType_t xReturned2 = xTaskCreate(encoder_task, "encoder_task", 4096, &rotatry_encoder, 4, NULL);
  if(xReturned2 == pdPASS) {
    ESP_LOGI("Task", "Task created successfully");
  } else {
    ESP_LOGE("Task", "Failed to create task");
  }

  // while(1)
  // {
    
  // }

}