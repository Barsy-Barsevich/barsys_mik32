#include "Servo32.h"

void Servo_ini(HAL_ServoPinTypeDef pin)
{
    switch (pin)
    {
        case ServoPin_0_7:  timer16_ini(TIMER16_0); break;
        case ServoPin_0_10: timer16_ini(TIMER16_1); break;
        case ServoPin_0_13: timer16_ini(TIMER16_2); break;
        case ServoPin_0_0:  timer32_ini(TIMER32_1,0); break;
        case ServoPin_0_1:  timer32_ini(TIMER32_1,1); break;
        case ServoPin_0_2:  timer32_ini(TIMER32_1,2); break;
        case ServoPin_0_3:  timer32_ini(TIMER32_1,3); break;
        case ServoPin_1_0:  timer32_ini(TIMER32_2,0); break;
        case ServoPin_1_1:  timer32_ini(TIMER32_2,1); break;
        case ServoPin_1_2:  timer32_ini(TIMER32_2,2); break;
        case ServoPin_1_3:  timer32_ini(TIMER32_2,3); break;
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
    // Установка параметров канала 
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
    /* Start timer */
    HAL_Timer32_Channel_Enable(local_channel);
    HAL_Timer32_Value_Clear(local_timer32);
    HAL_Timer32_Start(local_timer32);
    return HAL_OK;
}


static HAL_StatusTypeDef timer16_ini(TIMER16_TypeDef* timer)
{
    /* Подготовка указателей на структуры таймера и канала */
    Timer16_HandleTypeDef* local_timer16;
    if (timer == TIMER16_0)      local_timer16 = &servo_timer16_0;
    else if (timer == TIMER16_1) local_timer16 = &servo_timer16_1;
    else if (timer == TIMER16_2) local_timer16 = &servo_timer16_2;
    else return HAL_ERROR;
    /*  */
    local_timer16->Instance = timer;
    local_timer16->Clock.Source = TIMER16_SOURCE_INTERNAL_SYSTEM;
    local_timer16->CountMode = TIMER16_COUNTMODE_INTERNAL;  /* При тактировании от Input1 не имеет значения */
    local_timer16->Clock.Prescaler = SERVO_TIMER16_PRESCALER;
    local_timer16->ActiveEdge = TIMER16_ACTIVEEDGE_RISING;  /* Выбирается при тактировании от Input1 */
    local_timer16->Preload = TIMER16_PRELOAD_AFTERWRITE;
    local_timer16->Trigger.Source = TIMER16_TRIGGER_TIM1_GPIO1_9; 
    local_timer16->Trigger.ActiveEdge = TIMER16_TRIGGER_ACTIVEEDGE_SOFTWARE;    /* При использовании триггера значение должно быть отлично от software */
    local_timer16->Trigger.TimeOut = TIMER16_TIMEOUT_DISABLE;   /* Разрешить повторное срабатывание триггера */
    local_timer16->Filter.ExternalClock = TIMER16_FILTER_NONE;
    local_timer16->Filter.Trigger = TIMER16_FILTER_NONE;
    local_timer16->EncoderMode = TIMER16_ENCODER_DISABLE;
    local_timer16->Waveform.Enable = TIMER16_WAVEFORM_GENERATION_ENABLE;
    local_timer16->Waveform.Polarity = TIMER16_WAVEFORM_POLARITY_INVERTED;
    HAL_Timer16_Init(local_timer16);
    /*  */
    HAL_Timer16_StartPWM(local_timer16, SERVO_TIMER_TOP_VALUE, SERVO_180DEG_VALUE);
    return HAL_OK;
}

void Servo32_PrintAngle(HAL_ServoPinTypeDef pin, uint8_t angle)
{
    if (angle > 180) angle = 180;
    uint16_t ocr = SERVO_0DEG_VALUE+(SERVO_ANGLE_TO_VALUE_COEF*angle);
    switch (pin)
    {
        case ServoPin_0_7:  HAL_Timer16_SetCMP(&servo_timer16_0, ocr); break;
        case ServoPin_0_10: HAL_Timer16_SetCMP(&servo_timer16_1, ocr); break;
        case ServoPin_0_13: HAL_Timer16_SetCMP(&servo_timer16_2, ocr); break;
        case ServoPin_0_0:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_1_ch0, ocr); break;
        case ServoPin_0_1:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_1_ch1, ocr); break;
        case ServoPin_0_2:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_1_ch2, ocr); break;
        case ServoPin_0_3:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_1_ch3, ocr); break;
        case ServoPin_1_0:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_2_ch0, ocr); break;
        case ServoPin_1_1:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_2_ch1, ocr); break;
        case ServoPin_1_2:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_2_ch2, ocr); break;
        case ServoPin_1_3:  HAL_Timer32_Channel_OCR_Set(&servo_tim32_2_ch3, ocr); break;
    }
}

HAL_StatusTypeDef Servo_Clock_ini()
{
    uint32_t _0deg_value, _90deg_value, _180deg_value;
    uint32_t timer_prescaler, timer_freq;
    HAL_Timer16_PrescalerTypeDef timer16_prescaler;
    uint32_t apb_p_freq = HAL_PCC_GetSysClockFreq() / (PM->DIV_AHB * PM->DIV_APB_P);
#ifdef SERVO_50HZ
    if (apb_p_freq <= 2000000UL)
    {
        timer_prescaler = 1;
        timer16_prescaler = TIMER16_PRESCALER_1;
    }
    else if (apb_p_freq <= 4000000UL)
    {
        timer_prescaler = 2;
        timer16_prescaler = TIMER16_PRESCALER_2;
    }
    else if (apb_p_freq <= 8000000UL)
    {
        timer_prescaler = 4;
        timer16_prescaler = TIMER16_PRESCALER_4;
    }
    else if (apb_p_freq <= 16000000UL)
    {
        timer_prescaler = 8;
        timer16_prescaler = TIMER16_PRESCALER_8;
    }
    else
    {
        timer_prescaler = 16;
        timer16_prescaler = TIMER16_PRESCALER_16;
    }
    timer_freq = apb_p_freq /  timer_prescaler;
    /* Servo tact = 50Hz, space between 0.5ms & 2.5ms shold have >=256 steps */
    if (timer_freq < (50*40/4*256)) return HAL_ERROR;
    _0deg_value = timer_freq/50/40;  //< 0.5ms
    // _90deg_value = 3 * _0deg_value; //< 1.5ms
    _180deg_value = 5 * _0deg_value; //< 2.5ms
#else
#ifdef SERVO_330HZ
    if (apb_p_freq <= 16000000UL)
    {
        timer_prescaler = 1;
        timer16_prescaler = TIMER16_PRESCALER_1;
    }
    else
    {
        timer_prescaler = 2;
        timer16_prescaler = TIMER16_PRESCALER_2;
    }
    timer_freq = apb_p_freq /  timer_prescaler;
    //Servo tact = 333Hz=(1000/3)Hz, space between 0.5ms & 2.5ms shold have >=256 steps
    if (timer_freq < (1000*6/3/4*256)) return HAL_ERROR;
    _0deg_value = timer_freq/(1000*6/3); //0.5ms
    // _90deg_value = 3 * _0deg_value; //1.5ms
    _180deg_value = 5 * _0deg_value; //2.5ms
#endif
#endif
    return HAL_OK;
}
