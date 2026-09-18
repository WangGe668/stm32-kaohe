#include "motor.h"
#include "comm.h"
#include <math.h>

motor_measure_t motor[MOTOR_COUNT];
PID_t pid_angle;
PID_t pid_speed;

//PID 初始化

void PID_Init(PID_t *pid, float kp, float ki, float kd, float out_max, float int_max)
{
    pid->Kp = kp; pid->Ki = ki; pid->Kd = kd;
    pid->setpoint = pid->feedback = pid->error = 0;
    pid->integral = pid->prev_error = pid->output = 0;
    pid->out_max = out_max;
    pid->int_max = int_max;
}

//PID 计算

float PID_Calc(PID_t *pid, float setpoint, float feedback)
{
    pid->setpoint = setpoint;
    pid->feedback = feedback;
    pid->error = setpoint - feedback;

    if (fabsf(pid->error) < 0.3f) {
        pid->integral = 0;
    } else {
        pid->integral += pid->error;
        if (pid->integral >  pid->int_max) pid->integral =  pid->int_max;
        if (pid->integral < -pid->int_max) pid->integral = -pid->int_max;
    }

    float derivative = pid->error - pid->prev_error;
    pid->prev_error = pid->error;

    pid->output = pid->Kp * pid->error
                + pid->Ki * pid->integral
                + pid->Kd * derivative;

    if (pid->output >  pid->out_max) pid->output =  pid->out_max;
    if (pid->output < -pid->out_max) pid->output = -pid->out_max;

    return pid->output;
}

//电机初始化

void Motor_Init(void)
{
    PID_Init(&pid_angle, 8.0f, 0.3f, 3.0f, 5000.0f, 1000.0f);
    PID_Init(&pid_speed, 10.0f, 0.1f, 0.0f, 16000.0f, 1000.0f);

    for (int i = 0; i < MOTOR_COUNT; i++) {
        motor[i].initialized = 0;
        motor[i].total_ecd = 0;
        motor[i].total_angle = 0;
    }
}

//通过 CAN 发送电流指令

void Motor_SendCurrent(int16_t c1, int16_t c2, int16_t c3, int16_t c4)
{
    CAN_TxHeaderTypeDef tx;
    uint8_t data[8];
    uint32_t mailbox;

    tx.StdId = 0x200;
    tx.IDE   = CAN_ID_STD;
    tx.RTR   = CAN_RTR_DATA;
    tx.DLC   = 8;

    data[0] = (c1 >> 8) & 0xFF;  data[1] = c1 & 0xFF;
    data[2] = (c2 >> 8) & 0xFF;  data[3] = c2 & 0xFF;
    data[4] = (c3 >> 8) & 0xFF;  data[5] = c3 & 0xFF;
    data[6] = (c4 >> 8) & 0xFF;  data[7] = c4 & 0xFF;

    HAL_CAN_AddTxMessage(&hcan1, &tx, data, &mailbox);
}

//处理 CAN 接收到的电机反馈

void Motor_Update(CAN_RxHeaderTypeDef *hdr, uint8_t *data)
{
    uint8_t id = hdr->StdId - 0x201;
    if (id >= MOTOR_COUNT) return;

    uint16_t new_ecd = (data[0] << 8) | data[1];

    if (!motor[id].initialized) {
        motor[id].last_ecd = new_ecd;
        motor[id].total_ecd = 0;
        motor[id].total_angle = 0;
        motor[id].initialized = 1;
    } else {
        int16_t delta = (int16_t)(new_ecd - motor[id].last_ecd);
        if (delta >  4096) delta -= 8192;
        if (delta < -4096) delta += 8192;
        motor[id].total_ecd  += delta;
        motor[id].last_ecd    = new_ecd;
        motor[id].total_angle = motor[id].total_ecd / (ENCODER_RES * 19.203f) * 360.0f;
    }

    motor[id].ecd           = new_ecd;
    motor[id].speed_rpm     = (data[2] << 8) | data[3];
    motor[id].given_current = (data[4] << 8) | data[5];
    motor[id].temperate     = data[6];
}

//控制循环：串级 PID + Synex 打印

void Motor_ControlLoop(void)
{
    float error = pid_angle.setpoint - motor[0].total_angle;

    //死区 ±1°
    if (fabsf(error) < 1.0f) {
        Motor_SendCurrent(0, 0, 0, 0);
        pid_angle.integral = 0;
        pid_angle.prev_error = error;
    } else {
        float target_rpm = PID_Calc(&pid_angle, pid_angle.setpoint, motor[0].total_angle);
        float target_cur = PID_Calc(&pid_speed, target_rpm, (float)motor[0].speed_rpm);
        Motor_SendCurrent((int16_t)target_cur, 0, 0, 0);
    }

    static uint8_t print_cnt = 0;
    if (++print_cnt >= 100) {
        print_cnt = 0;
        float tx[3];
        tx[0] = (float)motor[0].given_current;
        tx[1] = (float)motor[0].ecd / ENCODER_RES * 360.0f;
        tx[2] = (float)motor[0].speed_rpm;
        JustFloat_Send(tx, 3);
    }
}