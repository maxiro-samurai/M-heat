#ifndef EXTERNDRAW_H
#define EXTERNDRAW_H



#include "astra_ui_drawer.h"
#include "bitmap.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

#define pgm_read_byte(addr)   (*(const unsigned char *)(addr))



void Draw_Num_Bar(float i, float a, float b, int x, int y, int w, int h, int c);
void Draw_Slow_Bitmap(int x, int y, const unsigned char* bitmap, unsigned char w, unsigned char h);
void System_UI(void);






#endif