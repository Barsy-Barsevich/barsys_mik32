#ifndef MPU9250_SPI_Driver
#define MPU9250_SPI_Driver

typedef struct {
    SPI_HandleTypeDef* spi;
    uint8_t cs;
} MPU_HandleTypeDef;

MPU_HandleTypeDef mpu;


inline void SPI_down();
inline void SPI_up();
inline void SPI_writeReg(uint8_t addr, uint8_t data);
inline void SPI_readReg(uint8_t addr, uint8_t* buf);


void SPI_init(MPU_HandleTypeDef* local, SPI_HandleTypeDef* spi, uint8_t cs)
{
    local->spi = spi;
    local->cs = cs;
}

inline void SPI_down(MPU_HandleTypeDef* local)
{
    HAL_SPI_CS_Enable(local->spi, local->cs);
}
inline void SPI_writeReg(MPU_HandleTypeDef* local, uint8_t addr, uint8_t data)
{
    addr &= 0b01111111;
    SPI_down(local->spi);
    SPI_ex(local->spi, addr);
    SPI_ex(local->spi, data);
    SPI_up(spi);
}
inline void SPI_readReg(MPU_HandleTypeDef* local, uint8_t addr, uint8_t* buf)
{
    addr &= 0b01111111;
    addr |= 0b10000000;
    SPI_down(local->spi);
    SPI_ex(local->spi, addr);
    *buf = SPI_ex(local->spi, 0xFF);
    SPI_up(local->spi);
}

#endif
