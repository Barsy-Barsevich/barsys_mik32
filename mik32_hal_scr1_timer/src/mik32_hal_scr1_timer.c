#include "mik32_hal_scr1_timer.h"

/*
 * Функция разрешения работы системного таймера
 * Ввод: адрес смещения для локальных переменных
 * Вывод: нет
 */
void HAL_SCR1_Timer_Enable()
{
    SCR1_TIMER->TIMER_CTRL |= SCR1_TIMER_ENABLE_M;
}

/*
 * Функция запрета работы системного таймера
 * Ввод: адрес смещения для локальных переменных
 * Вывод: нет
 */
void HAL_SCR1_Timer_Disable()
{
    SCR1_TIMER->TIMER_CTRL &= ~SCR1_TIMER_ENABLE_M;
}

/*
 * Функция установки источника тактирования системного таймера
 * Ввод: адрес смещения для локальных переменных, источник тактирования (0 или 1)
 * Вывод: нет
 */
void HAL_SCR1_Timer_SetClockSource(uint8_t ClockSource)
{
    switch (ClockSource)
    {
    case SCR1_TIMER_CLKSRC_INTERNAL:
        SCR1_TIMER->TIMER_CTRL &= SCR1_TIMER_CLKSRC_INTERNAL_M;
        break;
    
    case SCR1_TIMER_CLKSRC_EXTERNAL_RTC:
        SCR1_TIMER->TIMER_CTRL |= SCR1_TIMER_CLKSRC_RTC_M;
        break;
    }
}

/*
 * Функция делителя частоты для системного таймера
 * Ввод: адрес смещения для локальных переменных, значение делителя (0-1023)
 * Вывод: нет
 */
void HAL_SCR1_Timer_SetDivider(uint16_t Divider)
{
    /* Divider 10-битное число */
    if(Divider > 1023)
    {
        Divider = 1023;
    }
    SCR1_TIMER->TIMER_DIV = Divider;
}

// /*
//  * Функция инициализации системного таймера
//  * Ввод: адрес смещения для локальных переменных
//  * Вывод: нет
//  */
// void HAL_SCR1_Timer_Init(SCR1_TIMER_HandleTypeDef *hscr1_timer)
// {
//     /* Установка адреса смещения регистров */
//     //hscr1_timer->Instance = SCR1_TIMER_BASE_ADDRESS;
//     SCR1_TIMER->TIMER_CTRL = 0;
//     /* Настройка источника тактирования */
//     HAL_SCR1_Timer_SetClockSource(hscr1_timer, hscr1_timer->ClockSource);
//     /* Настройка делителя */
//     HAL_SCR1_Timer_SetDivider(hscr1_timer, hscr1_timer->Divider);
//     /* Разрешение работы таймера */
//     HAL_SCR1_Timer_Enable(hscr1_timer);
// }

/*
 * Функция получения информации о системном времени
 * Ввод: фдрес смещения для локальных переменных
 * Вывод: системное время (uint64_t)
 */
uint64_t HAL_SCR1_Timer_RawTime()
{
    //uint64_t out = ((uint64_t)(hscr1_timer->Instance->MTIMEH) << 32);
    //out |= (uint64_t)(hscr1_timer->Instance->MTIME);
    uint64_t out = ((uint64_t)(SCR1_TIMER->MTIMEH) << 32);
    out |= (uint64_t)(SCR1_TIMER->MTIME);
    return out;
}

/*
 * Функция очистки счетчика машинного времени
 * Ввод: нет
 * Вывод: нет
 */
void HAL_SCR1_Timer_ClearCounter()
{
    //HAL_SCR1_Timer_Disable(hscr1_timer);
    uint32_t bitsave = (SCR1_TIMER->TIMER_CTRL & SCR1_TIMER_ENABLE_M);
    SCR1_TIMER->TIMER_CTRL &= ~SCR1_TIMER_ENABLE_M;
    SCR1_TIMER->MTIME = 0;
    SCR1_TIMER->MTIMEH = 0;
    //HAL_SCR1_Timer_Enable(hscr1_timer);
    SCR1_TIMER->TIMER_CTRL |= bitsave;
}

/*
 * Функция вывода значения машинного времени в миллисекундах
 * Ввод: нет
 * Вывод: время в миллисекундах (uint64_t)
 */
uint64_t HAL_Millis()
{
    uint64_t ticks = HAL_SCR1_Timer_RawTime();
    ticks /= (MIK32_FREQ / 1000UL);
    return ticks;
}

/*
 * Функция вывода значения машинного времени в микросекундах
 * Ввод: нет
 * Вывод: время в микросекундах (uint64_t)
 */
uint64_t HAL_Micros()
{
    uint64_t ticks = HAL_SCR1_Timer_RawTime();
    ticks /= (MIK32_FREQ / 1000000UL);
    return ticks;
}

/*
 * Функция программной задержки (в миллисекундах)
 * Ввод: адрес смещения для локальных переменных, время в миллисекундах
 * Вывод: нет
 */
void HAL_Delay_ms(uint32_t time_ms)
{
    uint64_t ticks = time_ms * (MIK32_FREQ / ((PM->DIV_AHB + 1) * (SCR1_TIMER->TIMER_DIV + 1) * 1000));
    uint64_t metka = HAL_SCR1_Timer_RawTime();
    while ((HAL_SCR1_Timer_RawTime() - metka) < ticks);
}

/*
 * Функция программной задержки (в микросекундах)
 * Ввод: адрес смещения для локальных переменных, время в микросекундах
 * Вывод: нет
 */
void HAL_Delay_us(uint32_t time_us)
{
    uint64_t ticks = time_us * (MIK32_FREQ / ((PM->DIV_AHB + 1) * (SCR1_TIMER->TIMER_DIV + 1) * 1000000));
    uint64_t metka = HAL_SCR1_Timer_RawTime();
    while ((HAL_SCR1_Timer_RawTime() - metka) < ticks);
}













// void HAL_SCR1_TimerStart(SCR1_TIMER_HandleTypeDef *hscr1_timer, uint32_t time_ms)
// {
//     uint64_t ticks = time_ms * (MIK32_FREQ / ((PM->DIV_AHB + 1) * (SCR1_TIMER->TIMER_DIV + 1) * 1000));
//     ticks += 0;
//     uint64_t to_cmp = HAL_SCR1_Timer_RawTime(hscr1_timer) + ticks;
//     hscr1_timer->Instance->MTIMECMPH = (to_cmp >> 32);
//     hscr1_timer->Instance->MTIMECMP = (to_cmp & 0xFFFFFFFF);
// }

