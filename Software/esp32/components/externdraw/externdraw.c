#include "externdraw.h"

uint8_t TempCTRL_Status = TEMP_STATUS_OFF;
uint8_t* C_table[] = {c1, c2, c3, Lightning, c5, c6, c7};
char* TempCTRL_Status_Mes[] = {
    "错误",
    "停机",
    "休眠",
    "提温",
    "正常",
    "加热",
    "维持",
};
extern bool ble_state;
extern bool wifi_enable;
static const char *TAG = "example";


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

/*

图标驱动

*/
void Draw_Slow_Bitmap(int x, int y, const unsigned char* bitmap, unsigned char w, unsigned char h)
{
    uint8_t color = oled_get_draw_color();
    int xi, yi, intWidth = (w + 7) / 8;
    for (yi = 0; yi < h; yi++)
    {
        for (xi = 0; xi < w; xi++)
        {
            if (pgm_read_byte(bitmap + yi * intWidth + xi / 8) & (128 >> (xi & 7)))
            {
                oled_draw_pixel(x + xi, y + yi);
            } else if (color != 2)
            {
                oled_set_draw_color(0);
                oled_draw_pixel(x + xi, y + yi);
                oled_set_draw_color(color);
            }
        }
    }
}
/*** 
 * @description: 绘制温度状态条
 * @param bool color 颜色
 * @return {*}
 */
void DrawStatusBar(bool color)
{
    char buff[20];
    int font_height = oled_get_str_height(); // 获取字体高度
    oled_set_draw_color(color);
    //温度条
    //框
    oled_draw_frame(0, 53, 103, 11);
    //条
    if (ADC.now_temp <= TEMP_MAX)
        oled_draw_box(0, 53, map(ADC.now_temp, TEMP_MIN, TEMP_MAX, 5, 98), 11);
    // if (TipTemperature <= TipMaxTemp)
    

    //功率条
    oled_draw_frame(104, 53, 23, 11);
    oled_draw_box(104, 53, map(output_pwm, 0, 8191, 0, 23), 11);

    oled_draw_H_line(117, 51, 11);
    oled_draw_pixel(103, 52);
    oled_draw_pixel(127, 52);
     // // 切换回正常颜色模式
    // oled_set_draw_color(0);
    // oled_set_font(u8g2_font_5x8_tn);
    // 显示温度值
    oled_set_draw_color(!color);
    sprintf(buff, "%d", ADC.set_temp);
    oled_draw_UTF8(2, 62, buff);

    // 显示功率值
    sprintf(buff, "%lu%%", map(output_pwm, 0, 8191, 0, 100));
    oled_draw_UTF8(105, 62, buff);
    //////////////进入反色////////////////////////////////
    // 进入反色模式
    oled_set_draw_color(2);

    // 画指示针
    Draw_Slow_Bitmap(map(250, 0, 300, 5, 98) - 4, 54, PositioningCursor, 8, 8);
    
   

    // 刷新显示缓冲区
    
    


    // arduboy.setCursor(105, 55); arduboy.print(map(PID_Output, 255, 0, 0, 100)); arduboy.print(F("%")); //功率百分比
    oled_set_draw_color(color);
}

