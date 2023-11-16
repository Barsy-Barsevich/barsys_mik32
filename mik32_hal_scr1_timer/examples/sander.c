#include "mik32_hal_pcc.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_scr1_timer.h"

#include "uart_lib.h"
#include "xprintf.h"

//SCR1_TIMER_HandleTypeDef local;


// typedef enum
// {
//     MSTATUS_ADDR        0x300
//     MISA_ADDR           0x301
//     MIE_ADDR            0x304
//     MTVEC_ADDR          0x305
// } Ma_chine_TypeDef;

// #define MSTATUS             ((uint32_t*)MSTATUS_ADDR)
// #define MISA                ((uint32_t*)MISA_ADDR)
// #define MIE                 ((MIE_TypeDef*)0x304)
// #define MTVEC               ((uint32_t*)MTVEC_ADDR)

#ifndef _ASSEMBLER_
    #include <inttypes.h>

    typedef struct
    {
        uint32_t LAP;        
    } MSTATUS_TypeDef;
#endif
#ifndef _ASSEMBLER_
    #include <inttypes.h>

    typedef struct
    {
        uint32_t LAP;        
    } MIE_TypeDef;
#endif
#ifndef _ASSEMBLER_
    #include <inttypes.h>

    typedef struct
    {
        uint32_t LAP;        
    } MIP_TypeDef;
#endif

#define MSTATUS             ((MSTATUS_TypeDef*)0x300)
#define MIE                 ((MIE_TypeDef*)0x304)
#define MIP                 ((MIP_TypeDef*)0x345)

void SystemClock_Config();
//void GPIO_Init();

int main()
{
    //HAL_Init();
    SystemClock_Config();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    //GPIO_Init();

    SCR1_TIMER->MTIME = 0;
    SCR1_TIMER->MTIMEH = 0;
    SCR1_TIMER->MTIMECMP = 32000000ul;
    SCR1_TIMER->MTIMECMPH = 0;
    HAL_SCR1_Timer_SetDivider(0);
    HAL_SCR1_Timer_Enable();
    //HAL_SCR1_Timer_Init(&local);
    xprintf("start\n");
    /*//MSTATUS->LAP &= ~MSTATUS_MIE;
    //MIE->LAP |= MIE_MTIE;
    MIE->LAP = 0;*/
    while (1)
    {
        /*uint64_t metka = HAL_SCR1_Timer_RawTime(&local);
        xprintf("%u", (uint32_t)(metka / 32000000ul));
        xprintf("\t");
        xprintf("%X", ((MIP->LAP) & MIE_MTIE));
        //xprintf("%X", (MSTATUS->LAP & MSTATUS_MIE));
        xprintf("\n");
        //HAL_Delay_ms(&local, 500);
        //MIE->LAP |= MIE_MTIE;*/

        xprintf("%u", ((uint32_t)HAL_Micros()));
        xprintf("\n");
        HAL_Delay_ms(100);
    }
}

void trap_handler() {
    xprintf("Anomalnain");
    xprintf("%X", ((MIP->LAP) & (1<<7)));
    xprintf("%X", (MSTATUS->LAP & MSTATUS_MIE));
    xprintf("\n");
    SCR1_TIMER->MTIMECMP = 0xffffffff;
    MIE->LAP &= ~MIE_MTIE;
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
