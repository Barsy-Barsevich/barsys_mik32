#include "MPU9250.h"

HAL_StatusTypeDef mpu_begin(MPU_HandleTypeDef* local)
{
    // Настройка режима энергопотребления
    SPI_writeReg(local, MPU_PWR_MGMT_1,
        (1<<MPU_H_RESET)|
        (0<<MPU_SLEEP)|
        (0<<MPU_CYCLE)|
        (0<<MPU_GYRO_STANDBY));
    // Тест на MPU9250
    HAL_DelayMs(10);
    uint8_t data;
    SPI_readReg(local, MPU_WHO_I_AM, &data);
    if ((data != MPU9250_ID_VALUE) && (data != MPU9255_ID_VALUE)) return HAL_ERROR;
    // Установка BANDWITH
    SPI_writeReg(local, MPU_CONFIG,
        (1<<MPU_DLPF_CFG2)|
        (0<<MPU_DLPF_CFG1)|
        (0<<MPU_DLPF_CFG0)|
        (0<<MPU_Fchoice_b1)|
        (0<<MPU_Fchoice_b0));
    SPI_writeReg(local, MPU_ACCEL_CONFIG_2,
        (0<<MPU_accel_fchoice_b)|
        (1<<MPU_A_DLPFCFG2)|
        (0<<MPU_A_DLPFCFG1)|
        (0<<MPU_A_DLPFCFG0));
    // Разрешить работу гироскопа и акселерометра
    SPI_writeReg(local, MPU_PWR_MGMT_2,
        (0<<MPU_DISABLE_XA)|
        (0<<MPU_DISABLE_YA)|
        (0<<MPU_DISABLE_ZA)|
        (0<<MPU_DISABLE_XG)|
        (0<<MPU_DISABLE_YG)|
        (0<<MPU_DISABLE_ZG));
    //Магнитометр не нужон
    //<>
    //Установка биасов
    local->accel_bias.x = 0;
    local->accel_bias.y = 0;
    local->accel_bias.z = 0;
    local->gyro_bias.x = 0;
    local->gyro_bias.y = 0;
    local->gyro_bias.z = 0;
    //Инициализация фильтров
    local->filter_gx = &filter_gx;
    local->filter_gx = &filter_gy;
    local->filter_gx = &filter_gz;
    return HAL_OK;
}

    
/* !
 * mpu_setAccelScale - установка диапазона акселерометра
 * 
 */
void mpu_setAccelScale(MPU_HandleTypeDef* local, MPU9250_AccelScale scale)
{
    //Установка константы
    switch (scale) {
        case ACCEL_SCALE_2G:  local->accel_scale = ACCEL_SCALE_2G_COEF; break;
        case ACCEL_SCALE_4G:  local->accel_scale = ACCEL_SCALE_4G_COEF; break;
        case ACCEL_SCALE_8G:  local->accel_scale = ACCEL_SCALE_8G_COEF; break;
        case ACCEL_SCALE_16G:  local->accel_scale = ACCEL_SCALE_16G_COEF; break;
        default: local->accel_scale = ACCEL_SCALE_16G_COEF; break;
    }
    //Установка настройки
    SPI_writeReg(local, MPU_ACCEL_CONFIG, (scale<<3));
}
/* !
 * mpu_setAccelScale - установка диапазона акселерометра
 * 
 */
void mpu_setGyroScale(MPU_HandleTypeDef* local, MPU9250_GyroScale scale)
{
    //Установка константы
    switch (scale) {
        case GYRO_SCALE_250:  local->gyro_scale = GYRO_SCALE_250_COEF; break;
        case GYRO_SCALE_500:  local->gyro_scale = GYRO_SCALE_500_COEF; break;
        case GYRO_SCALE_1000:  local->gyro_scale = GYRO_SCALE_1000_COEF; break;
        case GYRO_SCALE_2000:  local->gyro_scale = GYRO_SCALE_2000_COEF; break;
        default: local->gyro_scale = GYRO_SCALE_250_COEF; break;
    }
    //Установка настройки
    SPI_writeReg(local, MPU_GYRO_CONFIG, (scale<<3));
}
/* !
 * mpu_setAccelScale - установка диапазона акселерометра
 * 
 */
