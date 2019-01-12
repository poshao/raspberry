#include "canvas.h"

/**
 * 内部变量
 */

/**
 * 前景色(白色)
 */
uint8_t m_forecolor =1;

/**
 * 背景色(黑色)
 */
uint8_t m_backcolor =0;

/**
 * 线条宽度
 */
uint8_t m_lineWidth=1;

/**
 * 角度递增单位(主要用于极坐标绘图)
 */
float m_resolutionAngle=0.01;

/**
 * 长度递增单位(主要用于极坐标绘图)
 */
float m_resolutionLength=0.5;

/**
 * 绘制一个点
 */
void drawPoint(uint8_t x,uint8_t y){
    uint8_t page,bit=1;
    if(x>127 || y>63) return;
    page=y/8;
    y=y%8;
    bit<<=y;
    if(m_forecolor){
        screen[page][x]|=bit;
    }else{
        screen[page][x]&=~bit;
    }
}

/**
 * 绘制线段
 */
void drawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2){
    float l,i,j;
    double deg;

    // a=1.0f*(x2-x1)/(y2-y1);
    // deg=atan(a);
    deg=atan2(y2-y1,x2-x1);
    l=sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
    // a=(y2-y1)/l;
    // deg=asin(a);
    // printf("point: x1:%d, y1:%d, x2:%d, y2:%d, deg:%f, l:%f \n",x1,y1,x2,y2,deg/M_PI*180,l);
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

    for(i=0;i<=l;i+=m_resolutionLength){
        for(j=0;j<m_lineWidth;j+=m_resolutionLength){
            drawPoint(x1+i*cos(deg)+j*cos(deg-M_PI_2),y1+i*sin(deg)+j*sin(deg-M_PI_2));
            // printf("%f %f\n",x1+i*cos(deg),y1+i*sin(deg));
            // drawPoint(i,a*i+b);
        }
        // drawPoint(x1+i*cos(deg)+j*cos(deg-M_PI_2),y1+i*sin(deg)+j*sin(deg-M_PI_2));
    }
}
/**
 * 绘制圆形
 */
void drawCircle(uint8_t x,uint8_t y,uint8_t r){
    int8_t p_x,p_y;
    float deg;
    for(deg=0;deg<M_PI_2;deg+=m_resolutionAngle){
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
void drawQRcode(char* content,uint8_t x,uint8_t y,uint8_t pixel_width,uint8_t level){
    uint8_t bitdata[QR_MAX_BITDATA]={0};
    int side,a;
    uint8_t i,j; 
    side=qr_encode(QR_LEVEL_H,0,content,0,bitdata);

    // for (i = 0; i < side + 2; i++) printf("██");
	// printf("\n");
    m_forecolor=1;
    drawRectangle(x,y,side+2*m_lineWidth-1,side+2*m_lineWidth-1);
	for (i = 0; i < side; i++) {
		// printf("██");
		for (j = 0; j < side; j++) {
			a = i * side + j;
            if(bitdata[a / 8] & (1 << (7 - a % 8))){
                m_forecolor=0;
            }else{
                m_forecolor=1;
            }
            drawPoint(x+j+m_lineWidth,y+i+m_lineWidth);
			// printf((bitdata[a / 8] & (1 << (7 - a % 8))) ? "  " : "██");
		}
		// printf("██");
		// printf("\n");
	}
	// for (i = 0; i < side + 2; i++) printf("██");
	// printf("\n");

}


/**
 * 绘制矩形
 */
void drawRectangle(uint8_t x,uint8_t y,uint8_t width,uint8_t height){
    float old_resolutionLength=m_resolutionLength;
    uint8_t x2=x+width,y2=y+height;

    m_resolutionLength=1.0;
    // //顺时针
    // drawLine(x,y,x2,y);
    // drawLine(x2,y,x2,y2);
    // drawLine(x2,y2,x,y2);
    // drawLine(x,y2,x,y);

    //逆时针
    drawLine(x,y,x,y2);
    drawLine(x,y2,x2,y2);
    drawLine(x2,y2,x2,y);
    drawLine(x2,y,x,y);
    m_resolutionLength=old_resolutionLength;
    // uint8_t i;
    // for(i=0;i<width;i++){
    //     drawPoint(x+i,y);
    //     drawPoint(x+i,y+height-1);
    // }
    // for(i=0;i<height;i++){
    //     drawPoint(x,y+i);
    //     drawPoint(x+width-1,y+i);
    // }
}

/**
 * 绘制内切正多边形
 */
void drawPolygon(uint8_t c,uint8_t x,uint8_t y,uint8_t r){
    float x1,y1,x2,y2;
    uint8_t i;
    x1=x;
    y1=y+r;
    for(i=0;i<c;i++){
        x2=r*sin(-2*M_PI/c*i)+x;
        y2=r*cos(-2*M_PI/c*i)+y;
        drawLine(x1,y1,x2,y2);
        printf("%d %f,%f,%f,%f\n",i,x1,y1,x2,y2);
        x1=x2;
        y1=y2;
    }
    drawLine(x1,y1,x,y+r);
    printf("%f,%f,%f,%f\n",x1,y1,x,y+r);
}