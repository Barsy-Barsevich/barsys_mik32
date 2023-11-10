#include "mik32_hal_pcc.h"
#include "analog_reg.h"
#include "mik32_hal_tsens.h"

#include "uart_lib.h"
#include "xprintf.h"

/*
 * Данный пример демонстрирует работу температурного сенсора в режиме
 * SingleMode
 */
int main()
{
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    /* Инициализация температурного сенсора */
	HAL_TSENS_Init();
	xprintf("Temperature sensor initialization done.\n");
    /* Cycle */
	while (1) {
		xprintf("%d", (uint32_t)HAL_TSENS_SingleMeasurement());
        xprintf("ºC\n");
        for (uint32_t i=0; i<1000000; i++);
	}
}
