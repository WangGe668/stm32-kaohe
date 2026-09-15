#include "buzzer.h"
#include "main.h"

extern TIM_HandleTypeDef htim4;

/* ============================================================
 * 全局变量
 * ============================================================ */
volatile uint8_t Buzzer_ErrFlag1 = 0;
volatile uint8_t Buzzer_ErrFlag2 = 0;

/* ============================================================
 * 音符频率定义 (Hz)
 * ============================================================ */
#define NOTE_SIL   0
#define NOTE_C5    523
#define NOTE_D5    587
#define NOTE_E5    659
#define NOTE_F5    698
#define NOTE_G5    784
#define NOTE_A5    880
#define NOTE_B5    988
#define NOTE_C6    1047
#define NOTE_D6    1175
#define NOTE_E6    1319
#define NOTE_F6    1397
#define NOTE_G6    1568
#define NOTE_A6    1760

static uint8_t buzzer_boost = 3;

void Buzzer_SetFreq(uint32_t freq)
{
    if (freq == 0) {
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 0);
        return;
    }

    freq *= buzzer_boost;
    if (freq > 8000) freq = 8000;
    if (freq < 200)  freq = 200;

    uint32_t arr = (1000000 / freq) - 1;
    __HAL_TIM_SET_AUTORELOAD(&htim4, arr);
    __HAL_TIM_SET_COUNTER(&htim4, 0);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, arr / 2);   /* 50% 占空比 */
}

//初始化：启动 PWM，保持静音

void Buzzer_Init(void)
{
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 0);
}

//播放单个音符

void Buzzer_PlayNote(uint16_t freq, uint16_t duration_ms)
{
    if (freq == 0) {
        Buzzer_SetFreq(0);
        HAL_Delay(duration_ms);
        return;
    }
    Buzzer_SetFreq(freq);
    HAL_Delay(duration_ms);
    Buzzer_SetFreq(0);
}

//上电提示音

void Buzzer_PlayBootBeep(void)
{
    uint32_t notes[] = { NOTE_C6, NOTE_E6, NOTE_G6, NOTE_C6 * 2 };
    for (int i = 0; i < 4; i++) {
        Buzzer_SetFreq(notes[i]);
        HAL_Delay(90);
        Buzzer_SetFreq(0);
        HAL_Delay(40);
    }
}

//报错音调 1

static void Play_Error1(void)
{
    uint32_t m[] = { NOTE_E5, NOTE_E5, NOTE_F5, NOTE_G5,
                     NOTE_G5, NOTE_F5, NOTE_E5, NOTE_D5 };
    uint16_t d[] = { 300, 300, 300, 300,
                     300, 300, 300, 600 };

    for (int i = 0; i < 8; i++) {
        Buzzer_SetFreq(m[i]);
        HAL_Delay(d[i]);
        Buzzer_SetFreq(0);
        HAL_Delay(30);
    }
}

//报错音调 2

static void Play_Error2(void)
{
    uint32_t m[] = { NOTE_E6, NOTE_E6, NOTE_SIL, NOTE_E6, NOTE_SIL, NOTE_C6 };
    uint16_t d[] = { 100, 100, 100, 100, 100, 200 };

    for (int i = 0; i < 6; i++) {
        Buzzer_SetFreq(m[i]);
        HAL_Delay(d[i]);
        Buzzer_SetFreq(0);
    }
}

//播放指定报错音调

void Buzzer_PlayErrorTone(Buzzer_ErrorType_t errorType)
{
    if (errorType == BUZZER_ERR_1) {
        Play_Error1();
    } else if (errorType == BUZZER_ERR_2) {
        Play_Error2();
    }
}