void System_UI(void)
{
    if(in_astra) return;
    
    char temp[20];
    char _msg[20];
    int font_height = oled_get_str_height(); // 获取字体高度
    //M-HEAT项目名称
    oled_draw_UTF8(0, font_height-1, "M-heat");
  

    // 温度控制图标
    Draw_Slow_Bitmap(74, 37, C_table[TempCTRL_Status], 14, 14);
    sprintf(temp,"%.2fV", system_vol); //获取温度值
    oled_draw_UTF8(90, font_height, temp);
    if (system_vol <19){

        if ((get_ticks() / 1000) % 2)
                {
                    //欠压告警图标
                    Draw_Slow_Bitmap(74, 21, Battery_NoPower, 14, 14);
                } 
                // else
                // {
                //     //主电源电压
                //     sprintf(temp,"%.2f v", system_vol); //获取温度值
                //     oled_draw_UTF8(100, font_height-1, temp);
                //     // ESP_LOGI(TAG, "vol: %.2f", system_vol);
                //     // Disp.printf("%.1fV", Get_MainPowerVoltage());
                // }
                // PWMOutput_Lock = true;
    }
    //图标下显示中文
    oled_set_font(u8g2_font_my_chinese);
    oled_draw_UTF8(91, 40, TempCTRL_Status_Mes[TempCTRL_Status]);
    // 欠压报警

    /*待完善*/


    //蓝牙显示
    if (ble_state) Draw_Slow_Bitmap(92, 25, IMG_BLE_S, 9, 11);
    if (wifi_enable) Draw_Slow_Bitmap(92, 25, epd_bitmap_wifi_solid, 16, 16);


    //主页面显示温度
    oled_set_font(u8g2_font_logisoso38_tr);
    
    if (TempCTRL_Status == TEMP_STATUS_ERROR){
        if((get_ticks()/250) % 2) oled_draw_str(0, 51, "---");

    }
    else{
        sprintf(temp,"%d", ADC.now_temp); //获取温度
        oled_draw_str(0, 51, temp);
    }
    oled_set_font(u8g2_font_my_chinese); //先显示温度
    //右上角运行指示角标

    if(output_pwm >0 && PWM_state)
    {

        uint8_t TriangleSize = map(output_pwm, 0, 8191, 16, 0);
    
        oled_draw_triangle((119 - 12) + TriangleSize, 12, 125, 12, 125, (18 + 12) - TriangleSize);


    }
    


    /////////////////////////////////////绘制遮罩层//////////////////////////////////////////////
    oled_set_draw_color(2);
    //几何图形切割
    oled_draw_box(0, 12, 96, 40);
    oled_draw_triangle(96, 12, 96, 52, 125, 42);
    oled_draw_triangle(125, 42, 96, 52, 118, 52);
    oled_set_draw_color(1);
    
    DrawStatusBar(1);

    // //如果当前是处于爆发技能，则显示技能剩余时间进度条
    // if (TempCTRL_Status == TEMP_STATUS_BOOST && DisplayFlashTick % 2)
    // {
    //     uint8_t BoostTimeBar = map(millis() - BoostTimer, 0, BoostTime * 1000, 0, 14);
    //     Disp.drawBox(74, 37, 14, BoostTimeBar);

    // }

   
    if (rotatry_encoder.key_state ==HOLD){

        if (rotatry_encoder.hold_tick < 2000){

            // sprintf(_msg, "2s");
            // ESP_LOGI(TAG, "Hold time: %s", _msg);
            astra_push_info_bar("Keep holding 2s", 2000);
            
        }
        else if (rotatry_encoder.hold_tick>=2000){
            astra_push_info_bar("Enter menu! :p", 2000);
            rotatry_encoder.hold_tick = 0;
            in_astra = true;

        }
        
}


}   

float temperature_data[SCREEN_WIDTH];  // 温度数据数组


// 更新温度数据（左移旧数据，插入新数据到末尾）
void update_temperature_data(float new_temp) {
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        temperature_data[i] = temperature_data[i + 1];
    }
    temperature_data[SCREEN_WIDTH - 1] = new_temp;
  }
  



/*
温度曲线绘制函数
*/
void temp_plot(void)

{
   
    //获取当前颜色
    uint8_t color = oled_get_draw_color();
    update_temperature_data(ADC.now_temp); // 更新温度数据
    oled_set_draw_color(1);//设置颜色为白色
    // --- 自动计算 Y 轴范围 ---
    float temp_min = temperature_data[0];
    float temp_max = temperature_data[0];
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        if (temperature_data[i] < temp_min) temp_min = temperature_data[i];
        if (temperature_data[i] > temp_max) temp_max = temperature_data[i];
    }
    // 处理数据全等或范围过小的情况
    if (temp_max - temp_min < 1.0f) {
        temp_min -= 0.5f;
        temp_max += 0.5f;
    }

     // 绘制坐标系
    oled_draw_H_line( 0, SCREEN_HEIGHT - 1, SCREEN_WIDTH - 1); // X轴
    oled_draw_V_line( 0, 0,SCREEN_HEIGHT - 1);                                // Y轴
      // --- 动态绘制 Y 轴标签 ---
    u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
    char label[16];
    // 绘制最小值
    snprintf(label, sizeof(label), "%.0f", temp_min);
    u8g2_DrawStr(&u8g2, 2, SCREEN_HEIGHT - 5, label);
    // 绘制最大值
    snprintf(label, sizeof(label), "%.0f", temp_max);
    u8g2_DrawStr(&u8g2, 2, 10, label);
    
    // --- 映射温度到屏幕 Y 坐标 ---
    for (int i = 0; i < SCREEN_WIDTH - 1; i++) {
        int x1 = i;
        int x2 = i + 1;
        // 动态计算 Y 坐标
        int y1 = SCREEN_HEIGHT - 1 - (int)((temperature_data[i] - temp_min) * (SCREEN_HEIGHT - 1) / (temp_max - temp_min));
        int y2 = SCREEN_HEIGHT - 1 - (int)((temperature_data[i + 1] - temp_min) * (SCREEN_HEIGHT - 1) / (temp_max - temp_min));
        // 边界保护
        y1 = (y1 < 0) ? 0 : (y1 >= SCREEN_HEIGHT) ? SCREEN_HEIGHT - 1 : y1;
        y2 = (y2 < 0) ? 0 : (y2 >= SCREEN_HEIGHT) ? SCREEN_HEIGHT - 1 : y2;
        u8g2_DrawLine(&u8g2, x1, y1, x2, y2);
    }

    
    oled_set_draw_color(2);
    //几何图形切割
    oled_draw_box(0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1);
    oled_set_draw_color(color); //恢复颜色
    vTaskDelay(200 / portTICK_PERIOD_MS); // 延时 0.2 秒
}
/**
 * 初始化温度绘制函数
 */

