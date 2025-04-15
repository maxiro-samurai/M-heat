//
// Created by Fir on 24-11-30.
//
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>


#include "sdkconfig.h"
#include "u8g2_esp32_hal.h"
#include "u8g2.h"
#include "astra_ui_draw_driver.h"


// SDA - GPIO21
#define PIN_SDA 21

// SCL - GPIO22
#define PIN_SCL 22


static const char* TAG = "ssd1306";


// /* 此处自行编写oled及图形库初始化函数所需的函数 */
u8g2_t u8g2;

uint32_t launcher_get_tick_ms()
{
  return xTaskGetTickCount() * portTICK_PERIOD_MS;
}


void esp32_i2c_hal_init()
{
  u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
  u8g2_esp32_hal.bus.i2c.sda = PIN_SDA;
  u8g2_esp32_hal.bus.i2c.scl = PIN_SCL;
  u8g2_esp32_hal_init(u8g2_esp32_hal);
}

void u8g2_init(u8g2_t *u8g2)
{
  u8g2_Setup_ssd1306_i2c_128x64_noname_f(
      u8g2, U8G2_R0,
      // u8x8_byte_sw_i2c,
      u8g2_esp32_i2c_byte_cb,
      u8g2_esp32_gpio_and_delay_cb);  // init u8g2 structure
      u8x8_SetI2CAddress(&u8g2->u8x8, 0x78);

      ESP_LOGI(TAG, "u8g2_InitDisplay");
      u8g2_InitDisplay(u8g2);  // send init sequence to the display, display is in
                                // sleep mode after this,
  
      ESP_LOGI(TAG, "u8g2_SetPowerSave");
      u8g2_SetPowerSave(u8g2, 0);  // wake up display
      ESP_LOGI(TAG, "u8g2_ClearBuffer");
      u8g2_ClearBuffer(u8g2);

      ESP_LOGI(TAG, "u8g2_SendBuffer");
      u8g2_SendBuffer(u8g2);
  
      ESP_LOGI(TAG, "All done!");
}

/* 此处自行编写oled及图形库初始化函数所需的函数 */

/* 此处自行修改内部函数名 */
void astra_ui_driver_init()
{
    esp32_i2c_hal_init();
    u8g2_init(&u8g2);
    oled_set_font(u8g2_font_my_chinese);
}

/* 此处自行修改内部函数名 */
