
#include "mik32_hal.h"
#include "mik32_hal_timer16.h"
#include "mik32_hal_irq.h"

#include "uart_lib.h"
#include "xprintf.h"









Timer16_HandleTypeDef time_timer16;
struct {
    uint32_t presc;     // AHB * APB_P prescaler
    uint32_t pt;        // Timer prescaler
    volatile uint32_t ticks;     // Time in ticks, updated by interrupts
} HAL_Time_TIM16_Handler;

void SystemClock_Config(void);
void HAL_Time_TIM16_Init(TIMER16_TypeDef* timer);
uint32_t HAL_Time_TIM16_Micros();
uint32_t HAL_Time_TIM16_Millis();
void HAL_Time_TIM16_DelayUs(uint32_t time_us);
void HAL_Time_TIM16_DelayMs(uint32_t time_ms);
void GPIO_Init();













int main()
{    
    HAL_Init();
    
    SystemClock_Config();

    GPIO_Init();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    HAL_Time_TIM16_Init(TIMER16_0);



    uint32_t cur, prev;
    // metka = HAL_Time_TIM16_Micros();
    // while (HAL_Time_TIM16_Micros() - metka < 100000UL);


    // xprintf("%u\n", HAL_Time_TIM16_Micros());
    // while(1);
    cur = HAL_Time_TIM16_Micros();
    prev = cur;

    while (1)
    {
        HAL_IRQ_DisableInterrupts();
        xprintf("%u\n", prev/*HAL_Time_TIM16_Micros()*/);
        HAL_IRQ_EnableInterrupts();
        //for (uint32_t i=0; i<1000; i++);
        while (HAL_Time_TIM16_Micros() - prev < 1000000UL);// cur = HAL_Time_TIM16_GetTick();
        prev = HAL_Time_TIM16_Micros();//cur;
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







void HAL_Time_TIM16_Init(TIMER16_TypeDef* timer)
{
    time_timer16.Instance = timer;
    time_timer16.Clock.Source = TIMER16_SOURCE_INTERNAL_SYSTEM;
    time_timer16.CountMode = TIMER16_COUNTMODE_INTERNAL;
    time_timer16.ActiveEdge = TIMER16_ACTIVEEDGE_RISING;
    time_timer16.Preload = TIMER16_PRELOAD_AFTERWRITE;
    time_timer16.Trigger.Source = TIMER16_TRIGGER_TIM1_GPIO1_9; 
    time_timer16.Trigger.ActiveEdge = TIMER16_TRIGGER_ACTIVEEDGE_SOFTWARE;
    time_timer16.Trigger.TimeOut = TIMER16_TIMEOUT_DISABLE;
    time_timer16.Filter.ExternalClock = TIMER16_FILTER_NONE;
    time_timer16.Filter.Trigger = TIMER16_FILTER_NONE;
    time_timer16.EncoderMode = TIMER16_ENCODER_DISABLE;
    time_timer16.Waveform.Enable = TIMER16_WAVEFORM_GENERATION_DISABLE;
    time_timer16.Waveform.Polarity = TIMER16_WAVEFORM_POLARITY_NONINVERTED;
    /* Calculate prescaler values */
    uint32_t clock_freq = HAL_PCC_GetSysClockFreq();
    HAL_Time_TIM16_Handler.presc = (PM->DIV_AHB+1) * (PM->DIV_APB_P+1);
    if (clock_freq % (HAL_Time_TIM16_Handler.presc * 1000000UL) != 0)
    {
        HAL_Time_TIM16_Handler.pt = 1;
        time_timer16.Clock.Prescaler = TIMER16_PRESCALER_1;
    }
    else
    {
        uint32_t pt_raw = clock_freq / (HAL_Time_TIM16_Handler.presc * 1000000UL);
        switch (pt_raw)
        {
            case 1: HAL_Time_TIM16_Handler.pt = 1; time_timer16.Clock.Prescaler = TIMER16_PRESCALER_1; break;
            case 2: HAL_Time_TIM16_Handler.pt = 2; time_timer16.Clock.Prescaler = TIMER16_PRESCALER_2; break;
            case 4: HAL_Time_TIM16_Handler.pt = 4; time_timer16.Clock.Prescaler = TIMER16_PRESCALER_4; break;
            case 8: HAL_Time_TIM16_Handler.pt = 8; time_timer16.Clock.Prescaler = TIMER16_PRESCALER_8; break;
            case 16: HAL_Time_TIM16_Handler.pt = 16; time_timer16.Clock.Prescaler = TIMER16_PRESCALER_16; break;
            case 32: HAL_Time_TIM16_Handler.pt = 32; time_timer16.Clock.Prescaler = TIMER16_PRESCALER_32; break;
            case 64: HAL_Time_TIM16_Handler.pt = 64; time_timer16.Clock.Prescaler = TIMER16_PRESCALER_64; break;
            case 128: HAL_Time_TIM16_Handler.pt = 128; time_timer16.Clock.Prescaler = TIMER16_PRESCALER_128; break;
            default: HAL_Time_TIM16_Handler.pt = 1; time_timer16.Clock.Prescaler = TIMER16_PRESCALER_1; break;
        }
    }
    HAL_Timer16_Init(&time_timer16);
    /* Reset the timer */
    time_timer16.Instance->CNT = 0;
    HAL_Time_TIM16_Handler.ticks = 0;
    /* Set interrupt mask */
    switch ((uint32_t)timer)
    {
        case (uint32_t)TIMER16_0: HAL_EPIC_MaskLevelSet(HAL_EPIC_TIMER16_0_MASK); break;
        case (uint32_t)TIMER16_1: HAL_EPIC_MaskLevelSet(HAL_EPIC_TIMER16_1_MASK); break;
        case (uint32_t)TIMER16_2: HAL_EPIC_MaskLevelSet(HAL_EPIC_TIMER16_2_MASK); break;
    }
    HAL_IRQ_EnableInterrupts();
    HAL_Timer16_Counter_Start_IT(&time_timer16, 0xFFFF);
}

void trap_handler()
{
    if (EPIC_CHECK_TIMER16_0())
    {
        uint32_t interrupt_status = HAL_Timer16_GetInterruptStatus(&time_timer16);

        if (interrupt_status & TIMER16_ISR_ARR_MATCH_M)
        {
            //xprintf("\nARRM_IRQ\n");
            /* code */
            HAL_Time_TIM16_Handler.ticks += 0x00010000UL;
            HAL_GPIO_TogglePin(GPIO_2, GPIO_PIN_7);
            HAL_GPIO_TogglePin(GPIO_2, GPIO_PIN_7);
            
        }  

        HAL_Timer16_ClearInterruptMask(&time_timer16, 0xFFFFFFFF); /* Сброс нескольких флагов прерываний по маске */
    }


    /* Сброс прерываний */
    HAL_EPIC_Clear(0xFFFFFFFF);
}

void GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_2, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_INPUT;
    HAL_GPIO_Init(GPIO_1, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_INPUT;
    HAL_GPIO_Init(GPIO_2, &GPIO_InitStruct);
}








uint32_t HAL_Time_TIM16_Micros()
{
    uint64_t time_raw = HAL_Time_TIM16_Handler.ticks + time_timer16.Instance->CNT;
    uint32_t clock_freq = HAL_PCC_GetSysClockFreq();
    return (uint32_t)(time_raw * (1000000UL * HAL_Time_TIM16_Handler.presc *
        HAL_Time_TIM16_Handler.pt) / clock_freq);
}

uint32_t HAL_Time_TIM16_Millis()
{
    uint64_t time_raw = HAL_Time_TIM16_Handler.ticks + time_timer16.Instance->CNT;
    uint32_t clock_freq = HAL_PCC_GetSysClockFreq();
    return (uint32_t)(time_raw * (1000UL * HAL_Time_TIM16_Handler.presc *
        HAL_Time_TIM16_Handler.pt) / clock_freq);
}

void HAL_Time_TIM16_DelayUs(uint32_t time_us)
{
    uint32_t time_metka = HAL_Time_TIM16_Micros();
    while ((HAL_Time_TIM16_Micros() - time_metka) < time_us);
}

void HAL_Time_TIM16_DelayMs(uint32_t time_ms)
{
    uint32_t time_metka = HAL_Time_TIM16_Millis();
    while ((HAL_Time_TIM16_Millis() - time_metka) < time_ms);
}