void init_temp_plot(void){


  for (int i = 0; i < SCREEN_WIDTH; i++) {
    temperature_data[i] = 25.0;
  }

}

/**
 * 退出温度绘图
 */

void temp_plot_quit(void){

  oled_clear_buffer(); // 清除OLED显示

}

void init_save_config(void){
    
    esp_err_t err1 = save_float_to_nvs("Kp",pid.Kp);
    esp_err_t err2 = save_float_to_nvs("Ki",pid.Ki);
    esp_err_t err3 = save_float_to_nvs("Kd",pid.Kd);
    if (err1 == ESP_OK && err2 == ESP_OK && err3 == ESP_OK)
    {
        astra_push_info_bar("PID saved !", 2000);
    }

    else{

        astra_push_info_bar("PID save failed!", 2000);
    }
    
}


void UI_task(void *arg) {

    while (1) {
      
      oled_clear_buffer();
      System_UI();
      astra_ui_main_core();
      astra_ui_widget_core();
      oled_send_buffer();
      vTaskDelay(pdMS_TO_TICKS(10)); // 延时 10 毫秒
    }
}

void UI_init(void){
  astra_list_item_t* setting_list_item = astra_new_list_item("Setup");

  astra_list_item_t* wifi_list_item = astra_new_list_item("Wifi");
  astra_list_item_t* ble_list_item = astra_new_list_item("Bluetooth");
  astra_list_item_t* temp_control_item = astra_new_list_item("Temperature Control"); 
  astra_list_item_t* about_list_item = astra_new_list_item("关于");
  astra_list_item_t* PID_list_item = astra_new_list_item("PID Setting");


  astra_push_item_to_list(astra_get_root_list(), setting_list_item);
  astra_push_item_to_list(setting_list_item, wifi_list_item);
  astra_push_item_to_list(setting_list_item, ble_list_item);
  astra_push_item_to_list(astra_get_root_list(), temp_control_item);
  astra_push_item_to_list(astra_get_root_list(), PID_list_item);
  astra_push_item_to_list(astra_get_root_list(), about_list_item);
  astra_push_item_to_list(wifi_list_item, astra_new_switch_item("Enable wifi",&wifi_enable ));
  astra_push_item_to_list(ble_list_item, astra_new_switch_item("Enable BLE",&ble_state ));
  astra_push_item_to_list(temp_control_item, astra_new_slider_item("Temperature", &ADC.set_temp,10,10,250));
  astra_push_item_to_list(PID_list_item, astra_new_slider_item("Kp", &pid.Kp,5,0,200));
  astra_push_item_to_list(PID_list_item, astra_new_slider_item("Ki", &pid.Ki,1,0,50));
  astra_push_item_to_list(PID_list_item, astra_new_slider_item("Kd", &pid.Kd,1,10,100));
  astra_push_item_to_list(PID_list_item, astra_new_user_item("Save config",init_save_config,NULL,NULL));
  astra_push_item_to_list(temp_control_item,astra_new_user_item("Temp plot",init_temp_plot,temp_plot,temp_plot_quit));
}