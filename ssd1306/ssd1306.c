#include <bcm2835.h>
#include <stdio.h>

//调试模式
#define SSD1306_DEBUG
#define SSD1306_I2C_ADDR 0x3C

void writeCommand(uint8_t cmd){
    uint8_t data[2]={0};
    data[0]=0x40;
    data[1]=cmd;
    bcm2835_i2c_write(data,2);
}

void writeData(uint8_t data){
    uint8_t temp[2]={0};
    temp[0]=0x00;
    temp[1]=data;
    bcm2835_i2c_write(temp,2);
}

int main(void){

}