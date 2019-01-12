/**
 * 使用BCM2835库操作 0.96 OLED 基础操作库
 * 2019/01/05 Byron Gong
 * 
 * 编译:
 *     gcc ./ssd1306.c -o oled -lbcm2835
 * 运行:
 *     sudo ./oled
 */
#include "ssd1306.h"

/**
 * 显示缓存
 */
uint8_t screen[SCREEN_ROWS/8][SCREEN_COLUMNS]={0};

/**
 * 写入一个命令
 */
void writeCommand(uint8_t cmd){
    uint8_t data[2]={0};
    data[0]=0x00;
    data[1]=cmd;
    bcm2835_i2c_write(data,2);
}

/**
 * 写入一个数据
 */
void writeData(uint8_t data){
    uint8_t temp[2]={0};
    temp[0]=0x40;
    temp[1]=data;
    bcm2835_i2c_write(temp,2);
}

/**
 * 初始I2C设定
 */
void initDevice(){
    bcm2835_i2c_set_baudrate(SSD1306_I2C_RATE);
    bcm2835_i2c_setSlaveAddress(SSD1306_I2C_ADDR);
}

/** 通讯设定 ******************************************************************/

/**
 * 打开屏幕
 */
void openScreen(){
    //设置电荷泵
    writeCommand(CMD_POWER_CHARGE_PUMP);
    writeCommand(CHARGE_PUMP_ON);
    writeCommand(CMD_DISPLAY_ON);
}

/**
 * 关闭屏幕
 */
void closeScreen(){
    writeCommand(CMD_POWER_CHARGE_PUMP);
    writeCommand(CHARGE_PUMP_OFF);
    writeCommand(CMD_DISPLAY_OFF);
}

/**
 * 设置渲染模式
 * 
 * 参数:
 *      mode ==> MODE_HORIZONTAL(水平),MODE_VERTICAL(垂直),MODE_PAGE(页 默认值)
 */
void setRenderMode(uint8_t mode){
    writeCommand(CMD_ADDR_MODE);
    writeCommand(mode);
}

/**
 * 设置可编辑区域(仅适用与水平和垂直模式)
 * 
 * 参数:
 *      start_page : 编辑区域的页起始地址,取值范围 0-7 默认为0
 *      end_page : 编辑区域的页结束地址,取值范围 0-7 默认为7
 *      start_col : 编辑区域的列起始地址,取值范围 0-127 默认为0
 *      end_col : 编辑区域的列结束地址,取值范围 0-127 默认为127
 */
void setRange(uint8_t start_page,uint8_t end_page,uint8_t start_col,uint8_t end_col){
    writeCommand(CMD_ADDR_PAGE_RANGE);
    writeCommand(start_page);
    writeCommand(end_page);
    writeCommand(CMD_ADDR_COL_RANGE);
    writeCommand(start_col);
    writeCommand(end_col);
}

/**
 * 设置编辑页的起始位置(仅适用页模式)
 * 
 * 参数:
 *      page : 页序号,取值范围 0-7 默认为0
 *      col : 列序号,取值范围 0-127 默认为0
 */
void setPos(uint8_t page,uint8_t col){
    writeCommand(CMD_ADDR_PAGE_START_MASK | page);
    writeCommand(CMD_ADDR_COL_START_LOW_MASK | (col & 0x0F));
    writeCommand(CMD_ADDR_COL_START_HIGH_MASK | ((col & 0xF0)>>4));
}

/**
 * 设置对比度
 * 
 * 参数:
 *      level: 值越大 越清晰 默认0x7F
 */
void setContrast(uint8_t level){
    writeCommand(CMD_DISPLAY_CONTRAST);
    writeCommand(level);
}

/**
 * 设置供电电压
 * 
 * 参数:
 *      voltage : 电压值为n * Vcc,可选参数VOLTAGE_0_DOT_65X,VOLTAGE_0_DOT_77X(默认),VOLTAGE_0_DOT_83X
 */
void setVoltage(uint8_t voltage){
    writeCommand(CMD_POWER_VOLTAGE);
    writeCommand(voltage);
}

/**
 * 忽略内训显示
 * 
 * 参数:
 *      flag : SSD1306_FALSE ==> 显示内存数据, SSD1306_TRUE ==>全屏显示
 */
void setIgnoreRAM(uint8_t flag){
    if(flag){
        writeCommand(CMD_DISPLAY_ALL);
    }else{
        writeCommand(CMD_DISPLAY_RAM);
    }
}

/**
 * 设置显示翻转
 * 
 * 参数:
 *      h : SSD1306_TRUE ==> 左右翻转
 *      v : SSD1306_TRUE ==> 上下翻转
 */
void setReverse(uint8_t h,uint8_t v){
    if(v){
        h=h?0:1;
        writeCommand(CMD_HARD_SCAN_DIRECT_INVERSE);
    }else{
        writeCommand(CMD_HARD_SCAN_DIRECT_NORMAL);
    }
    if(h){
        writeCommand(CMD_HARD_MAP_COL_127);
    }else{
        writeCommand(CMD_HARD_MAP_COL_0);
    }
}

