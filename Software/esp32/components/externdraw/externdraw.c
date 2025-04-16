#include "externdraw.h"


uint8_t* C_table[] = {c1, c2, c3, Lightning, c5, c6, c7};
// char* TempCTRL_Status_Mes[] = {
//     "错误",
//     "停机",
//     "休眠",
//     "提温",
//     "正常",
//     "加热",
//     "维持",
// };





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
    // if (TipTemperature <= TipMaxTemp)
    oled_draw_box(0, 53, map(58, 0, 300, 5, 98), 11);

    //功率条
    oled_draw_frame(104, 53, 23, 11);
    oled_draw_box(104, 53, map(255, 0, 255, 0, 23), 11);

    oled_draw_H_line(117, 51, 11);
    oled_draw_pixel(103, 52);
    oled_draw_pixel(127, 52);

    //////////////进入反色////////////////////////////////
    // 进入反色模式
    oled_set_draw_color(2);

    // 画指示针
    Draw_Slow_Bitmap(map(250, 0, 300, 5, 98) - 4, 54, PositioningCursor, 8, 8);

    // // 切换回正常颜色模式
    // oled_set_draw_color(0);
    
    // 显示温度值
    sprintf(buff, "%03d", 26);
    oled_draw_UTF8(2, 62, buff);

    // 显示功率值
    sprintf(buff, "%d", 100);
    oled_draw_UTF8(105, 62, buff);

    // 刷新显示缓冲区
    oled_send_buffer();
    



    // arduboy.setCursor(105, 55); arduboy.print(map(PID_Output, 255, 0, 0, 100)); arduboy.print(F("%")); //功率百分比
    oled_set_draw_color(color);
}

void System_UI(void)
{
    char temp[20];
    int font_height = oled_get_str_height(); // 获取字体高度

    oled_draw_UTF8(0, font_height-1, "M-heat");
    //测试图标绘制函数
    // for (int i=0;i<7;i++){
    //     oled_clear_buffer();
    //     Draw_Slow_Bitmap(74, 37, C_table[i], 14, 14);
    //     oled_send_buffer();
    //     vTaskDelay(500 / portTICK_PERIOD_MS);
    // }
    //显示中文状态信息
    // oled_set_font(u8g2_font_my_chinese);
    // oled_draw_UTF8(91, 40, "关于");

    //蓝牙显示
    Draw_Slow_Bitmap(92, 25, IMG_BLE_S, 9, 11);


    //主页面显示温度
    oled_set_font(u8g2_font_logisoso38_tr);


    sprintf(temp,"%.1f", 12.1);
    oled_draw_str(0, 51, temp);
    oled_set_font(u8g2_font_my_chinese);

    //右上角运行指示角标
    uint8_t TriangleSize = map(100, 0, 255, 16, 0);
    
    oled_draw_triangle((119 - 12) + TriangleSize, 12, 125, 12, 125, (18 + 12) - TriangleSize);


    /////////////////////////////////////绘制遮罩层//////////////////////////////////////////////
    oled_set_draw_color(2);
    //几何图形切割
    oled_draw_box(0, 12, 96, 40);
    oled_draw_triangle(96, 12, 96, 52, 125, 42);
    oled_draw_triangle(125, 42, 96, 52, 118, 52);
    oled_set_draw_color(1);
    
    DrawStatusBar(1);
}   
