#ifndef MIK32_TSENS
#define MIK32_TSENS

#include "analog_reg.h"
#include "pad_config.h"
#include "stdbool.h"
#include "mcu32_memory_map.h"
#include <power_manager.h>
#include "mik32_hal_def.h"
#include "mik32_hal_pcc.h"

/*
 * Define: F_CPU
 * Тактовая частота микроконтроллера
 * ! Отключить, если F_CPU объявляется системно !
 *
 */
#define F_CPU 32000000UL

/*
 * Define
 * Константы-параметры функции установки источника тактирования TSENS
 */
typedef enum __HAL_TSENS_ClockTypeDef
{
    HAL_TSENS_SYS_CLK         = 0x0,
    HAL_TSENS_HCLK            = 0x1,
    HAL_TSENS_EXTERNAL_32MHZ  = 0x2,
    HAL_TSENS_HSI32M          = 0x3,
    HAL_TSENS_EXTERNAL_32KHZ  = 0x4,
    HAL_TSENS_LSI32K          = 0x5
} HAL_TSENS_ClockTypeDef;

/*
 * Variable: _hal_tsens_clkmux_
 *
 * Переменная для хранения значения текущего режима тактирования
 * 
 * Эта переменная может содержать число в пределах от 0 до 5
 * 
 */
uint8_t _hal_tsens_clkmux_;

/* Инициализация и начальная настройка */
void HAL_TSENS_MspInit();
void HAL_TSENS_Init();
HAL_StatusTypeDef HAL_TSENS_ClockSource(HAL_TSENS_ClockTypeDef clock_source);
HAL_StatusTypeDef HAL_TSENS_ClockDivider(uint16_t clock_divider);
HAL_StatusTypeDef HAL_TSENS_Clock(uint32_t clock_frequency_hz);
void HAL_TSENS_SingleBegin();
void HAL_TSENS_ContinuousOn();
void HAL_TSENS_ContinuousOff();
/* Установка значений температурных пределов threshold */
void HAL_TSENS_SetLowThresholdRaw(uint16_t raw_value);
void HAL_TSENS_SetHiThresholdRaw(uint16_t raw_value);
void HAL_TSENS_SetLowThreshold(int32_t value);
void HAL_TSENS_SetHiThreshold(int32_t value);
/* Настройка прерываний  */
void HAL_TSENS_LowIrq_Enable();
void HAL_TSENS_LowIrq_Disable();
void HAL_TSENS_LowIrq_Clear();
void HAL_TSENS_HiIrq_Enable();
void HAL_TSENS_HiIrq_Disable();
void HAL_TSENS_HiIrq_Clear();
void HAL_TSENS_EOCIrq_Enable();
void HAL_TSENS_EOCIrq_Disable();
void HAL_TSENS_EOCIrq_Clear();
/* Чтение флагов прерываний */
HAL_StatusTypeDef HAL_TSENS_LowIrq_Event();
HAL_StatusTypeDef HAL_TSENS_HiIrq_Event();
HAL_StatusTypeDef HAL_TSENS_EOCIrq_Event();
/* Чтение данных */
int32_t HAL_TSENS_ReadMeasurement();
int32_t HAL_TSENS_SingleMeasurement();

/* Установка TRIM (только для версии V0) */
#ifdef MIK32V0
void HAL_TSENS_SetTrim(uint8_t trim_value);
#endif

#endif
