#include "mik32_hal_tsens.h"

/*!
@brief  Функция начальной инициализации термосенсора
@param  нет
@return нет
*/
void HAL_TSENS_Init()
{
    /* Тактирование аналогового блока */
	PM->CLK_APB_P_SET = PM_CLOCK_APB_P_ANALOG_REGS_M;
    /* Включение, тактирование температурного сенсора, отключение режима сброса */
    ANALOG_REG->TSENS_CFG = (1<<TSENS_CFG_NRST_S)|
							(1<<TSENS_CFG_NPD_CLK_S)|
							(1<<TSENS_CFG_NPD_S);
    /* Предустановка значений default */
#ifdef MIK32V0
    HAL_TSENS_SetTrim(128);               //< Настройка Trim только для MIK32V0
#endif
    HAL_TSENS_ClockSource(TSENS_SYS_CLK); //< Источник такта - sys_clk
    HAL_TSENS_Clock(50000ul);             //< Предустановка частоты tsens
}

/*!
@brief  Функция установки источника тактирования термосенсора
@param  TSENS_SYS_CLK         - системная тактовая частота
        TSENS_HCLK            - частота шины АНВ
        TSENS_EXTERNAL_32MHZ  - внешний генератор 32МГц
        TSENS_HSI32M          - внутренний генератор 32МГц
        TSENS_EXTERNAL_32KHZ  - внешний генератор 32кГц
        TSENS_LSI32K          - внутренний генератор 32кГц
@return 
    true, если источник тактирования установлен успешно
    false, если входной параметр некорректен или если выбираемая частота меньше минимальной
*/
bool HAL_TSENS_ClockSource(uint8_t data)
{
    uint32_t f_real;
    switch (data)
    {
        case 0: //TSENS_SYS_CLK
            f_real = F_CPU / (PM->DIV_AHB + 1) / (PM->DIV_APB_P + 1);
            break;
        case 1: //TSENS_HCLK
            f_real = F_CPU / (PM->DIV_APB_P + 1);
            break;
        case 2: //TSENS_EXTERNAL_32MHZ
            f_real = 32000000UL;
            break;
        case 3: //TSENS_HSI32M
            f_real = 32000000UL;
            break;
        case 4: //TSENS_EXTERNAL_32KHZ
            f_real = 32000UL;
            break;
        case 5: //TSENS_LSI32K
            f_real = 32000UL;
            break;
        default:
            return false;
    }
    if (f_real < 32000UL) return false;
    _hal_tsens_clkmux_ = data;
    ANALOG_REG->TSENS_CFG &= ~TSENS_CFG_CLK_MUX_M;
    ANALOG_REG->TSENS_CFG |= (_hal_tsens_clkmux_ << TSENS_CFG_CLK_MUX_S);
    HAL_TSENS_Clock(50000ul);  //< Предустановка частоты tsens
    return true;
}

/*!
@brief  Функция установки делителя частоты для термосенсора
@param  делитель частоты (uint16_t)
@return 
    true, если делитель частоты установлен успешно
    false, если делитель частоты слишком мал или слишком велик
*/
bool HAL_TSENS_ClockDivider(uint16_t data)
{
    data &= 0x3FF;
    //32kHz < fclk < 100kHz
    uint32_t divider;
    switch (_hal_tsens_clkmux_)
    {
        case 0: //TSENS_SYS_CLK
            divider = ((uint16_t)PM->DIV_AHB + 1) * ((uint16_t)PM->DIV_APB_P + 1) * (data + 1);
            if ((F_CPU / divider > 100000UL)||(F_CPU / divider < 32000UL)) return false;
            break;
        case 1: //TSENS_HCLK
            divider = ((uint16_t)PM->DIV_APB_P + 1) * (data + 1);
            if ((F_CPU / divider > 100000UL)||(F_CPU / divider < 32000UL)) return false;
            break;
        case 2: //TSENS_EXTERNAL_32MHZ
            if ((data < 320)||(data > 1000)) return false;
            break;
        case 3: //TSENS_HSI32M
            if ((data < 320)||(data > 1000)) return false;
            break;
        case 4: //TSENS_EXTERNAL_32KHZ
            data = 0;
            break;
        case 5: //TSENS_LSI32K
            data = 0;
            break;
        default:
            return false;
    }
/*#ifdef MIK32V0
    ANALOG_REG->TSENS_CFG &= 0b11111111000000000011111111111111;
#endif
#ifdef MIK32V2
    ANALOG_REG->TSENS_CFG &= 0b11111111111111110000000000111111;
#endif*/
    ANALOG_REG->TSENS_CFG &= ~TSENS_CFG_DIV_M;
    ANALOG_REG->TSENS_CFG |= (data<<TSENS_CFG_DIV_S);
    return true;
}

