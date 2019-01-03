/**
 * 使用BCM2835库操作BOSCH的温湿度压强三合一传感器
 * 2010/01/02 Byron Gong
 * 
 * 编译:
 *     gcc ./bme280.c -o bme280 -lbcm2835
 * 运行:
 *     sudo ./bme280
 */
#include <bcm2835.h>
#include <stdio.h>

//调试模式
// #define BME280_DEBUG
//I2C通讯地址
#define BME280_I2C_ADDR 0x76

//采样率设置
#define OVERSAMPLING_NONE 0x00
#define OVERSAMPLING_1X   0x01
#define OVERSAMPLING_2X   0x02
#define OVERSAMPLING_4X   0x03
#define OVERSAMPLING_8X   0x04
#define OVERSAMPLING_16X  0x05

//模式设定
#define MODE_SLEEP  0x00
#define MODE_NORMAL 0x03
#define MODE_FORCE  0x01

//时间间隔
#define TSB_0_DOT_5MS   0x00
#define TSB_62_DOT_5MS  0x01
#define TSB_125MS       0x02
#define TSB_250MS       0x03
#define TSB_500MS       0x04
#define TSB_1000MS      0x05
#define TSB_10MS        0x06
#define TSB_20MS        0x07

//过滤器
#define FILTER_COEFFICIENT_NONE 0x00
#define FILTER_COEFFICIENT_2    0x01
#define FILTER_COEFFICIENT_4    0x02
#define FILTER_COEFFICIENT_8    0x03
#define FILTER_COEFFICIENT_16   0x04

/**
 * NVM数据结构
 */
typedef struct {
    uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;
	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;
	uint8_t  dig_H1;
	int16_t dig_H2;
	uint8_t  dig_H3;
	int16_t dig_H4;
	int16_t dig_H5;
	int8_t  dig_H6;
	int32_t t_fine;
} BME280_NVM;

/**
 * 传感器数据
 */
typedef struct{
    int32_t press;
    int32_t temp;
    int32_t hum;
} BME280_DATA;

/**
 * 配置参数
 */
uint8_t ctrl_hum=0;
uint8_t ctrl_meas=0;
uint8_t config=0;

/**
 * 写入一个字节
 */
void writeByte(uint8_t reg_addr,uint8_t data){
    uint8_t d[2]={reg_addr,data};
    bcm2835_i2c_write(d,2);
}

/**
 * 读取一个字节
 */
uint8_t readByte(uint8_t reg_addr){
    uint8_t data;
    bcm2835_i2c_write(&reg_addr,1);
    bcm2835_i2c_read(&data,1);
    return data;
}

/**
 * 连续读取指定长度
 */
void read(uint8_t reg_addr,uint8_t * data, uint8_t len){
    bcm2835_i2c_write(&reg_addr,1);
    bcm2835_i2c_read(data,len);
}

/**
 * 初始化通讯设置
 */
void initDevice(){
    bcm2835_i2c_set_baudrate(100000);
    bcm2835_i2c_setSlaveAddress(BME280_I2C_ADDR);
}

/**
 * 计算温度
 * 
 * "5123" ==> 51.23DegC
 */
int32_t calcTemp(BME280_DATA* data,BME280_NVM* nvm){
    int32_t v1,v2;
    v1 = ((((data->temp >> 3) - ((int32_t)nvm->dig_T1 << 1))) * ((int32_t)nvm->dig_T2)) >> 11;
    v2 = (((((data->temp >> 4) - ((int32_t)nvm->dig_T1)) * ((data->temp >> 4) - ((int32_t)nvm->dig_T1))) >> 12) * ((int32_t)nvm->dig_T3)) >> 14;
    nvm->t_fine = v1 + v2;
    return (nvm->t_fine * 5 + 128) >> 8;
}

/**
 * 计算压强
 * 
 * "24674867" ==> 24674867/256 Pa ==> 96386.2 Pa ==> 963.862 hPa
 */
