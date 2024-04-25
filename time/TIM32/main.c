#include "mik32_hal.h"
#include "mik32_hal_timer32.h"
#include "mik32_hal_irq.h"

#include "uart_lib.h"
#include "xprintf.h"

/*
 * В данном примере демонстрируется работа таймера32 с использованием прерываний.
 * Нулевой канал таймера используется в режиме захвата. На Port0_0 подается периодичный сигнал (ШИМ), частота которого измеряется и выводится в UART0.
 *
 * */

TIMER32_HandleTypeDef htimer32;
struct {
    uint32_t presc;     // AHB * APB_P prescaler
    uint32_t pt;        // Timer prescaler
} HAL_Time_TIM32_Handler;






void SystemClock_Config(void);
void HAL_Delay(uint32_t time_ms);



void HAL_Time_TIM32_Init(TIMER32_TypeDef* timer);
uint32_t HAL_Time_TIM32_Micros();
uint32_t HAL_Time_TIM32_Millis();
void HAL_Time_TIM32_DelayUs(uint32_t time_us);
void HAL_Time_TIM32_DelayMs(uint32_t time_ms);










int main()
{
    SystemClock_Config();
    HAL_Init();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    HAL_Time_TIM32_Init(TIMER32_0);
    uint32_t metka;

    while (1)
    {
        metka = HAL_Time_TIM32_Micros();
        metka = HAL_Time_TIM32_Micros() - metka;
        xprintf("%u\n", metka);
        HAL_Time_TIM32_DelayUs(1000000);
    }
}

void SystemClock_Config(void)
{
    PCC_InitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
    PCC_OscInit.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 128;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}


void HAL_Time_TIM32_Init(TIMER32_TypeDef* timer)
{
    htimer32.Instance = timer;
    htimer32.Top = 0xFFFFFFFF;
    htimer32.Clock.Source = TIMER32_SOURCE_PRESCALER;
    /* Calculate prescaler values */
    uint32_t clock_freq = HAL_PCC_GetSysClockFreq();
    HAL_Time_TIM32_Handler.presc = (PM->DIV_AHB+1) * (PM->DIV_APB_P+1);
    if (clock_freq % (HAL_Time_TIM32_Handler.presc * 1000000UL) != 0)
        HAL_Time_TIM32_Handler.pt = 1;
    else
    {
        uint32_t pt_raw = clock_freq / (HAL_Time_TIM32_Handler.presc * 1000000UL);
        if (pt_raw < 2) HAL_Time_TIM32_Handler.pt = 1;
        else HAL_Time_TIM32_Handler.pt = pt_raw;
    }
    htimer32.Clock.Prescaler = HAL_Time_TIM32_Handler.pt-1;

    htimer32.CountMode = TIMER32_COUNTMODE_FORWARD;
    HAL_Timer32_Init(&htimer32);
    HAL_Timer32_Value_Clear(&htimer32);
    HAL_Timer32_Start(&htimer32);
}


uint32_t HAL_Time_TIM32_Micros()
{
    uint32_t clock_freq = HAL_PCC_GetSysClockFreq();
    return (uint32_t)((uint64_t)htimer32.Instance->VALUE * (1000000UL *
        HAL_Time_TIM32_Handler.presc * HAL_Time_TIM32_Handler.pt) / clock_freq);
}

uint32_t HAL_Time_TIM32_Millis()
{
    uint32_t clock_freq = HAL_PCC_GetSysClockFreq();
    return (uint32_t)((uint64_t)htimer32.Instance->VALUE * (1000UL *
        HAL_Time_TIM32_Handler.presc * HAL_Time_TIM32_Handler.pt) / clock_freq);
}

void HAL_Time_TIM32_DelayUs(uint32_t time_us)
{
    uint32_t metka;
    metka = HAL_Time_TIM32_Micros();
    while ((HAL_Time_TIM32_Micros() - metka) < time_us);
}

void HAL_Time_TIM32_DelayMs(uint32_t time_ms)
{
    uint32_t metka;
    metka = HAL_Time_TIM32_Millis();
    while ((HAL_Time_TIM32_Millis() - metka) < time_ms);
}






















void HAL_Delay(uint32_t time_ms)
{
    uint32_t temp;
    asm volatile(
        "cycle_main_%=:"
        //"lui    %[temp], 1"                         "\n\t" //4096
        //"addi   %[temp], %[temp], 1235"             "\n\t" //1235
//        "li     %[temp], 5333"                      "\n\t" // 32000/6 ticks
        "li     %[temp], 888"                      "\n\t" // 32000/6 ticks
        "cycle_internal_%=:"
        "addi   %[temp], %[temp], -1"               "\n\t"
        "bne    %[temp], x0, cycle_internal_%="     "\n\t"
        "addi   %[count], %[count], -1"             "\n\t"
        "bne    %[count], x0, cycle_main_%="        "\n\t"
        ::
        [count] "r" (time_ms),
        [temp] "r" (temp)
    );
}
