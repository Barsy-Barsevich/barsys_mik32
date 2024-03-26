#include "mik32_hal_timer16.h"
#include "mik32_hal_scr1_timer.h"

#include "xprintf.h"
#include "uart_lib.h"
#include "stdlib.h"

#include "Servo32.h"


SCR1_TIMER_HandleTypeDef scr;
Timer16_HandleTypeDef htimer16_1;

void SystemClock_Config(void);
void GPIO_Init();
static void SCR1_TIMER_Init();
static void Timer16_1_Init(void);


/*
 * В данном примере демонстрируется работа таймера32.
 * Нулевой канал таймера используется в режиме ШИМ. На Port0_0 генерирует периодичный сигнал (ШИМ)
 * с заполнением 50 %, частота которого задается верхним пределом таймера. Таймер считает от частоты
 * системной шины, поэтому для установки частоты нужно передать по UART0 значение: 
 * top = (частота основного генератора) / (задаваемая частота)
 */

int main()
{
    SystemClock_Config();
    SCR1_TIMER_Init();
    GPIO_Init();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M | UART_CONTROL1_RE_M, 0, 0);
    xprintf("UART init\n");

    /* Timer16 initializing */
    //Timer16_1_Init();
    /* Запуск таймера с ШИМ сигналом */
    //HAL_Timer16_StartPWM(&htimer16_1, 40000, 3000);
    /*while(1)
    {
        HAL_Timer16_SetCMP(&htimer16_1, 3000);
        HAL_DelayMs(&scr, 1000);
        HAL_Timer16_SetCMP(&htimer16_1, 5000);
        HAL_DelayMs(&scr, 1000);
    }*/

    Servo_ini(ServoGPIO0_10);
    while (1)
    {
        Servo32_PrintAngle(ServoGPIO0_10, 180);
        HAL_DelayMs(&scr, 1000);
        Servo32_PrintAngle(ServoGPIO0_10, 0);
        HAL_DelayMs(&scr, 1000);
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

void GPIO_Init()
{
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();
}


static void SCR1_TIMER_Init()
{
    scr.Instance = SCR1_TIMER;
    scr.ClockSource = SCR1_TIMER_CLKSRC_INTERNAL;
    scr.Divider = 0;
    HAL_SCR1_Timer_Init(&scr);
}


static void Timer16_1_Init(void)
{
    htimer16_1.Instance = TIMER16_0;

    /* Настройка тактирования */
    htimer16_1.Clock.Source = TIMER16_SOURCE_INTERNAL_SYSTEM;
    htimer16_1.CountMode = TIMER16_COUNTMODE_INTERNAL;  /* При тактировании от Input1 не имеет значения */
    htimer16_1.Clock.Prescaler = TIMER16_PRESCALER_16;
    htimer16_1.ActiveEdge = TIMER16_ACTIVEEDGE_RISING;  /* Выбирается при тактировании от Input1 */

    /* Настройка режима обновления регистра ARR и CMP */
    htimer16_1.Preload = TIMER16_PRELOAD_AFTERWRITE;

    /* Настройка триггера */
    htimer16_1.Trigger.Source = TIMER16_TRIGGER_TIM1_GPIO1_9; 
    htimer16_1.Trigger.ActiveEdge = TIMER16_TRIGGER_ACTIVEEDGE_SOFTWARE;    /* При использовании триггера значение должно быть отлично от software */
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
}
