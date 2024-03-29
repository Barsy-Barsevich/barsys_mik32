#ifndef MPU9250_MIK32_Barsotion
#define MPU9250_MIK32_Barsotion

#include "mik32_hal_scr1_timer.h"
#include "mik32_hal_spi.h"
#include "MPU9250_RegisterMap.c"
#include "GKalman.c"

//Mag options
#define MAG_RESOLUTION_14b          0
#define MAG_RESOLUTION_16b          1
#define MAG_POWER_DOWN_MODE         0b0000
#define MAG_SINGLE_MEAS_MODE        0b0001
#define MAG_CONT_MEAS_MODE_1        0b0010
#define MAG_CONT_MEAS_MODE_2        0b0110
#define MAG_EXT_TRIG_MEAS_MODE      0b0100
#define MAG_SELF_TEST_MODE          0b1000
#define MAG_FUSE_ROM_ACCESS_MODE    0b1111
//Scale setting
#define ACCEL_SCALE_2G_COEF         2./32767
#define ACCEL_SCALE_4G_COEF         4./32767
#define ACCEL_SCALE_8G_COEF         8./32767
#define ACCEL_SCALE_16G_COEF        16./32767
#define GYRO_SCALE_250_COEF         250./32767
#define GYRO_SCALE_500_COEF         500./32767
#define GYRO_SCALE_1000_COEF        1000./32767.0
#define GYRO_SCALE_2000_COEF        2000./32767.0
//IDs
#define MPU9250_ID_VALUE            0x68
#define MPU9255_ID_VALUE            0x73

typedef struct {
    float x;
    float y;
    float z;
} MPU_AxesTypeDef;

typedef struct {
    SPI_HandleTypeDef* spi;
    uint8_t cs;
    float accel_scale;
    float gyro_scale;
    MPU_AxesTypeDef accel;      //Значения акселерометра
    MPU_AxesTypeDef gyro;       //Значения гироскопа
    MPU_AxesTypeDef accel_bias; //Смещения акселерометра
    MPU_AxesTypeDef gyro_bias;  //Смещения гироскопа
    MPU_AxesTypeDef eps;        //Средняя погрешность недвижимого гироскопа
    GKalman_HandlerTypeDef* filter_gx;
    GKalman_HandlerTypeDef* filter_gy;
    GKalman_HandlerTypeDef* filter_gz;
    MPU_AxesTypeDef gyro_filtered;
} MPU_HandleTypeDef;

GKalman_HandlerTypeDef filter_gx;
GKalman_HandlerTypeDef filter_gy;
GKalman_HandlerTypeDef filter_gz;

typedef enum {
    ACCEL_SCALE_2G = 0,
    ACCEL_SCALE_4G = 1,
    ACCEL_SCALE_8G = 2,
    ACCEL_SCALE_16G = 3,
    ACCEL_SCALE_DEFAULT = 0
} MPU9250_AccelScale;
typedef enum {
    GYRO_SCALE_250 = 0,
    GYRO_SCALE_500 = 1,
    GYRO_SCALE_1000 = 2,
    GYRO_SCALE_2000 = 3,
    GYRO_SCALE_DEFAULT = 0
} MPU9250_GyroScale;
typedef enum {
    ACCEL_BANDWIDTH_218p1Hz = 0,
    ACCEL_BANDWIDTH_99Hz = 2,
    ACCEL_BANDWIDTH_44p8Hz = 3,
    ACCEL_BANDWIDTH_21p2Hz = 4,
    ACCEL_BANDWIDTH_10p2Hz = 5,
    ACCEL_BANDWIDTH_5p05Hz = 6,
    ACCEL_BANDWIDTH_420Hz = 7
} MPU9250_AccelBandwidth;
typedef enum {
    GYRO_BANDWIDTH_3600Hz = 7,
    GYRO_BANDWIDTH_250Hz = 0,
    GYRO_BANDWIDTH_184Hz = 1,
    GYRO_BANDWIDTH_92Hz = 2,
    GYRO_BANDWIDTH_41Hz = 3,
    GYRO_BANDWIDTH_20Hz = 4,
    GYRO_BANDWIDTH_10Hz = 5,
    GYRO_BANDWIDTH_5Hz = 6
} MPU9250_GyroBandwidth;

#include "SPI_Driver.c"

HAL_StatusTypeDef mpu_begin(MPU_HandleTypeDef* local);
void mpu_setAccelScale(MPU_HandleTypeDef* local, MPU9250_AccelScale scale);
void mpu_setGyroScale(MPU_HandleTypeDef* local, MPU9250_GyroScale scale);
void mpu_setAccelBandwidth(MPU_HandleTypeDef* local, MPU9250_AccelBandwidth bw);
void mpu_setGyroBandwidth(MPU_HandleTypeDef* local, MPU9250_GyroBandwidth bw);
void mpu_readData(MPU_HandleTypeDef* local);

void mpu_CalibrateGyro(MPU_HandlerTypeDef* local);
void mpu_GyroFilter(MPU_HandlerTypeDef* local);

#include "MPU9250.c"

#endif
