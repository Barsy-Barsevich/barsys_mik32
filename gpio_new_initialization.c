#include "mik32_hal_pcc.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_scr1_timer.h"


typedef enum __HAL_Des
{
    OUTPUT = HAL_GPIO_MODE_GPIO_OUTPUT,
    INPUT = HAL_GPIO_MODE_GPIO_INPUT
} HAL_Des;
typedef enum __HAL_Pul
{
    PULL_NONE = HAL_GPIO_PULL_NONE,
    PULL_UP = HAL_GPIO_PULL_UP,
    PULL_DOWN = HAL_GPIO_PULL_DOWN
} HAL_Pul;
typedef enum __HAL_Current
{
    _2mA = HAL_GPIO_DS_2MA,
    _4mA = HAL_GPIO_DS_4MA,
    _8mA = HAL_GPIO_DS_8MA
} HAL_Current;


void SystemClock_Config(void);
void HAL_GPIO_INIT(HAL_PinMapNewTypeDef, HAL_Des, HAL_Pul, HAL_Current);


int main()
{
    SystemClock_Config();
    HAL_GPIO_INIT(PORT2_7, OUTPUT, PULL_NONE, _8mA);
    while (1)
    {
        HAL_GPIO_TogglePin(GPIO_2, PORT2_7);
        HAL_Delay_ms(200);
    }
}


void HAL_GPIO_INIT(HAL_PinMapNewTypeDef pin, HAL_Des des, HAL_Pul pul, HAL_Current current)
{
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = des;
    GPIO_InitStruct.Pull = pul;
    GPIO_InitStruct.DS = current;
    HAL_GPIO_Init(GPIO_2, &GPIO_InitStruct);
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
