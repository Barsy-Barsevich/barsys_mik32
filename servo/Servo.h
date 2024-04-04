#ifndef SERVO32_LIB
#define SERVO32_LIB

#include "mik32_hal_timer32.h"
#include "mik32_hal_timer16.h"


typedef enum __ServoFrequency_TypeDef
{
    ServoFreq_50Hz = 0,
    ServoFreq_330Hz = 1
} ServoFrequency_TypeDef;

typedef enum __ServoPinTypeDef
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
} ServoPinTypeDef;


typedef struct __ServoTimerSetting_TypeDef
{
    ServoFrequency_TypeDef servo_freq;
    uint32_t timer_freq;
    uint32_t _0deg_value;
    uint32_t _180deg_value;
    uint32_t angle_to_ocr_coef;
} ServoTimerSetting_TypeDef;


struct __ServoGlobalSetting
{
    /* Timer settings */
    ServoTimerSetting_TypeDef tim16_0_setting;
    ServoTimerSetting_TypeDef tim16_1_setting;
    ServoTimerSetting_TypeDef tim16_2_setting;
    ServoTimerSetting_TypeDef tim32_1_setting;
    ServoTimerSetting_TypeDef tim32_2_setting;
    
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
