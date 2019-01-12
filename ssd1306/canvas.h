/**
 * 简易绘图库
 */

#ifndef __SSD1306_CANVAS__
#define __SSD1306_CANVAS__

#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include "qrcode/qr_encode.h"

#define SCREEN_ROWS 64
#define SCREEN_COLUMNS 128

/**
 * 显示缓存
 */
extern uint8_t screen[SCREEN_ROWS/8][SCREEN_COLUMNS];
/**
 * 更新屏幕
 */
extern void updateScreen(uint8_t data[][SCREEN_COLUMNS]);


/**
 * 清空一个点
 */
void clearPoint(uint8_t x,uint8_t y);
/**
 * 绘制一个点
 */
void drawPoint(uint8_t x,uint8_t y);
/**
 * 绘制一个矩形
 */
void drawRectangle(uint8_t x,uint8_t y,uint8_t width,uint8_t height);
/**
 * 绘制圆形
 */
void drawCircle(uint8_t x,uint8_t y,uint8_t r);
/**
 * 绘制直线
 */
void drawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2);
/**
 * 绘制圆内接正多边形
 */
void drawPolygon(uint8_t c,uint8_t x,uint8_t y,uint8_t r);

void drawQRcode(char* content,uint8_t x,uint8_t y,uint8_t pixel_width,uint8_t level);
#endif