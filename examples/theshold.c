#include "mik32_hal_pcc.h"
#include "analog_reg.h"
#include "mik32_hal_tsens.h"
#include "epic.h"

#include "uart_lib.h"
#include "xprintf.h"

// Верхний предел температуры
#define HIGH_BORDER     30 //Celsium degrees
// Нижный предел температуры
#define LOW_BORDER      20 //Celsium degrees

/*
 * Данный пример демонстрирует работу перрывний температурного сенсора
 * при превышении пределов температуры
 */
int main()
{
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    /* Инициализация температурного сенсора */
	HAL_TSENS_Init();

    // HAL_TSENS_SetLowThresholdRaw(TSENS_CELSIUS_TO_VALUE(LOW_BORDER));
    // HAL_TSENS_SetHiThresholdRaw(TSENS_CELSIUS_TO_VALUE(HIGH_BORDER));
    HAL_TSENS_SetHiThreshold(HIGH_BORDER);
    HAL_TSENS_SetLowThreshold(LOW_BORDER);
    // xprintf("Alku:\n");
    // xprintf("%d", TSENS_CELSIUS_TO_VALUE(HIGH_BORDER));
    // xprintf("\n");
    // xprintf("%d", (40960ul/(6406600ul/(HIGH_BORDER*100+27315)-93)));
    // xprintf("\n");
    HAL_TSENS_LowIrq_Enable();
    HAL_TSENS_HiIrq_Enable();
    __HAL_PCC_EPIC_CLK_ENABLE();
    EPIC->MASK_LEVEL_SET |= (1<<EPIC_LINE_TSENS_S);

	xprintf("Temperature sensor initialization done.\n");
    /* Cycle */
	while (1) {
		xprintf("%d", (uint32_t)HAL_TSENS_SingleMeasurement());
        xprintf("ºC\tint status: ");
        //xprintf("%X", EPIC->RAW_STATUS);
        if ((EPIC->RAW_STATUS & (1<<EPIC_LINE_TSENS_S)) != 0)
        {
            xprintf("1\n");
        }
        else
        {
            xprintf("0\n");
        }

        /* Очистка флагов прерываний */
        HAL_TSENS_HiIrq_Clear();
        HAL_TSENS_LowIrq_Clear();
        for (uint32_t i=0; i<1000000; i++);
	}
}
