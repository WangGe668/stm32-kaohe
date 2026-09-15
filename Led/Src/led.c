#include "led.h"

//初始化

void Led_Init(void)
{
    HAL_TIM_PWM_Start(&htim5, LED_R_CHANNEL);
    HAL_TIM_PWM_Start(&htim5, LED_G_CHANNEL);
    HAL_TIM_PWM_Start(&htim5, LED_B_CHANNEL);
}

//设置 RGB 亮度

void Led_SetColor(uint16_t r, uint16_t g, uint16_t b)
{
    __HAL_TIM_SET_COMPARE(&htim5, LED_R_CHANNEL, r);
    __HAL_TIM_SET_COMPARE(&htim5, LED_G_CHANNEL, g);
    __HAL_TIM_SET_COMPARE(&htim5, LED_B_CHANNEL, b);
}

void Led_AllOff(void)
{
    Led_SetColor(0, 0, 0);
}