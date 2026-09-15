#ifndef __BUZZER_H
#define __BUZZER_H

#include <stdint.h>

//报错类型 
typedef enum {
    BUZZER_ERR_NONE = 0,
    BUZZER_ERR_1    = 1, 
    BUZZER_ERR_2    = 2,  
} Buzzer_ErrorType_t;

//报错标志：其他模块检测到故障时置 1
extern volatile uint8_t Buzzer_ErrFlag1;
extern volatile uint8_t Buzzer_ErrFlag2;

void Buzzer_Init(void);
void Buzzer_PlayBootBeep(void);
void Buzzer_PlayNote(uint16_t freq, uint16_t duration_ms);
void Buzzer_PlayErrorTone(Buzzer_ErrorType_t errorType);

#endif