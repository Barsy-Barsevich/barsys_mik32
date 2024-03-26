

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


//void anm(GPIO_TypeDef *GPIO_x, HAL_PinsTypeDef pin)

void Servo_ini(HAL_ServoPinTypeDef pin)
{
    switch (pin)
    {
        case ServoGPIO0_7:  timer16_ini(TIMER16_0); break;
        case ServoGPIO0_10: timer16_ini(TIMER16_1); break;
        case ServoGPIO0_13: timer16_ini(TIMER16_2); break;
        case ServoGPIO0_0:  timer32_ini(TIMER32_1,0); break;
        case ServoGPIO0_1:  timer32_ini(TIMER32_1,1); break;
        case ServoGPIO0_2:  timer32_ini(TIMER32_1,2); break;
        case ServoGPIO0_3:  timer32_ini(TIMER32_1,3); break;
        case ServoGPIO1_0:  timer32_ini(TIMER32_2,0); break;
        case ServoGPIO1_1:  timer32_ini(TIMER32_2,1); break;
        case ServoGPIO1_2:  timer32_ini(TIMER32_2,2); break;
        case ServoGPIO1_3:  timer32_ini(TIMER32_2,3); break;
    }
}

static HAL_StatusTypeDef timer32_ini(TIMER32_TypeDef* timer, uint8_t channel)
{
    /* Подготовка указателей на структуры таймера и канала */
    TIMER32_HandleTypeDef* local_timer32;
    TIMER32_CHANNEL_HandleTypeDef* local_channel;
    if (timer == TIMER32_1)
    {
        local_timer32 = &servo_timer32_1;
        switch (channel)
        {
            case 0: local_channel = &servo_tim32_1_ch0; break;
            case 1: local_channel = &servo_tim32_1_ch1; break;
            case 2: local_channel = &servo_tim32_1_ch2; break;
            case 3: local_channel = &servo_tim32_1_ch3; break;
            default: return HAL_ERROR;
        }
    }
    else if (timer == TIMER32_2)
    {
        local_timer32 = &servo_timer32_2;
        switch (channel)
        {
            case 0: local_channel = &servo_tim32_2_ch0; break;
            case 1: local_channel = &servo_tim32_2_ch1; break;
            case 2: local_channel = &servo_tim32_2_ch2; break;
            case 3: local_channel = &servo_tim32_2_ch3; break;
            default: return HAL_ERROR;
        }
    }
    else return HAL_ERROR;
    /* Инициализация таймера */
    local_timer32->Instance = timer;
    local_timer32->Top = SERVO_TIMER_TOP_VALUE;
    local_timer32->State = TIMER32_STATE_DISABLE;
    local_timer32->Clock.Source = TIMER32_SOURCE_PRESCALER;
    local_timer32->Clock.Prescaler = SERVO_TIMER_PRESCALER-1;
    local_timer32->InterruptMask = 0;
    local_timer32->CountMode = TIMER32_COUNTMODE_FORWARD;
    HAL_Timer32_Init(local_timer32);
    /* Установка параметров канала */
    local_channel->TimerInstance = local_timer32->Instance;
    switch (channel)
    {
        case 0: local_channel->ChannelIndex = TIMER32_CHANNEL_0; break;
        case 1: local_channel->ChannelIndex = TIMER32_CHANNEL_1; break;
        case 2: local_channel->ChannelIndex = TIMER32_CHANNEL_2; break;
        case 3: local_channel->ChannelIndex = TIMER32_CHANNEL_3; break;
        default: return HAL_ERROR;
    }
    local_channel->PWM_Invert = TIMER32_CHANNEL_NON_INVERTED_PWM;
    local_channel->Mode = TIMER32_CHANNEL_MODE_PWM;
    local_channel->CaptureEdge = TIMER32_CHANNEL_CAPTUREEDGE_RISING;
    local_channel->OCR = SERVO_90DEG_VALUE;
    local_channel->Noise = TIMER32_CHANNEL_FILTER_OFF;
    HAL_Timer32_Channel_Init(local_channel);
    return HAL_OK;
}


static HAL_StatusTypeDef timer16_ini(TIMER16_TypeDef* timer)
{
    /* Подготовка указателей на структуры таймера и канала */
    Timer16_HandleTypeDef* local_timer16;
    if (timer == TIMER16_0)      local_timer16 = &servo_timer16_1;
    else if (timer == TIMER16_1) local_timer16 = &servo_timer16_2;
    else if (timer == TIMER16_2) local_timer16 = &servo_timer16_2;
    else return HAL_ERROR;
    /*  */
    local_timer16->Instance = timer;
    local_timer16->Clock.Source = TIMER16_SOURCE_INTERNAL_SYSTEM;
    local_timer16->CountMode = TIMER16_COUNTMODE_INTERNAL;  /* При тактировании от Input1 не имеет значения */
    local_timer16->Clock.Prescaler = TIMER16_PRESCALER_16;
    local_timer16->ActiveEdge = TIMER16_ACTIVEEDGE_RISING;  /* Выбирается при тактировании от Input1 */
    local_timer16->Preload = TIMER16_PRELOAD_AFTERWRITE;
    local_timer16->Trigger.Source = TIMER16_TRIGGER_TIM1_GPIO1_9; 
    local_timer16->Trigger.ActiveEdge = TIMER16_TRIGGER_ACTIVEEDGE_SOFTWARE;    /* При использовании триггера значение должно быть отлично от software */
    local_timer16->Trigger.TimeOut = TIMER16_TIMEOUT_DISABLE;   /* Разрешить повторное срабатывание триггера */
    local_timer16->Filter.ExternalClock = TIMER16_FILTER_NONE;
    local_timer16->Filter.Trigger = TIMER16_FILTER_NONE;
    local_timer16->EncoderMode = TIMER16_ENCODER_DISABLE;
    local_timer16->Waveform.Enable = TIMER16_WAVEFORM_GENERATION_ENABLE;
    local_timer16->Waveform.Polarity = TIMER16_WAVEFORM_POLARITY_NONINVERTED;
    HAL_Timer16_Init(local_timer16);
    /*  */
    HAL_Timer16_StartPWM(local_timer16, SERVO_TIMER_TOP_VALUE, SERVO_90DEG_VALUE);
    return HAL_OK;
}




void Servo32_PrintAngle(HAL_ServoPinTypeDef pin, uint8_t angle)
{
    if (angle > 180) angle = 180;
    uint16_t ocr = SERVO_0DEG_VALUE+(SERVO_ANGLE_TO_VALUE_COEF*angle);
    switch (pin)
    {
        case ServoGPIO0_7:  HAL_Timer16_SetCMP(&servo_timer16_0, ocr); break;
        case ServoGPIO0_10: HAL_Timer16_SetCMP(&servo_timer16_1, ocr); break;
        case ServoGPIO0_13: HAL_Timer16_SetCMP(&servo_timer16_2, ocr); break;
        case ServoGPIO0_0:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_1_ch0, ocr); break;
        case ServoGPIO0_1:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_1_ch1, ocr); break;
        case ServoGPIO0_2:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_1_ch2, ocr); break;
        case ServoGPIO0_3:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_1_ch3, ocr); break;
        case ServoGPIO1_0:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_2_ch0, ocr); break;
        case ServoGPIO1_1:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_2_ch1, ocr); break;
        case ServoGPIO1_2:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_2_ch2, ocr); break;
        case ServoGPIO1_3:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_2_ch3, ocr); break;
    }
}


#endif
