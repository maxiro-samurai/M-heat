#include "externdraw.h"



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