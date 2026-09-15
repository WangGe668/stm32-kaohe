#ifndef __COMM_H
#define __COMM_H

#include "main.h"

extern UART_HandleTypeDef huart1;  

void Comm_Init(void);
void JustFloat_Send(float *data, uint8_t ch_count);

#endif