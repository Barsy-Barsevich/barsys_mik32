#ifndef SERVO32_LIB
#define SERVO32_LIB

#include "mik32_hal_timer32.h"
#include "mik32_hal_timer16.h"

/* Выбор частоты ШИМ-сигнала серво */
#define SERVO_50Hz
//#define SERVO_330Hz

/* Servo 50Hz setting */
/* 50Гц - это стандартная частота серво. Ее использование предпочтительнее */
#ifdef SERVO_50Hz
#define SERVO_TIMER_PRESCALER       16
#define SERVO_TIMER_TOP_VALUE       40000
#define SERVO_0DEG_VALUE            1000
#define SERVO_90DEG_VALUE           3000
#define SERVO_180DEG_VALUE          5000
#define SERVO_ANGLE_TO_VALUE_COEF   (SERVO_180DEG_VALUE-SERVO_0DEG_VALUE)/180
#else
/* Servo 330Hz setting */
/* Некоторые серво могут работать на частоте 330Гц. Эту настройку можно использовать, если необходим максимально быстрый отклик сервопривода на внешнее воздействие */
#ifdef SERVO_330Hz
#define SERVO_TIMER_PRESCALER       2
#define SERVO_TIMER_TOP_VALUE       48000
#define SERVO_0DEG_VALUE            8000
#define SERVO_90DEG_VALUE           24000
#define SERVO_180DEG_VALUE          40000
#define SERVO_ANGLE_TO_VALUE_COEF   (SERVO_180DEG_VALUE-SERVO_0DEG_VALUE)/180
#endif
#endif

typedef enum __HAL_ServoPinTypeDef
{
    ServoGPIO0_7 = 0,    //tim16_0
    ServoGPIO0_10 = 1,   //tim16_1
    ServoGPIO0_13 = 2,   //tim16_2
    ServoGPIO0_0 = 3,    //tim32_1, ch1
    ServoGPIO0_1 = 4,    //tim32_1, ch2
    ServoGPIO0_2 = 5,    //tim32_1, ch3
    ServoGPIO0_3 = 6,    //tim32_1, ch4
    ServoGPIO1_0 = 7,    //tim32_2, ch1
    ServoGPIO1_1 = 8,    //tim32_2, ch2
    ServoGPIO1_2 = 9,    //tim32_2, ch3
    ServoGPIO1_3 = 10   //tim32_2, ch4
} HAL_ServoPinTypeDef;


TIMER32_HandleTypeDef servo_timer32_1;
TIMER32_HandleTypeDef servo_timer32_2;
TIMER32_CHANNEL_HandleTypeDef servo_tim32_1_ch0;
TIMER32_CHANNEL_HandleTypeDef servo_tim32_1_ch1;
TIMER32_CHANNEL_HandleTypeDef servo_tim32_1_ch2;
TIMER32_CHANNEL_HandleTypeDef servo_tim32_1_ch3;
TIMER32_CHANNEL_HandleTypeDef servo_tim32_2_ch0;
TIMER32_CHANNEL_HandleTypeDef servo_tim32_2_ch1;
TIMER32_CHANNEL_HandleTypeDef servo_tim32_2_ch2;
TIMER32_CHANNEL_HandleTypeDef servo_tim32_2_ch3;
Timer16_HandleTypeDef servo_timer16_0;
Timer16_HandleTypeDef servo_timer16_1;
Timer16_HandleTypeDef servo_timer16_2;


void Servo_ini(HAL_ServoPinTypeDef pin);
static HAL_StatusTypeDef timer32_ini(TIMER32_TypeDef* timer, uint8_t channel);
static HAL_StatusTypeDef timer16_ini(TIMER16_TypeDef* timer);
void Servo32_PrintAngle(HAL_ServoPinTypeDef pin, uint8_t angle);

#endif
