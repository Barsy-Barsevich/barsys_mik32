#include "mik32_hal_tsens.h"

/*!
@brief  Функция включения тактирования аналогового блока (включающего термосенсор)
@param  нет
@return нет
*/
__attribute__((weak)) void HAL_TSENS_MspInit()
{
    //PM->CLK_APB_P_SET = PM_CLOCK_APB_P_ANALOG_REGS_M;
    __HAL_PCC_ANALOG_REGS_CLK_ENABLE();
}

/*!
@brief  Функция начальной инициализации термосенсора
@param  нет
@return нет
*/
void HAL_TSENS_Init()
{
    HAL_TSENS_MspInit();
    /* Включение, тактирование температурного сенсора, отключение режима сброса */
    ANALOG_REG->TSENS_CFG = (1<<TSENS_CFG_NRST_S)|
							(1<<TSENS_CFG_NPD_CLK_S)|
							(1<<TSENS_CFG_NPD_S);
    /* Предустановка значений default */
#ifdef MIK32V0
    HAL_TSENS_SetTrim(128);               //< Настройка Trim только для MIK32V0
#endif
    HAL_TSENS_ClockSource(HAL_TSENS_SYS_CLK); //< Источник такта - sys_clk
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
HAL_StatusTypeDef HAL_TSENS_ClockSource(HAL_TSENS_ClockTypeDef clk_source)
{
    uint32_t f_real;
    switch (clk_source)
    {
        case HAL_TSENS_SYS_CLK:
            f_real = F_CPU / (PM->DIV_AHB + 1) / (PM->DIV_APB_P + 1);
            break;
        case HAL_TSENS_HCLK:
            f_real = F_CPU / (PM->DIV_APB_P + 1);
            break;
        case HAL_TSENS_EXTERNAL_32MHZ:
            f_real = 32000000UL;
            break;
        case HAL_TSENS_HSI32M:
            f_real = 32000000UL;
            break;
        case HAL_TSENS_EXTERNAL_32KHZ:
            f_real = 32000UL;
            break;
        case HAL_TSENS_LSI32K:
            f_real = 32000UL;
            break;
        default:
            return HAL_ERROR;
    }
    if (f_real < 32000UL) return HAL_ERROR;
    _hal_tsens_clkmux_ = clk_source;
    ANALOG_REG->TSENS_CFG &= ~TSENS_CFG_CLK_MUX_M;
    ANALOG_REG->TSENS_CFG |= (_hal_tsens_clkmux_ << TSENS_CFG_CLK_MUX_S);
    HAL_TSENS_Clock(50000ul);  //< Предустановка частоты tsens
    return HAL_OK;
}

/*!
@brief  Функция установки делителя частоты для термосенсора
@param  делитель частоты (uint16_t)
@return 
    true, если делитель частоты установлен успешно
    false, если делитель частоты слишком мал или слишком велик
*/
HAL_StatusTypeDef HAL_TSENS_ClockDivider(uint16_t clk_div)
{
    clk_div &= 0x3FF;
    //32kHz < fclk < 100kHz
    uint32_t divider;
    switch (_hal_tsens_clkmux_)
    {
        case HAL_TSENS_SYS_CLK:
            divider = ((uint16_t)PM->DIV_AHB + 1) * ((uint16_t)PM->DIV_APB_P + 1) * (clk_div + 1);
            if ((F_CPU / divider > 100000UL)||(F_CPU / divider < 32000UL)) return false;
            break;
        case HAL_TSENS_HCLK:
            divider = ((uint16_t)PM->DIV_APB_P + 1) * (clk_div + 1);
            if ((F_CPU / divider > 100000UL)||(F_CPU / divider < 32000UL)) return false;
            break;
        case HAL_TSENS_EXTERNAL_32MHZ:
            if ((clk_div < 320)||(clk_div > 1000)) return false;
            break;
        case HAL_TSENS_HSI32M:
            if ((clk_div < 320)||(clk_div > 1000)) return false;
            break;
        case HAL_TSENS_EXTERNAL_32KHZ:
            clk_div = 0;
            break;
        case HAL_TSENS_LSI32K:
            clk_div = 0;
            break;
        default:
            return HAL_ERROR;
    }
/*#ifdef MIK32V0
    ANALOG_REG->TSENS_CFG &= 0b11111111000000000011111111111111;
#endif
#ifdef MIK32V2
    ANALOG_REG->TSENS_CFG &= 0b11111111111111110000000000111111;
#endif*/
    ANALOG_REG->TSENS_CFG &= ~TSENS_CFG_DIV_M;
    ANALOG_REG->TSENS_CFG |= (clk_div<<TSENS_CFG_DIV_S);
    return HAL_OK;
}

/*!
@brief  Функция установки тактовой частоты для термосенсора
@param  тактовая частота термосенсора (uint32_t)
@return 
    true, если частота установлена успешно
    false, если введенная частота не попадает в область допустимых значений частот для сенсора
*/
HAL_StatusTypeDef HAL_TSENS_Clock(uint32_t f_enter)
{
    if ((f_enter > 100000UL)||(f_enter < 32000UL)) return HAL_ERROR;
    // находим f_real
    uint32_t f_real;
    switch (_hal_tsens_clkmux_)
    {
        case HAL_TSENS_SYS_CLK:
            f_real = F_CPU / (PM->DIV_AHB + 1) / (PM->DIV_APB_P + 1);
            break;
        case HAL_TSENS_HCLK:
            f_real = F_CPU / (PM->DIV_APB_P + 1);
            break;
        case HAL_TSENS_EXTERNAL_32MHZ:
            f_real = 32000000UL;
            break;
        case HAL_TSENS_HSI32M:
            f_real = 32000000UL;
            break;
        case HAL_TSENS_EXTERNAL_32KHZ:
            f_real = 32000UL;
            break;
        case HAL_TSENS_LSI32K:
            f_real = 32000UL;
            break;
        default:
            return HAL_ERROR;
    }
    float up_border = (float)f_real / 100000.0;
    float down_border = (float)f_real / 32000.0;
    // If f_real < 32 kHz, return false
    if (down_border < 1) return HAL_ERROR;
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
    return HAL_OK;
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

void HAL_TSENS_SingleBegin()
{
    ANALOG_REG->TSENS_SINGLE = 1;
}

/*!
@brief  Функция включения режима непрерывных измерений
@param  нет
@return нет
*/
void HAL_TSENS_ContinuousOn()
{
    ANALOG_REG->TSENS_CONTINUOUS |= 1;
}

/*!
@brief  Функция отключения режима непрерывных измерений
@param  нет
@return нет
*/
void HAL_TSENS_ContinuousOff()
{
    ANALOG_REG->TSENS_CONTINUOUS &= ~1;
}

/*!
@brief  Функция установки сырого значения LOW_THRESHOLD
@param  значение LOW_THRESHOLD (uint16_t)
@return нет
*/
void HAL_TSENS_SetLowThresholdRaw(uint16_t data)
{
    data &= 0x3FF;
    //ANALOG_REG->TSENS_THRESHOLD &= 0b11111111111100000000001111111111;
    ANALOG_REG->TSENS_THRESHOLD &= ~TSENS_TRESHOLD_LOW_M;
    ANALOG_REG->TSENS_THRESHOLD |= ((uint32_t)data << TSENS_TRESHOLD_LOW_S);
}

/*!
@brief  Функция установки сырого значения HI_THRESHOLD
@param  значение HI_THRESHOLD (uint16_t)
@return нет
*/
void HAL_TSENS_SetHiThresholdRaw(uint16_t data)
{
    data &= 0x3FF;
    //ANALOG_REG->TSENS_THRESHOLD &= 0b11111111111111111111110000000000;
    ANALOG_REG->TSENS_THRESHOLD &= ~TSENS_TRESHOLD_HI_M;
    ANALOG_REG->TSENS_THRESHOLD |= ((uint32_t)data << TSENS_TRESHOLD_HI_S);
}

/*!
@brief  Функция установки значения LOW_THRESHOLD
@param  значение LOW_THRESHOLD (uint16_t)
@return нет
*/
void HAL_TSENS_SetLowThreshold(int32_t data)
{
    HAL_TSENS_SetLowThresholdRaw(TSENS_CELSIUS_TO_VALUE(data));
}

/*!
@brief  Функция установки значения HI_THRESHOLD
@param  значение HI_THRESHOLD (uint16_t)
@return нет
*/
void HAL_TSENS_SetHiThreshold(int32_t data)
{
    HAL_TSENS_SetHiThresholdRaw(TSENS_CELSIUS_TO_VALUE(data));
}

/*!
@brief  Функция разрешения прерываний по флагу LOW_THRESHOLD_IRQ
@param  нет
@return нет
*/
void HAL_TSENS_LowIrq_Enable()
{
    ANALOG_REG->TSENS_IRQ |= TSENS_IRQ_LOW_MASK_M;
}

/*!
@brief  Функция запрета прерываний по флагу LOW_THRESHOLD_IRQ
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
HAL_StatusTypeDef HAL_TSENS_LowIrq_Event()
{
    if ((ANALOG_REG->TSENS_IRQ & TSENS_IRQ_LOW_IRQ_M) != 0) return HAL_OK;
    else return HAL_ERROR;
}

/*!
@brief  Функция разрешения прерываний по флагу HI_THRESHOLD_IRQ
@param  нет
@return нет
*/
void HAL_TSENS_HiIrq_Enable()
{
    ANALOG_REG->TSENS_IRQ |= TSENS_IRQ_HI_MASK_M;
}

/*!
@brief  Функция запрета прерываний по флагу HI_THRESHOLD_IRQ
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
HAL_StatusTypeDef HAL_TSENS_HiIrq_Event()
{
    if ((ANALOG_REG->TSENS_IRQ & TSENS_IRQ_HI_IRQ_M) != 0) return HAL_OK;
    else return HAL_ERROR;
}

/*!
@brief  Функция разрешения прерываний по флагу EOC_IRQ
@param  нет
@return нет
*/
void HAL_TSENS_EOCIrq_Enable()
{
    ANALOG_REG->TSENS_IRQ |= TSENS_IRQ_EOC_MASK_M;
}

/*!
@brief  Функция запрета прерываний по флагу EOC_IRQ
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
HAL_StatusTypeDef HAL_TSENS_EOCIrq_Event()
{
    if ((ANALOG_REG->TSENS_IRQ & TSENS_IRQ_EOC_IRQ_M) != 0) return HAL_OK;
    else return HAL_ERROR;
}

// Reading data
/*!
@brief  Функция чтения данных термосенсора в режиме Continious-mode
@param  нет
@return температура в градусах Цельсия
*/
int32_t HAL_TSENS_ReadMeasurement()
{
    uint32_t value = ANALOG_REG->TSENS_VALUE & 0x3FF;
    //return TSENS_VALUE_TO_CELSIUS(value);
//TODO: упростить формулы
#ifdef MIK32V0
    return (61920*(value)/10240*10-27315)/10;
#endif
#ifdef MIK32V2
    return ((640660*(value))/(40960+93*(value))*10-27315)/10;
#endif
}

/*!
@brief  Функция чтения данных термосенсора в режиме Single-mode
@param  нет
@return температура в градусах Цельсия
*/
int32_t HAL_TSENS_SingleMeasurement()
{
    ANALOG_REG->TSENS_SINGLE = 1;
    while (HAL_TSENS_EOCIrq_Event()); //< Ответ в формате HAL; HAL_OK = 0, HAL_ERROR = 1
    return HAL_TSENS_ReadMeasurement();
}
