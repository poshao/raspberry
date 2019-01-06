#include "ssd1306.h"
#include <math.h>
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
    if(x>127 || y>63) return;
    page=y/8;
    y=y%8;
    bit<<=y;
    screen[page][x]|=bit;
}

/**
 * 绘制线段
 */
void drawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2){
    
}

/**
 * 绘制圆形
 */
void drawCircle(uint8_t x,uint8_t y,uint8_t r){
    int8_t p_x,p_y;
    float deg;
    for(deg=0;deg<3.1416/2;deg+=0.01){
        p_x=(uint8_t)(sin(deg)*r);
        p_y=(uint8_t)(cos(deg)*r);
        drawPoint(x+p_x,y+p_y);
        drawPoint(x+p_x,y-p_y);

        drawPoint(x-p_x,y+p_y);
        drawPoint(x-p_x,y-p_y);
        // drawPoint((uint8_t)(sin(deg)*r+x) , (uint8_t)(cos(deg)*r+y));
    }
    // for(p_x=0;p_x<=r;p_x++){
    //     p_y=sqrt(r*r-p_x*p_x);

    //     drawPoint(x+p_x,y+p_y);
    //     drawPoint(x+p_x,y-p_y);

    //     drawPoint(x-p_x,y+p_y);
    //     drawPoint(x-p_x,y-p_y);
    // }
}

/**
 * 绘制文字
 */
void drawString(uint8_t x,uint8_t y,uint8_t width,uint8_t height,char * text){

}

/**
 * 绘制图片
 */
void drawImage(uint8_t *data,uint8_t x,uint8_t y,uint8_t width,uint8_t height){

}

/**
 * 绘制条形码
 */
void drawBarcode(uint8_t type,uint8_t x,uint8_t y,char * text){

}

/**
 * 绘制二维码
 */
void drawQRcode(uint8_t x,uint8_t y,uint8_t pixel_width,uint8_t level){

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