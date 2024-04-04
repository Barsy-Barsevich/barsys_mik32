
#ifndef SERVO32_LIB
#define SERVO32_LIB

#include "mik32_hal_timer32.h"
#include "mik32_hal_timer16.h"

/* Выбор частоты ШИМ-сигнала серво */
#define SERVO_50HZ
//#define SERVO_330HZ

/* Servo 50Hz setting */
/* 50Гц - это стандартная частота серво. Ее использование предпочтительнее */
#ifdef SERVO_50HZ
#define SERVO_TIMER_PRESCALER       16
#define SERVO_TIMER16_PRESCALER     TIMER16_PRESCALER_16
#define SERVO_TIMER_TOP_VALUE       40000
#define SERVO_0DEG_VALUE            1000
#define SERVO_90DEG_VALUE           3000
#define SERVO_180DEG_VALUE          5000
#define SERVO_ANGLE_TO_VALUE_COEF   (SERVO_180DEG_VALUE-SERVO_0DEG_VALUE)/180

//#define SERVO_0DEG_VALUE            TIMER_FREQ/50/40
//#define SERVO_90DEG_VALUE           TIMER_FREQ/50/40*3
//#define SERVO_180DEG_VALUE          TIMER_FREQ/50/40*5

#else
/* Servo 330Hz setting */
/* Некоторые серво могут работать на частоте 330Гц. Эту настройку можно использовать, если необходим максимально быстрый отклик сервопривода на внешнее воздействие */
#ifdef SERVO_330HZ
#define SERVO_TIMER_PRESCALER       2
#define SERVO_TIMER16_PRESCALER     TIMER16_PRESCALER_2
#define SERVO_TIMER_TOP_VALUE       48000
#define SERVO_0DEG_VALUE            8000
#define SERVO_90DEG_VALUE           24000
#define SERVO_180DEG_VALUE          40000
#define SERVO_ANGLE_TO_VALUE_COEF   (SERVO_180DEG_VALUE-SERVO_0DEG_VALUE)/180
#endif
#endif

typedef enum __ServoFrequency_TypeDef
{
    ServoFreq_50Hz = 0,
    ServoFreq_330Hz = 1
} ServoFrequency_TypeDef;

typedef enum __HAL_ServoPinTypeDef
{
    ServoPin_0_7 = 0,    //Соответствует каналу tim16_0
    ServoPin_0_10 = 1,   //Соответствует каналу tim16_1
    ServoPin_0_13 = 2,   //Соответствует каналу tim16_2
    ServoPin_0_0 = 3,    //Соответствует каналу tim32_1, ch1
    ServoPin_0_1 = 4,    //Соответствует каналу tim32_1, ch2
    ServoPin_0_2 = 5,    //Соответствует каналу tim32_1, ch3
    ServoPin_0_3 = 6,    //Соответствует каналу tim32_1, ch4
    ServoPin_1_0 = 7,    //Соответствует каналу tim32_2, ch1
    ServoPin_1_1 = 8,    //Соответствует каналу tim32_2, ch2
    ServoPin_1_2 = 9,    //Соответствует каналу tim32_2, ch3
    ServoPin_1_3 = 10    //Соответствует каналу tim32_2, ch4
} HAL_ServoPinTypeDef;

typedef struct __ServoTimerSetting_TypeDef
{
    ServoFrequency_TypeDef servo_freq;
    uint32_t timer_freq;
    uint32_t _0deg_value;
    uint32_t _180deg_value;
    uint32_t angle_to_value_coef;
} ServoTimerSetting_TypeDef;

struct __ServoGlobalSetting
{
    
} ServoGlobalSetting;


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
HAL_StatusTypeDef Servo_Clock_ini();

#include "Servo32.c"

#endif