/**
 * 设置内存映射
 * 
 *  灯序号 行序号(抽象编号) 内存序号
 * 
 *  行序号=(内存序号+start_line) % 64
 *  灯序号=(行序号+offset) % 64
 *  行序号<rows 的为有效数据,其余数据全部显示黑色
 * 
 *  灯序号=((内存序号 + start_line) % 64 + offset) % 64
 *  (内存数据循环上移动start_line后获取前rows行数据,再上移offset)
 * 参数:
 *      start_line : 内存相对行号的偏移量,取值范围0-63,默认为0
 *      offset : 显示相对行号的偏移量,取值范围0-63,默认为0
 *      rows : 有效的行号上限(16-64) 默认 64
 */
void setMapping(uint8_t start_line,uint8_t offset,uint8_t rows){
    writeCommand(CMD_HARD_START_LINE_MASK | start_line);
    writeCommand(CMD_HARD_VERTICAL_OFFSET);
    writeCommand(offset);
    writeCommand(CMD_HARD_MUX);
    writeCommand(rows-1);
}

/**
 * 设置显示反色
 */
void setPointInvert(uint8_t flag){
    if(flag){
        writeCommand(CMD_DISPLAY_INVERSE);
    }else{
        writeCommand(CMD_DISPLAY_NORMAL);
    }
}

/**
 * 设置频率
 * 
 * 参数:
 *      rate : 频率设定,值越大频率越高,取值范围 0-15,默认为8
 *      div : 分频系数,取值范围 1-16,默认为1
 */
void setFrequency(uint8_t rate,uint8_t div){
    writeCommand(CMD_TIME_CLOCK);
    writeCommand(((div-1) & 0x0F) | ((rate & 0x0F)<<4));
}

/**
 * 设置供电时间分配
 * 
 * 参数:
 *      phase1: 第一阶段指令周期数,取值范围1-15,默认2
 *      phase2: 第一阶段指令周期数,取值范围1-15,默认2
 */
void setPeriodPreCharge(uint8_t phase1,uint8_t phase2){
    writeCommand(CMD_POWER_PRECHARGE);
    writeCommand(((phase2 & 0x0F)<<4) | (phase1 & 0x0F));
}

/**
 * 设置缩放特效
 * 
 * 参数:
 *      flag : SSD1306_TRUE 启用缩放特性
 */
void setGraphicZOOM(uint8_t flag){
    writeCommand(CMD_GRAPHIC_ZOOM);
    flag=flag?ZOOM_ON:ZOOM_OFF;
    writeCommand(flag);
}

/**
 * 设置渐隐特效
 * 
 * 参数:
 *      mode: FADE_OFF,FADE_ONCE,FADE_LOOP 默认为 FADE_OFF
 *      frame: FADE_FRAME_8
 */
void setGraphicFade(uint8_t mode,uint8_t frame){
    writeCommand(CMD_GRAPHIC_FADE);
    writeCommand(mode | frame);
}

/**
 * 设置水平滚动特效
 * 
 * 参数:
 *      direct : 方向 SCROLL_DIRECT_LEFT
 *      start_page : 开始页
 *      end_page : 结束页
 *      frame : 帧数
 */
void setGraphicScroll_H(uint8_t direct,uint8_t start_page,uint8_t end_page,uint8_t frame){
    writeCommand(direct);
    writeCommand(0x00);
    writeCommand(start_page);
    writeCommand(frame);
    writeCommand(end_page);
    writeCommand(0x00);
    writeCommand(0xFF);
}

/**
 * 设置对角线滚动特效
 * 
 * 参数:
 *      direct : 方向 CMD_SCROLL_DOWN_LEFT
 *      start_page : 开始页
 *      end_page : 结束页
 *      frame : 帧数
 *      offset: 偏移行数
 */
void setGraphicScroll_HV(uint8_t direct,uint8_t start_page,uint8_t end_page,uint8_t frame,uint8_t offset){
    writeCommand(direct);
    writeCommand(0x00);
    writeCommand(start_page);
    writeCommand(frame);
    writeCommand(end_page);
    writeCommand(offset);
}

/**
 * 设置垂直滚动区域
 * 
 * 参数:
 *      fix_rows : 固定的行数
 *      scroll_rows : 滚动的行数
 */
void setGraphicScrollRange_V(uint8_t fix_rows,uint8_t scroll_rows){
    writeCommand(CMD_SCROLL_DOWN_AREA);
    writeCommand(fix_rows);
    writeCommand(scroll_rows);
}

/**
 * 启动滚动特效
 */
void setGraphicScrollEnable(){
    writeCommand(CMD_SCROLL_ENABLE);
}

/**
 * 关闭滚动特效
 */