void mpu_setAccelBandwidth(MPU_HandleTypeDef* local, MPU9250_AccelBandwidth bw)
{
    SPI_writeReg(local, MPU_ACCEL_CONFIG_2, bw);
}
/* !
 * mpu_setAccelScale - установка диапазона акселерометра
 * 
 */
void mpu_setGyroBandwidth(MPU_HandleTypeDef* local, MPU9250_GyroBandwidth bw)
{
    SPI_writeReg(local, MPU_CONFIG, bw);
}
    
/* !
 * mpu_readData - чтение данных акселерометра, гироскопа, термометра
 * 
 */
void mpu_readData(MPU_HandleTypeDef* local)
{
    uint8_t raw[14];
    SPI_read14Bytes(local, MPU_ACCEL_XOUT_H, raw);
    int16_t a_x, a_y, a_z;
    int16_t g_x, g_y, g_z;
    a_x = (((int16_t)raw[0]<<8)|(int16_t)raw[1]); //X
    a_y = (((int16_t)raw[2]<<8)|(int16_t)raw[3]); //Y
    a_z = (((int16_t)raw[4]<<8)|(int16_t)raw[5]); //Z
    g_x = (((int16_t)raw[8]<<8)|(int16_t)raw[9]); //X
    g_y = (((int16_t)raw[10]<<8)|(int16_t)raw[11]); //Y
    g_z = (((int16_t)raw[12]<<8)|(int16_t)raw[13]); //Z
    local->accel.x = (float)a_x * local->accel_scale + local->accel_bias.x;
    local->accel.y = (float)a_y * local->accel_scale + local->accel_bias.y;
    local->accel.z = (float)a_z * local->accel_scale + local->accel_bias.z;
    local->gyro.x = (float)g_x * local->gyro_scale + local->gyro_bias.x;
    local->gyro.y = (float)g_y * local->gyro_scale + local->gyro_bias.y;
    local->gyro.z = (float)g_z * local->gyro_scale + local->gyro_bias.z;
}



void mpu_CalibrateGyro(MPU_HandlerTypeDef* local)
{
    //Обнуление значений смещений и средних отклонений
    local->gyro_bias.x = 0;
    local->gyro_bias.y = 0;
    local->gyro_bias.z = 0;
    local->eps.x = 0;
    local->eps.y = 0;
    local->eps.z = 0;
    //Получить значения смещений
    for (uint8_t i=0; i<200; i++)
    {
        mpu_readData(local);
        local->gyro_bias.x += local->gyro.x;
        local->gyro_bias.y += local->gyro.y;
        local->gyro_bias.z += local->gyro.z;
        HAL_DelayMs(10);
    }
    local->gyro_bias.x *= 0.005F; // /=200
    local->gyro_bias.y *= 0.005F;
    local->gyro_bias.z *= 0.005F;
    //Получить значения средних отклонений
    for (uint8_t i=0; i<200; i++)
    {
        mpu_readData(local);
        local->eps.x += fabs(local->gyro.x - local->gyro_bias.x);
        local->eps.y += fabs(local->gyro.y - local->gyro_bias.y);
        local->eps.z += fabs(local->gyro.z - local->gyro_bias.z);
        HAL_DelayMs(10);
    }
    local->eps.x *= 0.005F; // /=200
    local->eps.y *= 0.005F;
    local->eps.z *= 0.005F;
    //Загрузка значений в фильтр Калмана
    GKalman_setParameters(local->filter_gx, local->eps.x, local->eps.x, 0.5);
    GKalman_setParameters(local->filter_gy, local->eps.y, local->eps.y, 0.5);
    GKalman_setParameters(local->filter_gz, local->eps.z, local->eps.z, 0.5);
}

/* !
 * @brief 
 * 
 *
 */
void mpu_GyroFilter(MPU_HandlerTypeDef* local)
{
    local->gyro_filtered.x = GKalman_Filtered(local->filter_gx, local->gyro.x);
    local->gyro_filtered.y = GKalman_Filtered(local->filter_gy, local->gyro.y);
    local->gyro_filtered.z = GKalman_Filtered(local->filter_gz, local->gyro.z);
}
