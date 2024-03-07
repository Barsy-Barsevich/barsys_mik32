#ifndef MPU9250_SPI_Driver
#define MPU9250_SPI_Driver


void SPI_init(MPU_HandleTypeDef* local, SPI_HandleTypeDef* _spi, uint8_t _cs);
inline void SPI_down(MPU_HandleTypeDef* local);
inline void SPI_up(MPU_HandleTypeDef* local);
inline void SPI_writeReg(MPU_HandleTypeDef* local, uint8_t addr, uint8_t data);
inline void SPI_readReg(MPU_HandleTypeDef* local, uint8_t addr, uint8_t* buf);
inline void SPI_read14Bytes(MPU_HandleTypeDef* local, uint8_t addr, uint8_t[] buf);


void SPI_init(MPU_HandleTypeDef* local, SPI_HandleTypeDef* _spi, uint8_t _cs)
{
    local->spi = _spi;
    local->cs = _cs;
}

inline void SPI_down(MPU_HandleTypeDef* local)
{
    HAL_SPI_CS_Enable(local->spi, local->cs);
}
inline void SPI_up(MPU_HandleTypeDef* local)
{
    HAL_SPI_CS_Disable(local->spi, local->cs);
}
inline void SPI_writeReg(MPU_HandleTypeDef* local, uint8_t addr, uint8_t data)
{
    addr &= 0b01111111;
    uint8_t dummy;
    HAL_SPI_CS_Enable(local->spi, local->cs);
    HAL_SPI_Exchange(local->spi, &addr, &dummy, 1, SPI_TIMEOUT_DEFAULT);
    HAL_SPI_Exchange(local->spi, &data, &dummy, 1, SPI_TIMEOUT_DEFAULT);
    HAL_SPI_CS_Disable(local->spi, local->cs);
}
inline void SPI_readReg(MPU_HandleTypeDef* local, uint8_t addr, uint8_t* buf)
{
    addr &= 0b01111111;
    addr |= 0b10000000;
    uint8_t dummy = 0xFF;
    HAL_SPI_CS_Enable(local->spi, local->cs);
    HAL_SPI_Exchange(local->spi, &addr, &dummy, 1, SPI_TIMEOUT_DEFAULT);
    HAL_SPI_Exchange(local->spi, &dummy, buf, 1, SPI_TIMEOUT_DEFAULT);
    HAL_SPI_CS_Disable(local->spi, local->cs);
}
inline void SPI_read14Bytes(MPU_HandleTypeDef* local, uint8_t addr, uint8_t[] buf)
{
    addr &= 0b01111111;
    addr |= 0b10000000;
    uint8_t dummy[14];
    HAL_SPI_CS_Enable(local->spi, local->cs);
    HAL_SPI_Exchange(local->spi, &addr, dummy, 1, SPI_TIMEOUT_DEFAULT);
    HAL_SPI_Exchange(local->spi, buf, dummy, 14, SPI_TIMEOUT_DEFAULT);
    HAL_SPI_CS_Disable(local->spi, local->cs);
}

#endif
