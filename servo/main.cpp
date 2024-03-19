#include "mik32_hal_timer32.h"
#include "mik32_hal_scr1_timer.h"

#include "xprintf.h"
#include "uart_lib.h"
#include "stdlib.h"

#include "MPU9250.h"

MPU_HandleTypeDef mpu;
SPI_HandleTypeDef mpu_spi0;

TIMER32_HandleTypeDef htimer32_1;
TIMER32_CHANNEL_HandleTypeDef htimer32_channel0;
SCR1_TIMER_HandleTypeDef scr;

void SystemClock_Config(void);
static void Timer32_1_Init(void);
void GPIO_Init();
static void SPI0_Init(void);
static void SCR1_TIMER_Init();
void Servo32_PrintAngle(int16_t angle);

#define Servo32_Period      100000UL
#define Servo32_1ms_Time    16000UL
#define Servo32_1_5ms_Time  48000UL
#define Servo32_2ms_Time  80000UL
#define SERVO32_180DEG_VALUE  64000

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
    Timer32_1_Init();
    xprintf("UART init\n");
    HAL_Timer32_Channel_Enable(&htimer32_channel0);
    HAL_Timer32_Value_Clear(&htimer32_1);
    HAL_Timer32_Start(&htimer32_1);
    int top = 100000;//640000;
    HAL_Timer32_Top_Set(&htimer32_1, top);
    HAL_Timer32_Value_Clear(&htimer32_1);//*/

    /*Servo32_PrintAngle(70);
    while(1);*/
    while (1)
    {
        for (int8_t i=-90; i<90; i++)
        {
            Servo32_PrintAngle(i);
            HAL_DelayMs(&scr, 20);
        }
        for (int8_t i=90; i>-90; i--)
        {
            Servo32_PrintAngle(i);
            HAL_DelayMs(&scr, 20);
        }
    }

    // SPI0_Init();
    // SPI_init(&mpu, &mpu_spi0, SPI_CS_0);
    // mpu_begin(&mpu);
    // uint8_t data;
    // while (1) SPI_readReg(&mpu, MPU_WHO_I_AM, &data);
    // mpu_setAccelScale(&mpu, ACCEL_SCALE_16G);
    // mpu_setGyroScale(&mpu, GYRO_SCALE_2000);
    // mpu_setAccelBandwidth(&mpu, ACCEL_BANDWIDTH_420Hz);
    // mpu_setGyroBandwidth(&mpu, GYRO_BANDWIDTH_3600Hz);
    // while (1) mpu_readData(&mpu);
}




void Servo32_PrintAngle(int16_t angle)
{
    angle += 90;
    if (angle <= 0) angle = 1;
    if (angle >= 180) angle = 179;
    HAL_Timer32_Channel_OCR_Set(&htimer32_channel0, SERVO32_180DEG_VALUE/180*angle+16000);
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


static void Timer32_1_Init(void)
{
    htimer32_1.Instance = TIMER32_1;
    htimer32_1.Top = 7543;
    htimer32_1.State = TIMER32_STATE_DISABLE;
    htimer32_1.Clock.Source = TIMER32_SOURCE_PRESCALER;
    htimer32_1.Clock.Prescaler = 0;
    htimer32_1.InterruptMask = 0;
    htimer32_1.CountMode = TIMER32_COUNTMODE_FORWARD;
    HAL_Timer32_Init(&htimer32_1);

    htimer32_channel0.TimerInstance = htimer32_1.Instance;
    htimer32_channel0.ChannelIndex = TIMER32_CHANNEL_0;
    htimer32_channel0.PWM_Invert = TIMER32_CHANNEL_NON_INVERTED_PWM;
    htimer32_channel0.Mode = TIMER32_CHANNEL_MODE_PWM;
    htimer32_channel0.CaptureEdge = TIMER32_CHANNEL_CAPTUREEDGE_RISING;
    htimer32_channel0.OCR = 7544 >> 1;
    htimer32_channel0.Noise = TIMER32_CHANNEL_FILTER_OFF;
    HAL_Timer32_Channel_Init(&htimer32_channel0);
}

void GPIO_Init()
{
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();
}

static void SPI0_Init(void)
{
    mpu_spi0.Instance = SPI_0;

    /* Режим SPI */
    mpu_spi0.Init.SPI_Mode = HAL_SPI_MODE_MASTER;

    /* Настройки */
    mpu_spi0.Init.CLKPhase = SPI_PHASE_ON;
    mpu_spi0.Init.CLKPolarity = SPI_POLARITY_HIGH;
    mpu_spi0.Init.ThresholdTX = 4;

    /* Настройки для ведущего */
    mpu_spi0.Init.BaudRateDiv = SPI_BAUDRATE_DIV256;
    mpu_spi0.Init.Decoder = SPI_DECODER_NONE;
    mpu_spi0.Init.ManualCS = SPI_MANUALCS_OFF;
    mpu_spi0.Init.ChipSelect = SPI_CS_0;      

    if (HAL_SPI_Init(&mpu_spi0) != HAL_OK)
    {
        xprintf("SPI_Init_Error\n");
    }
}

static void SCR1_TIMER_Init()
{
    scr.Instance = SCR1_TIMER;
    scr.ClockSource = SCR1_TIMER_CLKSRC_INTERNAL;
    scr.Divider = 0;
    HAL_SCR1_Timer_Init(&scr);
}
