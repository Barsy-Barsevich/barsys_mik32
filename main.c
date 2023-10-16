/*#include <mcu32_memory_map.h>
#include <pad_config.h>
#include <gpio.h>
#include <analog_reg.h>
#include <power_manager.h>

#include <uart_lib.h>
#include <xprintf.h>
#include "tsens.h"

#define MY_POINTER		(*(uint32_t*)0x00085000)
#define MY_INT			((uint32_t*)0x100)

void InitClock() 
{
	// включение тактирования GPIO
	PM->CLK_APB_P_SET |= PM_CLOCK_APB_P_GPIO_0_M | PM_CLOCK_APB_P_GPIO_1_M | PM_CLOCK_APB_P_GPIO_0_M;
	// включение тактирования блока для смены режима выводов
 	PM->CLK_APB_M_SET |= PM_CLOCK_APB_M_PAD_CONFIG_M | PM_CLOCK_APB_M_WU_M | PM_CLOCK_APB_M_PM_M;
}//*/

/*int main() {
	InitClock(); // Включение тактирования GPIO

	PAD_CONFIG->PORT_2_CFG |= (1 << (2 * 7)); 	// Установка вывода 7 порта 2 в режим GPIO
	GPIO_2->DIRECTION_OUT = 1 << 7; 				// Установка направления вывода 7 порта 2 на выход
	//PAD_CONFIG->PORT_2_CFG |= (1 << (2 * PIN_BUTTON)); 	// Установка вывода 6 порта 2 в режим GPIO
	//GPIO_2->DIRECTION_IN = 1 << PIN_BUTTON; 			// Установка направления вывода 6 порта 2 на вход
	PM->CLK_APB_P_SET = PM_CLOCK_APB_P_UART_0_M;
	UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);//*/


	/*//(1) Тактирование аналогового блока
	PM->CLK_APB_P_SET = PM_CLOCK_ANALOG_REG_M;

	ANALOG_REG->TSENS_CONFIG = 	(499<<TSENS_DIV)|
								(0<<TSENS_TRIM_S)|
								(1<<TSENS_NRST_S)|
								(1<<TSENS_NPD_CLK_S)|
								(1<<TSENS_NPD_S);
	ANALOG_REG->TSENS_CONTINUOUS = 0x1; //Zapusk continious mode//*/

	//uint32_t temp = ANALOG_REG->TSENS_CONFIG;
	/*while (1)
	{
		uint32_t value = ANALOG_REG->TSENS_VALUE & 0x3FF; //Vydeliajem 10 bit
		xprintf("%X", value);
		xprintf(" ");
		float temperature = ((float)value * 619.2F) / 1024.0F - 273.15F;
		xprintf("%d", (uint32_t)temperature);
		xprintf("\n");
		for (uint32_t i=0; i<100000; i++);
	}//*/

	/*HAL_TSENS_Init();
	while (1)
	{
		xprintf("%d", (uint32_t)HAL_TSENS_SingleMeasurement());
		xprintf("\n");
		for (uint32_t i=0; i<100000; i++);
	}//*/
	
	/*while (1){
		xprintf("Anomalnain\n");
		GPIO_2->OUTPUT |= 1 << 7;   // Установка сигнала вывода 7 порта 2 в высокий уровень
		for (volatile uint32_t i = 0; i < 10000; i++);
		GPIO_2->OUTPUT &= ~(1 << 7); // Установка сигнала вывода 7 порта  в низкий уровень   
		for (volatile uint32_t i = 0; i < 10000; i++);
	}//*/
//}

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
	xprintf("%d", HAL_TSENS_ClockSource(TSENS_SYS_CLK));
	xprintf("\n");

	for (uint32_t i=30000ul; i<110000ul; i+=1000ul)
	{
		Clock_test(i);
	}


	while (1) {
		xprintf("%d", (uint32_t)HAL_TSENS_SingleMeasurement());
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
	uint32_t a = (ANALOG_REG->TSENS_CFG >> 14) & 0x3FF;
	xprintf("%d", a);
	xprintf(" ");
	a += 1;
	xprintf("%d", F_CPU / a);
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