#ifndef __SERVO_H
#define __SERVO_H

#include "main.h"

extern TIM_HandleTypeDef htim1;  

void Servo_Init(void);
void Servo_SetAngle(uint8_t angle);

#endif