uint32_t calcPress(BME280_DATA* data,BME280_NVM* nvm){
    int64_t v1,v2,p;
    v1 = ((int64_t)nvm->t_fine) - 128000;
    v2 = v1 * v1 * (int64_t)nvm->dig_P6;
    v2 = v2 + ((v1 * (int64_t)nvm->dig_P5) << 17);
    v2 = v2 + (((int64_t)nvm->dig_P4) << 35);
    v1 = ((v1 * v1 * (int64_t)nvm->dig_P3) >> 8) + ((v1 * (int64_t)nvm->dig_P2) << 12);
    v1 = (((((int64_t)1) << 47) + v1)) * ((int64_t)nvm->dig_P1) >> 33;
    if(v1 == 0){
        return 0;
    }
    p = 1048576 - data->press;
    p = (((p << 31) - v2) * 3125) / v1;
    v1 = (((int64_t)nvm->dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    v2 = (((int64_t)nvm->dig_P8) * p) >> 19;
    p = ((p + v1 + v2) >> 8) + (((int64_t)nvm->dig_P7) << 4);
    return (uint32_t)p;
}

/**
 * 计算湿度
 * 
 * "47445" ==> 47445/1024 ==> 46.333 %RH
 */
uint32_t calcHum(BME280_DATA* data,BME280_NVM* nvm){
    int32_t v;
    v = (nvm->t_fine - ((int32_t)76800));
    v = (((((data->hum << 14) - (((int32_t)nvm->dig_H4) << 20) - (((int32_t)nvm->dig_H5) * v)) + 
        ((int32_t)16384)) >> 15) * (((((((v * ((int32_t)nvm->dig_H6)) >> 10) * (((v * ((int32_t)nvm->dig_H3)) >> 11) + 
        ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)nvm->dig_H2) + 8192) >> 14));
    v = (v - (((((v >> 15) * (v >>15)) >> 7) * ((int32_t)nvm->dig_H1)) >> 4));
    v = (v < 0 ? 0 : v);
    v = (v > 419430400 ? 419430400 : v);
    return (uint32_t)(v >> 12);
}

/**
 * 计算温度
 */
double calcTemp_double(BME280_DATA* data,BME280_NVM* nvm){
    double v1,v2;
    v1 = (((double)data->temp) / 16384.0 - ((double)nvm->dig_T1) / 1024.0) * ((double)nvm->dig_T2);
    v2 = ((((double)data->temp) / 131072.0 - ((double)nvm->dig_T1) / 8192.0) * (((double)data->temp) / 131072.0 - ((double)nvm->dig_T1) / 8192.0)) * ((double)nvm->dig_T3);
    nvm->t_fine = (int32_t)(v1 + v2);
    return (v1 + v2) / 5120.0;
}

/**
 * 计算压强
 */
double calcPress_double(BME280_DATA* data,BME280_NVM* nvm){
    double v1,v2,p;
    v1 = ((double)nvm->t_fine / 2.0) - 64000.0;
    v2 = v1 * v1 * ((double)nvm->dig_P6) / 32768.0;
    v2 = v2 + v1 * ((double)nvm->dig_P5) * 2.0;
    v2 = (v2 / 4.0) + (((double)nvm->dig_P4) * 65536.0);
    v1 = (((double)nvm->dig_P3) * v1 * v1 / 524288.0 + ((double)nvm->dig_P2) * v1) / 524288.0;
    v1 = (1.0 + v1 / 32768.0) * ((double)nvm->dig_P1);
    if(v1 == 0.0){
        return 0;
    }
    p = 1048576.0 - (double)data->press;
    p = (p - (v2 / 4096.0)) * 6250.0 / v1;
    v1 = ((double)nvm->dig_P9) * p * p / 2147483648.0;
    v2 = p * ((double)nvm->dig_P8) / 32768.0;
    p = p + (v1 + v2 + ((double)nvm->dig_P7)) / 16.0;
    return p;
}

/**
 * 计算湿度
 */
double calcHum_double(BME280_DATA* data,BME280_NVM* nvm){
    double h;
    h = (((double)nvm->t_fine) - 76800.0);
    h = (data->hum - (((double)nvm->dig_H4) * 64.0 + ((double)nvm->dig_P5) / 16384.0 * h)) * (((double)nvm->dig_H2) / 65536.0 * (1.0 + ((double)nvm->dig_H6) / 67108864.0 * h * (1.0 + ((double)nvm->dig_H3) / 67108864.0 * h)));
    h = h * (1.0 - ((double)nvm->dig_H1) * h / 524288.0);
    if(h > 100.0){
        h=100.0;
    }else if(h < 0.0){
        h=0.0;
    }
    return h;
}

/**
 * 计算压强
 * 
 * "96386" ==> 96386 Pa ==> 963.86 hPa
 */
uint32_t calcPress_int32(BME280_DATA* data,BME280_NVM* nvm){
    int32_t v1,v2;
    uint32_t p;
    v1 = (((int32_t)nvm->t_fine) >> 1) - (int32_t)64000;
    v2 = (((v1 >> 2) * (v1 >> 2)) >> 11) * ((int32_t)nvm->dig_P6);
    v2 = v2 + ((v1 * ((int32_t)nvm->dig_P5)) << 1);
    v2 = (v2 >> 2) + (((int32_t)nvm->dig_P4) << 16);
    v1 = (((nvm->dig_P3 * (((v1 >> 2) * (v1 >>2)) >> 13)) >> 3) + ((((int32_t)nvm->dig_P2) * v1) >> 1)) >> 18;
    v1 = ((((32768 + v1)) * ((int32_t)nvm->dig_P1)) >> 15);
    if(v1 == 0){
        return 0;
    }
    p = (((uint32_t)(((int32_t)1048576) - data->press) - (v2 >> 12))) * 3125;
    if(p < 0x80000000){
        p = (p << 1) / ((uint32_t)v1);
    }else{
        p = (p / (uint32_t)v1) * 2;
    }
    v1 = (((int32_t)nvm->dig_P9) * ((int32_t)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
    v2 = (((int32_t)(p >> 2)) * ((int32_t)nvm->dig_P8)) >> 13;
    p = (uint32_t)((int32_t)p + ((v1 + v2 + nvm->dig_P7) >> 4));
    return p;
}

/**
 * 重置设备
 */
void reset(){
    writeByte(0xE0,0xB6);
}

/**
 * 获取设备ID
 */
uint8_t getId(){
    return readByte(0xD0);
}

/**
 * 获取校准数据(42字节)
 */
void getCalibData(BME280_NVM *nvm){
    uint8_t data[42]={0};
    uint8_t * p;
    uint8_t i;

    read(0x88,data,26);
    read(0xE1,&data[26],16);

#ifdef BME280_DEBUG
    printf("[debug] calib data:\n");
    for(i=0;i<42;i++){
        printf("    %d => 0x%02x\n",i,data[i]);
    }
#endif

    p=(uint8_t*)&nvm->dig_T1;
    for(i=0;i<24;i++){
        *p++=data[i];
    }
    nvm->dig_H1=data[25];
    p=(uint8_t*)&nvm->dig_H2;
    *p++=data[26];
    *p=data[27];
    nvm->dig_H3=data[28];

    p=(uint8_t*)&nvm->dig_H4;
    *p++=(data[29] & 0x0F)<<4 | data[30] & 0x0F;
    *p=(data[29] & 0xF0)>>4;

    p=(uint8_t*)&nvm->dig_H5;
    *p++=(data[30] & 0xF0)>>4 | (data[31] & 0x0F)<<4;
    *p=(data[31] & 0xF0)>>4;

    p=(uint8_t*)&nvm->dig_H6;
    *p=data[32];
}

/**
 * 获取传感器数据(8字节)
 */
void getData(BME280_DATA *data){
    uint8_t senser[8]={0};
    uint8_t *p;
#ifdef BME280_DEBUG
    uint8_t i;
#endif

    data->temp=0;
    data->press=0;
    data->hum=0;

    read(0xF7,senser,8);
    
    p=(uint8_t*)&data->press;
    *p++=(senser[2] & 0xF0)>>4 | (senser[1] & 0x0F)<<4;
    *p++=(senser[1] & 0xF0)>>4 | (senser[0] & 0x0F)<<4;
    *p=(senser[0] & 0xF0)>>4;

    p=(uint8_t*)&data->temp;
    *p++=(senser[5] & 0xF0)>>4 | (senser[4] & 0x0F)<<4;
    *p++=(senser[4] & 0xF0)>>4 | (senser[3] & 0x0F)<<4;
    *p=(senser[3] & 0xF0)>>4;

    p=(uint8_t*)&data->hum;
    *p++=senser[7];
    *p=senser[6];

#ifdef BME280_DEBUG
    printf("[debug] raw data: ");
    for(i=0;i<8;i++){
        printf("%02x ",senser[i]);
    }
    printf("\n[debug] convert data: %ld p: %ld h: %ld\n",data->temp,data->press,data->hum);
#endif
}

/**
 * 获取当前状态
 */
uint8_t getStatus(){
#ifdef BME280_DEBUG
    uint8_t status;
    status=readByte(0xF3);
    printf("[debug] status: %02x\n",status);
    return status;
#else
    return readByte(0xF3);
#endif
}

/**
 * 设置温度采样率
 */
void setTempOversampling(uint8_t oversample){
    ctrl_meas&=0x1F;
    ctrl_meas|=(oversample & 0x7)<<5;
#ifdef BME280_DEBUG
    printf("[debug] TempOversampling: %02x , ctrl_meas: %02x\n",oversample,ctrl_meas);
#endif
}

/**
 * 设置湿度采样率
 */
void setHumOverSampling(uint8_t oversample){
    ctrl_hum=oversample & 0x7;
#ifdef BME280_DEBUG
    printf("[debug] HumOversampling: %02x , ctrl_meas: %02x\n",oversample,ctrl_hum);
#endif
}

/**
 * 设置压强采样率
 */
void setPressOverSampling(uint8_t oversample){
    ctrl_meas&=0xE3;
    ctrl_meas|=(oversample & 0x7)<<2;
#ifdef BME280_DEBUG
    printf("[debug] PressOversampling: %02x , ctrl_meas: %02x\n",oversample,ctrl_meas);
#endif
}

/**
 * 设置模式
 */
void setMode(uint8_t mode){
    ctrl_meas&=0xFC;
    ctrl_meas|=mode & 0x3;
#ifdef BME280_DEBUG
    printf("[debug] mode: %02x , ctrl_meas: %02x\n",mode,ctrl_meas);
#endif
}

/**
 * 设置一般模式间隔时间
 */
void setTSB(uint8_t duration){
    config&=0x1F;
    config|=(duration & 0x7)<<5;
#ifdef BME280_DEBUG
    printf("[debug] TSB: %02x , config: %02x\n",duration,config);
#endif
}

/**
 * 设置过滤器
 */
void setFilter(uint8_t filter){
    config&=0xE3;
    config|=(filter & 0x7)<<2;
#ifdef BME280_DEBUG
    printf("[debug] Filter: %02x , config: %02x\n",filter,config);
#endif
}

/**
 * SPI3线模式
 */
void setSpi3Wire(){
    config|=0x1;
#ifdef BME280_DEBUG
    printf("[debug] SPI 3 wire: true , config: %02x\n",config);
#endif
}

/**
 * 将设置写入
 */
void execute(){
    writeByte(0xF2,ctrl_hum);
    writeByte(0xF5,config);
    writeByte(0xF4,ctrl_meas);
#ifdef BME280_DEBUG
    printf("[debug] write setting => ctrl_hum: %02x , config: %02x , ctrl_meas: %02x\n",ctrl_hum,config,ctrl_meas);
#endif
}

/**
 * 示例
 */
void sample(){
    BME280_DATA data;
    BME280_NVM nvm;

    initDevice();

    if(getId() != 0x60){
        printf("read id failed \n");
        return;
    }

    reset();

    setTempOversampling(OVERSAMPLING_2X);
    setPressOverSampling(OVERSAMPLING_2X);
    setHumOverSampling(OVERSAMPLING_2X);
    setFilter(FILTER_COEFFICIENT_NONE);
    setMode(MODE_FORCE);
    execute();
    //等待取样完成
    while((getStatus() & 0x09) != 0){
        bcm2835_delay(100);
    }

    getCalibData(&nvm);
    getData(&data);

    printf("Temperature: %0.2f DegC \nPressure: %0.2f hPa \nHumidity: %0.2f % \n",calcTemp_double(&data,&nvm),calcPress_double(&data,&nvm)/100.0,calcHum_double(&data,&nvm));
}



int main(int argc,char ** argv){
    //bcm2835初始化I2C总线
    bcm2835_init();
    bcm2835_i2c_begin();
    
    //执行示例
    sample();

    bcm2835_i2c_end();
    bcm2835_close();
}