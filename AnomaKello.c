#include "mik32_hal.h"
#include "mik32_hal_timer16.h"
#include "mik32_hal_irq.h"

#include "uart_lib.h"
#include "xprintf.h"

#define SEG_A_PORT      GPIO_1
#define SEG_B_PORT      GPIO_1
#define SEG_C_PORT      GPIO_1
#define SEG_D_PORT      GPIO_1
#define SEG_E_PORT      GPIO_1
#define SEG_F_PORT      GPIO_1
#define SEG_G_PORT      GPIO_1
#define SEG_DP_PORT     GPIO_1
#define SEG_A_PIN       GPIO_PIN_7
#define SEG_B_PIN       GPIO_PIN_6
#define SEG_C_PIN       GPIO_PIN_5
#define SEG_D_PIN       GPIO_PIN_4
#define SEG_E_PIN       GPIO_PIN_3
#define SEG_F_PIN       GPIO_PIN_2
#define SEG_G_PIN       GPIO_PIN_1
#define SEG_DP_PIN      GPIO_PIN_0
#define CAT_0_PORT      GPIO_1
#define CAT_1_PORT      GPIO_1
#define CAT_2_PORT      GPIO_1
#define CAT_3_PORT      GPIO_1
#define CAT_4_PORT      GPIO_1
#define CAT_5_PORT      GPIO_1
#define CAT_0_PIN       GPIO_PIN_12
#define CAT_1_PIN       GPIO_PIN_11
#define CAT_2_PIN       GPIO_PIN_10
#define CAT_3_PIN       GPIO_PIN_13
#define CAT_4_PIN       GPIO_PIN_14
#define CAT_5_PIN       GPIO_PIN_15








Timer16_HandleTypeDef htimer16_1;

void SystemClock_Config(void);
static void Timer16_1_Init(void);
void GPIO_Init();

void SegmentPrint(uint8_t data);
void DisplayOff();
void DisplayOn(uint8_t razr);
uint8_t BCDtoSEG7(uint8_t bcd);

/*
 * Пример для платы BOARD_LITE
 * В данном примере демонстрируется работа прерываний TIMER16_1. 
 * Пользовательская кнопка на выводе PORT2_6 соединена с выводом PORT1_9, фронт на котором 
 * является триггером для запуска отсчета TIMER16. При достижении значения сравнения или автозагрузки
 * срабатывает прерывание. Светодиод, который подключен к выводу PORT2_7, меняет свое состояние при 
 * совпадении счетчика со значением сравнения. На выводе PORT0_10 генерируется ШИМ сигнал.
 *
 * */

