#ifndef __MOTOR_H
#define __MOTOR_H

#include "main.h"

extern CAN_HandleTypeDef hcan1;

#define MOTOR_COUNT 4
#define ENCODER_RES 8192.0f

typedef struct {
    uint16_t ecd;
    int16_t  speed_rpm;
    int16_t  given_current;
    uint8_t  temperate;
    uint8_t  initialized;
    uint16_t last_ecd;
    int32_t  total_ecd;
    float    total_angle;
} motor_measure_t;

typedef struct {
    float Kp, Ki, Kd;
    float setpoint, feedback, error;
    float integral, prev_error, output;
    float out_max, int_max;
} PID_t;

extern motor_measure_t motor[MOTOR_COUNT];
extern PID_t pid_angle;
extern PID_t pid_speed;

void  Motor_Init(void);
void  Motor_SendCurrent(int16_t c1, int16_t c2, int16_t c3, int16_t c4);
void  Motor_Update(CAN_RxHeaderTypeDef *hdr, uint8_t *data);
void  Motor_ControlLoop(void);
void  PID_Init(PID_t *pid, float kp, float ki, float kd, float out_max, float int_max);
float PID_Calc(PID_t *pid, float setpoint, float feedback);

#endif