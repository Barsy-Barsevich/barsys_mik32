#include "mik32_hal_pcc.h"
#include "mik32_hal_gpio.h"
#include "analog_reg.h"
#include "mik32_hal_tsens.h"

#include "uart_lib.h"
#include "xprintf.h"

/*
 * Данный пример демонстрирует работу с GPIO и PAD_CONFIG.
 * В примере настраивается вывод, который подключенный к светодиоду, в режим GPIO.
 *
 * Плата выбирается ниже в #define
 */

/* Тип платы */
#define BOARD_LITE
// #define BOARD_DIP

void Clock_test(uint32_t);
void SystemClock_Config();
void GPIO_Init();

int main()
{
    SystemClock_Config();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    GPIO_Init();
    
	HAL_TSENS_Init();
	//xprintf("%d", HAL_TSENS_ClockSource(HAL_TSENS_LSI32K));//HAL_TSENS_SYS_CLK));
	xprintf("\n");

	for (uint32_t i=30000ul; i<110000ul; i+=1000ul)
	{
		Clock_test(i);
	}


	while (1) {
		xprintf("%d", (uint32_t)HAL_TSENS_SingleMeasurement());
        //HAL_TSENS_ContiniousOn();
        //xprintf("%d", HAL_TSENS_ReadMeasurement());
		xprintf("\n");
		for (uint32_t i=0; i<1000000; i++);
	}
}

void Clock_test(uint32_t freq)
{
	xprintf("%d", freq / 1000);
	xprintf("kHz: ");
	xprintf("%d", HAL_TSENS_Clock(freq));
	xprintf(" ");
	uint32_t a = (ANALOG_REG->TSENS_CFG >> TSENS_CFG_DIV_S) & 0x3FF;
	xprintf("%d", a);
	xprintf(" ");
	a += 1;
	uint32_t f_real = 0;
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
    }
	xprintf("%d", (f_real / a));
	xprintf("\n");
}

void SystemClock_Config()
{
    __HAL_PCC_PM_CLK_ENABLE();
    __HAL_PCC_WU_CLK_ENABLE();
    __HAL_PCC_PAD_CONFIG_CLK_ENABLE();

    PCC_OscInitTypeDef PCC_OscInit = {0};
    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 128;
    PCC_OscInit.RTCClockSelection = PCC_RTCCLKSOURCE_NO_CLK;
    PCC_OscInit.RTCClockCPUSelection = PCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_PCC_OscConfig(&PCC_OscInit);
}

void GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();
    __HAL_PCC_GPIO_IRQ_CLK_ENABLE();

#ifdef BOARD_LITE
    GPIO_InitStruct.Pin = PORT2_7;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_2, &GPIO_InitStruct);
#endif

#ifdef BOARD_DIP
    GPIO_InitStruct.Pin = PORT0_3;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = PORT1_3;
    HAL_GPIO_Init(GPIO_1, &GPIO_InitStruct);
#endif
}
