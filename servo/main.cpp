#ifndef SERVO32_LIB
#define SERVO32_LIB

#include "mik32_hal_timer32.h"
#include "mik32_hal_timer16.h"


#define SERVO_TIMER_TOP_VALUE       40000UL

void Servo_ini(HAL_ServoPinTypeDef pin);
static HAL_StatusTypeDef timer32_ini(TIMER32_TypeDef* timer, uint8_t channel);
static HAL_StatusTypeDef timer16_ini(TIMER16_TypeDef* timer);

void Servo32_PrintAngle(HAL_ServoPinTypeDef pin);


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




typedef enum __HAL_ServoPinTypeDef
{
    GPIO0_7,    //tim16_0
    GPIO0_10,   //tim16_1
    GPIO0_13,   //tim16_2
    GPIO0_0,    //tim32_1, ch1
    GPIO0_1,    //tim32_1, ch2
    GPIO0_2,    //tim32_1, ch3
    GPIO0_3,    //tim32_1, ch4
    GPIO1_0,    //tim32_2, ch1
    GPIO1_1,    //tim32_2, ch2
    GPIO1_2,    //tim32_2, ch3
    GPIO1_3     //tim32_2, ch4
} HAL_ServoPinTypeDef;


//void anm(GPIO_TypeDef *GPIO_x, HAL_PinsTypeDef pin)

void Servo_ini(HAL_ServoPinTypeDef pin)
{
    switch (pin)
    {
        case GPIO0_7:  timer16_ini(TIMER16_0); break;
        case GPIO0_10: timer16_ini(TIMER16_1); break;
        case GPIO0_13: timer16_ini(TIMER16_2); break;
        case GPIO0_0:  timer32_ini(TIMER32_1,0); break;
        case GPIO0_1:  timer32_ini(TIMER32_1,1); break;
        case GPIO0_2:  timer32_ini(TIMER32_1,2); break;
        case GPIO0_3:  timer32_ini(TIMER32_1,3); break;
        case GPIO1_0:  timer32_ini(TIMER32_2,0); break;
        case GPIO1_1:  timer32_ini(TIMER32_2,1); break;
        case GPIO1_2:  timer32_ini(TIMER32_2,2); break;
        case GPIO1_3:  timer32_ini(TIMER32_2,3); break;
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
    local_timer32->Clock.Prescaler = 16-1;
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
    local_channel->OCR = 7544 >> 1;
    local_channel->Noise = TIMER32_CHANNEL_FILTER_OFF;
    HAL_Timer32_Channel_Init(local_channel);
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
    HAL_Timer16_StartPWM(local_timer16, SERVO_TIMER_TOP_VALUE, 2000);
}




void Servo32_PrintAngle(HAL_ServoPinTypeDef pin)
{
    switch (pin)
    {
        case GPIO0_7:  HAL_Timer16_SetCMP(&servo_timer16_0, 2000+1000); break;
        case GPIO0_10: HAL_Timer16_SetCMP(&servo_timer16_1, 2000+1000); break;
        case GPIO0_13: HAL_Timer16_SetCMP(&servo_timer16_2, 2000+1000); break;
        case GPIO0_0:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_1_ch0, 2000+1000); break;
        case GPIO0_1:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_1_ch1, 2000+1000); break;
        case GPIO0_2:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_1_ch2, 2000+1000); break;
        case GPIO0_3:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_1_ch3, 2000+1000); break;
        case GPIO1_0:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_2_ch0, 2000+1000); break;
        case GPIO1_1:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_2_ch1, 2000+1000); break;
        case GPIO1_2:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_2_ch2, 2000+1000); break;
        case GPIO1_3:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_2_ch3, 2000+1000); break;
    }
}


#endif
