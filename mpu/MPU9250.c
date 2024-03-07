





void SPI_writeReg(uint8_t addr, uint8_t data)
{
    addr &= 0b01111111;
    SPI_down(&mpu_spi);
    SPI_ex(&mpu_spi, addr);
    SPI_ex(&mpu_spi, data);
    SPI_up(&mpu_spi);
}
void SPI_readReg(uint8_t addr, uint8_t* buf)
{
    addr &= 0b01111111;
    addr |= (1<<7);
    SPI_down(&mpu_spi);
    SPI_ex(&mpu_spi, addr);
    *buf = SPI_ex(&mpu_spi, 0xFF);
    SPI_up(&mpu_spi);
}

bool begin(void)
{
    // Настройка режима энергопотребления
    SPI_writeReg(MPU_PWR_MGMT_1,
        (1<<MPU_H_RESET)|
        (0<<MPU_SLEEP)|
        (0<<MPU_CYCLE)|
        (0<<MPU_GYRO_STANDBY));
    // Тест на MPU9250
    HAL_DelayMs(10);
    uint8_t data;
    SPI_readReg(MPU_WHO_I_AM, &data);
    if (data != 112) return false;
    // Установка BANDWITH
    SPI_writeReg(MPU_CONFIG,
        (1<<MPU_DLPF_CFG2)|
        (0<<MPU_DLPF_CFG1)|
        (0<<MPU_DLPF_CFG0)|
        (0<<MPU_Fchoice_b1)|
        (0<<MPU_Fchoice_b0));
    SPI_writeReg(MPU_ACCEL_CONFIG_2,
        (0<<MPU_accel_fchoice_b)|
        (1<<MPU_A_DLPFCFG2)|
        (0<<MPU_A_DLPFCFG1)|
        (0<<MPU_A_DLPFCFG0));
    // Разрешить работу гироскопа и акселерометра
    SPI_writeReg(MPU_PWR_MGMT_2,
        (0<<MPU_DISABLE_XA)|
        (0<<MPU_DISABLE_YA)|
        (0<<MPU_DISABLE_ZA)|
        (0<<MPU_DISABLE_XG)|
        (0<<MPU_DISABLE_YG)|
        (0<<MPU_DISABLE_ZG));
    //Магнитометр не нужон
    return true;
}