/*!
@brief  Функция установки тактовой частоты для термосенсора
@param  тактовая частота термосенсора (uint32_t)
@return 
    true, если частота установлена успешно
    false, если введенная частота не попадает в область допустимых значений частот для сенсора
*/
bool HAL_TSENS_Clock(uint32_t f_enter)
{
    if ((f_enter > 100000UL)||(f_enter < 32000UL)) return false;
    // находим f_real
    uint32_t f_real;
    switch (_hal_tsens_clkmux_)
    {
        case 0: //TSENS_SYS_CLK
            f_real = F_CPU / (PM->DIV_AHB + 1) / (PM->DIV_APB_P + 1);
            break;
        case 1: //TSENS_HCLK
            f_real = F_CPU / (PM->DIV_APB_P + 1);
            break;
        case 2: //TSENS_EXTERNAL_32MHZ
            f_real = 32000000UL;
            break;
        case 3: //TSENS_HSI32M
            f_real = 32000000UL;
            break;
        case 4: //TSENS_EXTERNAL_32KHZ
            f_real = 32000UL;
            break;
        case 5: //TSENS_LSI32K
            f_real = 32000UL;
            break;
        default:
            return false;
    }
    float up_border = (float)f_real / 100000.0;
    float down_border = (float)f_real / 32000.0;
    // If f_real < 32 kHz, return false
    if (down_border < 1) return false;
    uint32_t del = f_real / f_enter;
    if ((float)del < up_border) del += 1;
    if ((float)del > down_border) del -= 1;
    del -= 1;
    // на вывод
/*#ifdef MIK32V0
    ANALOG_REG->TSENS_CFG &= 0b11111111000000000011111111111111;
#endif
#ifdef MIK32V2
    ANALOG_REG->TSENS_CFG &= 0b11111111111111110000000000111111;
#endif*/
    ANALOG_REG->TSENS_CFG &= (~TSENS_CFG_DIV_M);
    ANALOG_REG->TSENS_CFG |= (del<<TSENS_CFG_DIV_S);
    return true;
}

/*!
@brief  Функция установки калибровочного значения для термосенсора
@param  калибровочное значение (uint8_t)
@return нет
!!! Функция работает только с MIK32V0 !!!
*/
#ifdef MIK32V0
void HAL_TSENS_SetTrim(uint8_t data)
{
    ANALOG_REG->TSENS_CFG &= 0b11111111111111111111100000000111;
    ANALOG_REG->TSENS_CFG |= (data << 3/*TSENS_CFG_TRIM_S*/);
}
#endif

/*!
@brief  Функция включения режима непрерывных измерений
@param  нет
@return нет
*/
void HAL_TSENS_ContiniousOn()
{
    ANALOG_REG->TSENS_CONTINUOUS |= 1;
}

/*!
@brief  Функция отключения режима непрерывных измерений
@param  нет
@return нет
*/
void HAL_TSENS_ContiniousOff()
{
    ANALOG_REG->TSENS_CONTINUOUS &= ~1;
}

/*!
@brief  Функция установки значения LOW_THRESHOLD
@param  значение LOW_THRESHOLD (uint16_t)
@return нет
*/
void HAL_TSENS_SetLowThreshold(uint16_t data)
{
    data &= 0x3FF;
    //ANALOG_REG->TSENS_THRESHOLD &= 0b11111111111100000000001111111111;
    ANALOG_REG->TSENS_THRESHOLD &= ~TSENS_TRESHOLD_LOW_M;
    ANALOG_REG->TSENS_THRESHOLD |= ((uint32_t)data << TSENS_TRESHOLD_LOW_S);
}

/*!
@brief  Функция установки значения HI_THRESHOLD
@param  значение HI_THRESHOLD (uint16_t)
@return нет
*/
void HAL_TSENS_SetHiThreshold(uint16_t data)
{
    data &= 0x3FF;
    //ANALOG_REG->TSENS_THRESHOLD &= 0b11111111111111111111110000000000;
    ANALOG_REG->TSENS_THRESHOLD &= ~TSENS_TRESHOLD_HI_M;
    ANALOG_REG->TSENS_THRESHOLD |= ((uint32_t)data << TSENS_TRESHOLD_HI_S);
}

