#include "servo.h"

//初始化，启动 PWM

void Servo_Init(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);  
}

//设置角度

void Servo_SetAngle(uint8_t angle)
{
    if (angle > 180) angle = 180;
    
    // 脉宽范围 500us(0°) ~ 2500us(180°)
    // 公式: CCR = 500 + (angle / 180.0) * 2000
    uint16_t ccr = 500 + (uint16_t)((angle / 180.0f) * 2000.0f);
    
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, ccr);
}