void setGraphicScrollDisable(){
    writeCommand(CMD_SCROLL_DISABLE);
}

/**
 * 引脚配置
 * 
 * 参数:
 *      alternative: SSD1306_TRUE ==>交替 ,SSD1306_FALSE ==> 连续 ,默认SSD1306_TRUE
 *      remap: SSD1306_TRUE ==> 左右交换,默认SSD1306_FALSE
 */
void setPinConfig(uint8_t alternative,uint8_t remap){
    uint8_t cmd=0x02;
    cmd|=alternative?0x10:0x00;
    cmd|=remap?0x20:0x00;

    writeCommand(CMD_HARD_COM_CONFIG);
    writeCommand(cmd);
}

/**
 * 更新屏幕区域
 */
void updateScreenRange(uint8_t data[][128],uint8_t start_page,uint8_t end_page,uint8_t start_col,uint8_t end_col){
    uint8_t r,c;
    setRenderMode(MODE_HORIZONTAL);
    setRange(start_page,end_page,start_col,end_col);
    for(r=start_page;r<=end_page;r++)
        for(c=start_col;c<=end_col;c++){
            writeData(data[r][c]);
        }
}

/**
 * 更新屏幕数据
 */
void updateScreen(uint8_t data[][128]){
    updateScreenRange(data,0,7,0,127);
}

/**
 * 清空屏幕
 */
void cleanScreen(){
    uint8_t i,j;
    for(i=0;i<8;i++)
        for(j=0;j<128;j++){
            screen[i][j]=0x00;
        }
}
// void drawPoint(uint8_t x,uint8_t y){
//     setRenderMode(MODE_PAGE);
//     setPos(x/8,y);
//     writeData(0xFF);
// }

/**
 * 初始化设置
 */
void reset(){
    //关闭屏幕
    closeScreen();

    //清空屏幕
    updateScreen(screen);

    //设置扫描方向
    setReverse(SSD1306_FALSE,SSD1306_TRUE);

    //设置映射关系
    setMapping(0,0,64);

    //设置对比度
    setContrast(127);

    //设置显示模式
    setPointInvert(SSD1306_FALSE);

    //显示内存信息
    setIgnoreRAM(SSD1306_FALSE);

    //设置分频率
    setFrequency(8,1);
    // setFrequency(8,8);
    //设置电源
    setPeriodPreCharge(2,2);

    //设置输出引脚
    setPinConfig(SSD1306_TRUE,SSD1306_FALSE);

    //设置电压
    setVoltage(VOLTAGE_0_DOT_77X);

    //设置特效关闭
    setGraphicScrollDisable();
    setGraphicFade(FADE_OFF,FADE_FRAME_8);
    setGraphicZOOM(SSD1306_FALSE);
}

/**
 * 示例代码
 */
void sample(){
    uint8_t i,j;
    initDevice();
    reset();
    openScreen();

    // for(i=0;i<64;i++){
    //     drawPoint(i,0);
    //     // bcm2835_delay(10);
    //     updateScreen(screen);
    // }
    // for(i=0;i<64;i++){
    //     drawPoint(63,i);
    //     // bcm2835_delay(10);
    //     updateScreen(screen);
    // }
    // for(i=0;i<64;i++){
    //     clearPoint(i,0);
    //     // bcm2835_delay(10);
    //     updateScreen(screen);
    // }
    // for(i=0;i<64;i++){
    //     clearPoint(63,i);
    //     // bcm2835_delay(10);
    //     updateScreen(screen);
    // }
    // drawRectangle(0,0,2,2);
    // drawRectangle(2,2,10,10);

    // for(i=0;i<100;i++){
    // cleanScreen();

    // drawCircle(63,31,0+i);
    // drawCircle(63,31,10+i);
    // drawCircle(63,31,20+i);
    // drawCircle(63,31,30+i);
    // drawCircle(63,31,40+i);
    // drawCircle(63,31,50+i);
    // drawCircle(63,31,60+i);
    // drawCircle(63,31,70+i);
    // if(i==10) i=0;

    // updateScreen(screen);
    // bcm2835_delay(50);
    // }
    drawCircle(64,32,25);
    drawLine(0,0,127,63);
    drawLine(0,63,127,0);
    drawRectangle(20,10,20,5);
    drawPolygon(3,64,32,10);
    drawPolygon(4,64,32,15);
    drawPolygon(5,64,32,20);
    drawPolygon(6,64,32,25);
    // drawLine(64,52,81,22);
    // drawLine(81,22,46,22);
    // drawLine(46,22,64,52);

    // drawQRcode("hello12345",10,10,0,0);
    updateScreen(screen);
    // closeScreen();
}

int main(int argc,char** argv){
    bcm2835_init();
    bcm2835_i2c_begin();

    sample();
    if(argc>1){
        drawQRcode(argv[1],10,10,0,0);
        updateScreen(screen);
    }

    if(argc==2) return 0;
    bcm2835_i2c_end();
    bcm2835_close();
}