int main()
{    
    HAL_Init();
    
    SystemClock_Config();

    GPIO_Init();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    Timer16_1_Init();

    // /* Задать значение для сравнения */
    // HAL_Timer16_SetCMP(&htimer16_1, htimer16_1.Period/2); 

    /* Включать прерывания Timer16 рекомендуется после его инициализации */
    HAL_EPIC_MaskLevelSet(HAL_EPIC_TIMER16_1_MASK); 
    HAL_IRQ_EnableInterrupts();

    /* Запуск таймера в одиночном или продолжительном режиме */
    HAL_Timer16_Counter_Start(&htimer16_1, 60000);

    /*****************Запуск таймера в одиночном или продолжительном режиме*****************/
    // HAL_Timer16_StartSingleMode(&htimer16_1); /* Одиночный режим */
    //HAL_Timer16_StartContinuousMode(&htimer16_1); /* Продолжительный режим */
    /***************************************************************************************/

    /* Запуск таймера с ШИМ сигналом */
    HAL_Timer16_StartPWM_IT(&htimer16_1, 0xFFFF, 0xFFFF/3);
    
    while (1)
    {    
        xprintf("Counter = %d\n", HAL_Timer16_GetCounterValue(&htimer16_1));
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


static void Timer16_1_Init(void)
{
    htimer16_1.Instance = TIMER16_1;
    /* Настройка тактирования */
    htimer16_1.Clock.Source = TIMER16_SOURCE_INTERNAL_SYSTEM;
    htimer16_1.CountMode = TIMER16_COUNTMODE_INTERNAL;  /* При тактировании от Input1 не имеет значения */
    htimer16_1.Clock.Prescaler = TIMER16_PRESCALER_128;
    htimer16_1.ActiveEdge = TIMER16_ACTIVEEDGE_RISING;  /* Выбирается при тактировании от Input1 */
    /* Настройка режима обновления регистра ARR и CMP */
    htimer16_1.Preload = TIMER16_PRELOAD_AFTERWRITE;
    /* Настройка триггера */
    htimer16_1.Trigger.Source = TIMER16_TRIGGER_TIM1_GPIO1_9; 
    htimer16_1.Trigger.ActiveEdge = TIMER16_TRIGGER_ACTIVEEDGE_RISING;    /* При использовании триггера значение должно быть отлично от software */
    htimer16_1.Trigger.TimeOut = TIMER16_TIMEOUT_DISABLE;   /* Разрешить повторное срабатывание триггера */
    /* Настройки фильтра */
    htimer16_1.Filter.ExternalClock = TIMER16_FILTER_NONE;
    htimer16_1.Filter.Trigger = TIMER16_FILTER_NONE;
    /* Настройка режима энкодера */
    htimer16_1.EncoderMode = TIMER16_ENCODER_DISABLE;
    /* Выходной сигнал */
    htimer16_1.Waveform.Enable = TIMER16_WAVEFORM_GENERATION_ENABLE;
    htimer16_1.Waveform.Polarity = TIMER16_WAVEFORM_POLARITY_NONINVERTED;
    HAL_Timer16_Init(&htimer16_1);
    /* ARR setting */
    //HAL_Timer16_SetCMP(&htimer16_1, 59999);
    //HAL_Timer16_SetARR(&htimer16_1, 60000);
    //HAL_Timer16_ClearInterruptMask(&htimer16_1, 0x11111111);
    HAL_Timer16_SetInterruptARRM(&htimer16_1);
}

void trap_handler()
{
    if (EPIC_CHECK_TIMER16_1())
    {
        uint32_t interrupt_status = HAL_Timer16_GetInterruptStatus(&htimer16_1);

        if (interrupt_status & TIMER16_ISR_DOWN_M)
        {
            xprintf("\nDOWN_IRQ\n");
            /* code */
            // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_DOWN_IRQ); /* Сброс флага прерывания */
        }

        if (interrupt_status & TIMER16_ISR_UP_M)
        {
            xprintf("\nUP_IRQ\n");
            /* code */
            // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_UP_IRQ); /* Сброс флага прерывания */
        }

        if (interrupt_status & TIMER16_ISR_ARR_OK_M)
        {
            xprintf("\nARROK_IRQ\n");
            /* code */
            // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_ARROK_IRQ); /* Сброс флага прерывания */
        }

        if (interrupt_status & TIMER16_ISR_CMP_OK_M)
        {
            xprintf("\nCMPOK_IRQ\n");
            /* code */
            // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_CMPOK_IRQ); /* Сброс флага прерывания */
        }  

        if (interrupt_status & TIMER16_ISR_EXT_TRIG_M)
        {
            xprintf("\nEXTTRIG_IRQ\n");
            /* code */
            // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_EXTTRIG_IRQ); /* Сброс флага прерывания */
        }  

        if (interrupt_status & TIMER16_ISR_ARR_MATCH_M)
        {
            xprintf("\nARRM_IRQ\n");
            /* code */
            //HAL_GPIO_TogglePin(GPIO_2, GPIO_PIN_7);
            // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_ARRM_IRQ); /* Сброс флага прерывания */
        }  

        if (interrupt_status & TIMER16_ISR_CMP_MATCH_M)
        {
            xprintf("\nCMPM_IRQ\n");
            HAL_GPIO_TogglePin(GPIO_2, GPIO_PIN_7); /* Смена сигнала PORT2_7 на противоположный */
            // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_CMPM_IRQ); /* Сброс флага прерывания */
        }  

        HAL_Timer16_ClearInterruptMask(&htimer16_1, 0xFFFFFFFF); /* Сброс нескольких флагов прерываний по маске */
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

    HAL_GPIO_PinConfig(SEG_A_PORT, SEG_A_PIN, __OUTPUT, __PULL_NONE, __DS_8MA);
    HAL_GPIO_PinConfig(SEG_B_PORT, SEG_B_PIN, __OUTPUT, __PULL_NONE, __DS_8MA);
    HAL_GPIO_PinConfig(SEG_C_PORT, SEG_C_PIN, __OUTPUT, __PULL_NONE, __DS_8MA);
    HAL_GPIO_PinConfig(SEG_D_PORT, SEG_D_PIN, __OUTPUT, __PULL_NONE, __DS_8MA);
    HAL_GPIO_PinConfig(SEG_E_PORT, SEG_E_PIN, __OUTPUT, __PULL_NONE, __DS_8MA);
    HAL_GPIO_PinConfig(SEG_F_PORT, SEG_F_PIN, __OUTPUT, __PULL_NONE, __DS_8MA);
    HAL_GPIO_PinConfig(SEG_G_PORT, SEG_G_PIN, __OUTPUT, __PULL_NONE, __DS_8MA);
    HAL_GPIO_PinConfig(SEG_DP_PORT, SEG_DP_PIN, __OUTPUT, __PULL_NONE, __DS_8MA);
    HAL_GPIO_PinConfig(CAT_0_PORT, CAT_0_PIN, __OUTPUT, __PULL_NONE, __DS_8MA);
    HAL_GPIO_PinConfig(CAT_1_PORT, CAT_1_PIN, __OUTPUT, __PULL_NONE, __DS_8MA);
    HAL_GPIO_PinConfig(CAT_2_PORT, CAT_2_PIN, __OUTPUT, __PULL_NONE, __DS_8MA);
    HAL_GPIO_PinConfig(CAT_3_PORT, CAT_3_PIN, __OUTPUT, __PULL_NONE, __DS_8MA);
    HAL_GPIO_PinConfig(CAT_4_PORT, CAT_4_PIN, __OUTPUT, __PULL_NONE, __DS_8MA);
    HAL_GPIO_PinConfig(CAT_5_PORT, CAT_5_PIN, __OUTPUT, __PULL_NONE, __DS_8MA);
}














void SegmentPrint(uint8_t data)
{
    HAL_GPIO_WritePin(SEG_A_PORT, SEG_A_PIN, (data>>7)&1);
    HAL_GPIO_WritePin(SEG_B_PORT, SEG_B_PIN, (data>>6)&1);
    HAL_GPIO_WritePin(SEG_C_PORT, SEG_C_PIN, (data>>5)&1);
    HAL_GPIO_WritePin(SEG_D_PORT, SEG_D_PIN, (data>>4)&1);
    HAL_GPIO_WritePin(SEG_E_PORT, SEG_E_PIN, (data>>3)&1);
    HAL_GPIO_WritePin(SEG_F_PORT, SEG_F_PIN, (data>>2)&1);
    HAL_GPIO_WritePin(SEG_G_PORT, SEG_G_PIN, (data>>1)&1);
    HAL_GPIO_WritePin(SEG_DP_PORT, SEG_DP_PIN, (data>>0)&1);
}
void DisplayOff()
{
    HAL_GPIO_WritePin(CAT_0_PORT, CAT_0_PIN, 1);
    HAL_GPIO_WritePin(CAT_1_PORT, CAT_1_PIN, 1);
    HAL_GPIO_WritePin(CAT_2_PORT, CAT_2_PIN, 1);
    HAL_GPIO_WritePin(CAT_3_PORT, CAT_3_PIN, 1);
    HAL_GPIO_WritePin(CAT_4_PORT, CAT_4_PIN, 1);
    HAL_GPIO_WritePin(CAT_5_PORT, CAT_5_PIN, 1);
}
void DisplayOn(uint8_t razr)
{
    razr &= 0b00000111;
    uint8_t data = ~(1<<razr);
    HAL_GPIO_WritePin(CAT_0_PORT, CAT_0_PIN, (data>>0)&1);
    HAL_GPIO_WritePin(CAT_1_PORT, CAT_1_PIN, (data>>1)&1);
    HAL_GPIO_WritePin(CAT_2_PORT, CAT_2_PIN, (data>>2)&1);
    HAL_GPIO_WritePin(CAT_3_PORT, CAT_3_PIN, (data>>3)&1);
    HAL_GPIO_WritePin(CAT_4_PORT, CAT_4_PIN, (data>>4)&1);
    HAL_GPIO_WritePin(CAT_5_PORT, CAT_5_PIN, (data>>5)&1);
}
uint8_t BCDtoSEG7(uint8_t bcd)
{
    bcd &= 0x0F;
    switch (bcd) {
        case 0: return 0b11111100;
        case 1: return 0b01100000;
        case 2: return 0b11011010;
        case 3: return 0b11110010;
        case 4: return 0b01100110;
        case 5: return 0b10110110;
        case 6: return 0b10111110;
        case 7: return 0b11100000;
        case 8: return 0b11111110;
        case 9: return 0b11110110;
        default: return 0b00000010;
    }
}
