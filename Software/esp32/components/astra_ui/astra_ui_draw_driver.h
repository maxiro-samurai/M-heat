//
// Created by Fir on 24-11-29.
//

#ifndef FUCKCLION_CORE_SRC_ASTRA_UI_LITE_DRAW_DRIVER_H_
#define FUCKCLION_CORE_SRC_ASTRA_UI_LITE_DRAW_DRIVER_H_

/* 此处自行添加头文件 */
#include "u8g2.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_err.h"
// #include "main.h"
// #include "spi.h"
// #include "../astra-launcher/launcher_delay.h"
/* 此处自行添加头文件 */





/* 此处修改oled绘制函数 */
// extern u8g2_t u8g2;
#define SSD1306_ADDR 0x3C
#define I2C_MASTER_SCL_IO           22      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           21 
#define I2C_MASTER_FREQ_HZ          400000 
#define I2C_MASTER_NUM              0  
#define OLED_HEIGHT 64
#define OLED_WIDTH 128

#define get_ticks() ____
#define delay(ms) ____
#define oled_set_font(font) ____
#define oled_draw_str(x, y, str) ____
#define oled_draw_UTF8(x, y, str) ____
#define oled_get_str_width(str) ____
#define oled_get_UTF8_width(str) ____
#define oled_get_str_height() ____
#define oled_draw_pixel(x, y) ____
#define oled_draw_circle(x, y, r) ____
#define oled_draw_R_box(x, y, w, h, r) ____
#define oled_draw_box(x, y, w, h) ____
#define oled_draw_frame(x, y, w, h) ____
#define oled_draw_R_frame(x, y, w, h, r) ____
#define oled_draw_H_line(x, y, l) ____
#define oled_draw_V_line(x, y, h) ____
#define oled_draw_line(x1, y1, x2, y2) ____
#define oled_draw_H_dotted_line(x, y, l) ____
#define oled_draw_V_dotted_line(x, y, h) ____
#define oled_draw_bMP(x, y, w, h, bitMap) ____
#define oled_set_draw_color(color) ____
#define oled_set_font_mode(mode) ____
#define oled_set_font_direction(dir) ____
#define oled_draw_pixel(x, y) ____
#define oled_clear_buffer() ____
#define oled_send_buffer() ____
#define oled_send_area_buffer(x, y, w, h) ____
/* 此处修改oled绘制函数 */

extern void astra_ui_driver_init();

#endif //FUCKCLION_CORE_SRC_ASTRA_UI_LITE_DRAW_DRIVER_H_
