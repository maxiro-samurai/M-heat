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
#include <math.h>
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
long map(long x, long in_min, long in_max, long out_min, long out_max) {
  const long run = in_max - in_min;
  if(run == 0){
      
      return -1; // AVR returns -1, SAM returns 0
  }
  const long rise = out_max - out_min;
  const long delta = x - in_min;
  return (delta * rise) / run + out_min;
}
/*
	@作用：绘制数值条
	@输入：i=值 a=值的最小值 b=值的最大值 x=左上顶点x轴坐标 y=左上顶点y轴坐标 w=宽度 h=高度 c=颜色
*/
void Draw_Num_Bar(float i, float a, float b, int x, int y, int w, int h, int c)
{
    char buffer[20];
    sprintf(buffer, "%.2f", i);
    uint8_t textWidth = oled_get_UTF8_width(buffer) + 3;

    oled_set_draw_color(c);
    oled_draw_frame(x, y, w - textWidth - 2, h);
    oled_draw_box(x + 2, y + 2, map(i, a, b, 0, w - textWidth - 6), h - 4);


    oled_draw_str(x + w - textWidth, y - 1, buffer);
    //进行去棱角操作:增强文字视觉焦点
    oled_set_draw_color(0);
    oled_set_draw_color(1);
}



void my_test_task(void *arg) {
    while (1) {
      
      oled_clear_buffer();
      Draw_Num_Bar(24.5, 0, 100, 0, 0, 128, 10, 1);
      // astra_ui_widget_core();
      // astra_ui_main_core();
      // test_user_item_loop_function();
      oled_send_buffer();
      // vTaskDelay(1 / portTICK_PERIOD_MS);
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

  // astra_init_core(); //初始化UI核心
  // in_astra = 1;
  astra_list_item_t* test_list_item = astra_new_list_item("关于ESP32");

  astra_list_item_t* test_list_item2 = astra_new_list_item("test1");
  astra_list_item_t* test_list_item3 = astra_new_list_item("test2");
  astra_list_item_t* test_list_item4 = astra_new_list_item("test3"); 
  astra_list_item_t* test_list_item5 = astra_new_list_item("hello world");
  astra_list_item_t* test_list_item6 = astra_new_list_item("test4");
  astra_list_item_t* test_list_item7 = astra_new_list_item("test5");
  astra_push_item_to_list(astra_get_root_list(), test_list_item);
  astra_push_item_to_list(astra_get_root_list(), test_list_item2);
  astra_push_item_to_list(astra_get_root_list(), test_list_item3);
  astra_push_item_to_list(astra_get_root_list(), test_list_item4);
  astra_push_item_to_list(astra_get_root_list(), test_list_item5);
  astra_push_item_to_list(astra_get_root_list(), test_list_item6);
  astra_push_item_to_list(astra_get_root_list(), test_list_item7);
  BaseType_t xReturned = xTaskCreate(my_test_task, "my_test_task", 4096, NULL, 5, NULL);
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

  BaseType_t xReturned3 = xTaskCreate(adc_oneshot_read_task, "adc_oneshot_read_task", 4096, NULL, 4, NULL);
// BaseType_t xReturned3 = xTaskCreate(beep_task, "beep_task", 2048, NULL, 4, NULL);
//   if(xReturned3 == pdPASS) {
//     ESP_LOGI("Task", "Task created successfully");
//   } else {
//     ESP_LOGE("Task", "Failed to create task");
//   }
  // while(1)
  // {
  //   oled_clear_buffer();

  //   oled_draw_UTF8(0, 0, "abccdefg");
  //   oled_draw_UTF8(0, 10, "1234567890");
  //   // oled_draw_UTF8(0, 30, "关于ESP32");

  //   oled_send_buffer();
  // }

}