/*!
@brief  Функция разрешения прерываний по вектору LOW_THRESHOLD_IRQ
@param  нет
@return нет
*/
void HAL_TSENS_LowIrq_Enable()
{
    ANALOG_REG->TSENS_IRQ |= TSENS_IRQ_LOW_MASK_M;
}

/*!
@brief  Функция запрета прерываний по вектору LOW_THRESHOLD_IRQ
@param  нет
@return нет
*/
void HAL_TSENS_LowIrq_Disable()
{
    ANALOG_REG->TSENS_IRQ &= ~TSENS_IRQ_LOW_MASK_M;
}

/*!
@brief  Функция очистки флага прерывания LOW_THRESHOLD_IRQ
@param  нет
@return нет
*/
void HAL_TSENS_LowIrq_Clear()
{
    ANALOG_REG->TSENS_CLEAR_IRQ = TSENS_CLEAR_IRQ_LOW_CLEAR_M;
}

/*!
@brief  Функция проверки флага прерывания LOW_THRESHOLD_IRQ
@param  нет
@return true  - флаг установлен
        false - флаг очищен
*/
bool HAL_TSENS_LowIrq_Event()
{
    return ANALOG_REG->TSENS_IRQ & TSENS_IRQ_LOW_IRQ_M;
}

/*!
@brief  Функция разрешения прерываний по вектору HI_THRESHOLD_IRQ
@param  нет
@return нет
*/
void HAL_TSENS_HiIrq_Enable()
{
    ANALOG_REG->TSENS_IRQ |= TSENS_IRQ_HI_MASK_M;
}

/*!
@brief  Функция запрета прерываний по вектору HI_THRESHOLD_IRQ
@param  нет
@return нет
*/
void HAL_TSENS_HiIrq_Disable()
{
    ANALOG_REG->TSENS_IRQ &= ~TSENS_IRQ_HI_MASK_M;
}

/*!
@brief  Функция очистки флага прерывания HI_THRESHOLD_IRQ
@param  нет
@return нет
*/
void HAL_TSENS_HiIrq_Clear()
{
    ANALOG_REG->TSENS_CLEAR_IRQ = TSENS_CLEAR_IRQ_HI_CLEAR_M;
}

/*!
@brief  Функция проверки флага прерывания HI_THRESHOLD_IRQ
@param  нет
@return true  - флаг установлен
        false - флаг очищен
*/
bool HAL_TSENS_HiIrq_Event()
{
    return ANALOG_REG->TSENS_IRQ & TSENS_IRQ_HI_IRQ_M;
}

/*!
@brief  Функция разрешения прерываний по вектору EOC_IRQ
@param  нет
@return нет
*/
void HAL_TSENS_EOCIrq_Enable()
{
    ANALOG_REG->TSENS_IRQ |= TSENS_IRQ_EOC_MASK_M;
}

/*!
@brief  Функция запрета прерываний по вектору EOC_IRQ
@param  нет
@return нет
*/
void HAL_TSENS_EOCIrq_Disable()
{
    ANALOG_REG->TSENS_IRQ &= ~TSENS_IRQ_EOC_MASK_M;
}

/*!
@brief  Функция очистки флага прерывания EOC_IRQ
@param  нет
@return нет
*/
void HAL_TSENS_EOCIrq_Clear()
{
    ANALOG_REG->TSENS_CLEAR_IRQ = TSENS_CLEAR_IRQ_EOC_CLEAR_M;
}

/*!
@brief  Функция проверки флага прерывания EOC_IRQ
@param  нет
@return true  - флаг установлен
        false - флаг очищен
*/
bool HAL_TSENS_EOCIrq_Event()
{
    return ANALOG_REG->TSENS_IRQ & TSENS_IRQ_EOC_IRQ_M;
}

// Reading data
/*!
@brief  Функция чтения данных термосенсора в режиме Continious-mode
@param  нет
@return температура в градусах Цельсия
*/
float HAL_TSENS_ReadMeasurement()
{
    uint32_t value = ANALOG_REG->TSENS_VALUE & 0x3FF;
    return (float)TSENS_VALUE_TO_CELSIUS(value) / 100.0;
}

/*!
@brief  Функция чтения данных термосенсора в режиме Single-mode
@param  нет
@return температура в градусах Цельсия
*/
float HAL_TSENS_SingleMeasurement()
{
    ANALOG_REG->TSENS_SINGLE = 1;
    while (!HAL_TSENS_EOCIrq_Event());
    return HAL_TSENS_ReadMeasurement();
}
