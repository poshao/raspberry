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
    float a,l,i;
    double deg;

    // a=1.0f*(x2-x1)/(y2-y1);
    // deg=atan(a);
    deg=atan2(y2-y1,x2-x1);
    l=sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
    // a=(y2-y1)/l;
    // deg=asin(a);
    printf("point: x1:%d, y1:%d, x2:%d, y2:%d, deg:%f, l:%f \n",x1,y1,x2,y2,deg/M_PI*180,l);
    // printf("point: x1:%d, y1:%d, x2:%d, y2:%d, a:%f, b:%f \n",x1,y1,x2,y2,a,b);
    // if(fabs(x1)>fabs(x2)){
    //     x1=x1+x2;
    //     x2=x1-x2;
    //     x1=x1-x2;
    // }
    // if(fabs(y1)>fabs(x2)){
    //     y1=y1+y2;
    //     y2=y1-y2;
    //     y1=y1-y2;
    // }

    for(i=0;i<=l;i+=0.5){
        drawPoint(x1+i*cos(deg),y1+i*sin(deg));
        printf("%f %f\n",x1+i*cos(deg),y1+i*sin(deg));
        // drawPoint(i,a*i+b);
    }
}
/**
 * 绘制圆形
 */
void drawCircle(uint8_t x,uint8_t y,uint8_t r){
    int8_t p_x,p_y;
    float deg;
    for(deg=0;deg<M_PI_2;deg+=0.01){
        p_x=(uint8_t)(cos(deg)*r);
        p_y=(uint8_t)(sin(deg)*r);
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

/**
 * 绘制内切正多边形
 */
void drawPolygon(uint8_t c,uint8_t x,uint8_t y,uint8_t r){
    float x1,y1,x2,y2;
    uint8_t i;
    x1=x;
    y1=y+r;
    for(i=1;i<c;i++){
        x2=r*sin(2*M_PI/c*i)+x;
        y2=r*cos(2*M_PI/c*i)+y;
        drawLine(x1,y1,x2,y2);
        printf("%d %f,%f,%f,%f\n",i,x1,y1,x2,y2);
        x1=x2;
        y1=y2;
    }
    drawLine(x1,y1,x,y+r);
    printf("%f,%f,%f,%f\n",x1,y1,x,y+r);
}