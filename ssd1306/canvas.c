#include "ssd1306.h"
// extern uint8_t screen[8][128];
/**
 * 清除一个点
 */
void clearPoint(uint8_t x,uint8_t y){
    uint8_t page,bit=1;

    page=y/8;
    y=y%8;
    bit<<=y;
    screen[page][x]&=~bit;
}

/**
 * 绘制一个点
 */
void drawPoint(uint8_t x,uint8_t y){
    uint8_t page,bit=1;

    page=y/8;
    y=y%8;
    bit<<=y;
    screen[page][x]|=bit;
}

/**
 * 绘制矩形
 */
void drawRectangle(uint8_t x,uint8_t y,uint8_t width,uint8_t height){
    uint8_t i;
    for(i=0;i<width;i++){
        drawPoint(x+i,y);
        drawPoint(x+i,y+height-1);
    }
    for(i=0;i<height;i++){
        drawPoint(x,y+i);
        drawPoint(x+width-1,y+i);
    }
}