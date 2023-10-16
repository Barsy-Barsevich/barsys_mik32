#ifndef MIK32_TSENS
#define MIK32_TSENS

#include "analog_reg.h"
#include "pad_config.h"
#include "stdbool.h"
#include "mcu32_memory_map.h"
#include <power_manager.h>
//#include "mik32_hal_pcc.h"

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
#define TSENS_SYS_CLK           0x0
#define TSENS_HCLK              0x1
#define TSENS_EXTERNAL_32MHZ    0x2
#define TSENS_HSI32M            0x3
#define TSENS_EXTERNAL_32KHZ    0x4
#define TSENS_LSI32K            0x5

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
void HAL_TSENS_Init(void);
bool HAL_TSENS_ClockSource(uint8_t);
bool HAL_TSENS_ClockDivider(uint16_t);
bool HAL_TSENS_Clock(uint32_t);
void HAL_TSENS_SetTrim(uint8_t);
void HAL_TSENS_ContiniousOn();
void HAL_TSENS_ContiniousOff();
/* Установка значений температурных пределов threshold */
void HAL_TSENS_SetLowThreshold(uint16_t);
void HAL_TSENS_SetHiThreshold(uint16_t);
/* Настройка прерываний  */
void HAL_TSENS_LowIrq_Enable();
void HAL_TSENS_LowIrq_Disable();
void HAL_TSENS_LowIrq_Clear();
void HAL_TSENS_HiIrq_Enable();
void HAL_TSENS_HiIrq_Disable();
void HAL_TSENS_HiIrq_Clear();
void HAL_TSENS_EOSIrq_Enable();
void HAL_TSENS_EOSIrq_Disable();
void HAL_TSENS_EOSIrq_Clear();
/* Чтение флагов прерываний */
bool HAL_TSENS_LowIrq_Event();
bool HAL_TSENS_HiIrq_Event();
bool HAL_TSENS_EOSIrq_Event();
/* Чтение данных */
float HAL_TSENS_ReadMeasurement(void);
float HAL_TSENS_SingleMeasurement();

#endif