#ifndef __LED_H
#define __LED_H

#include "main.h"

extern TIM_HandleTypeDef htim5;

#define LED_R_CHANNEL   TIM_CHANNEL_1   // PH12 - 红
#define LED_G_CHANNEL   TIM_CHANNEL_2   // PH11 - 绿
#define LED_B_CHANNEL   TIM_CHANNEL_3   // PH10 - 蓝

void Led_Init(void);
void Led_SetColor(uint16_t r, uint16_t g, uint16_t b);
void Led_AllOff(void);

#endif