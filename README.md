# RoboMaster 开发板 C 型 工程

## 任务列表

| 任务 | 功能 | 状态 |
|------|------|------|
| 任务一 | 蜂鸣器上电音 + 两种报错音调 | 完成 |
| 任务二 | LED 流水灯 PWM + FreeRTOS | 完成 |
| 任务三 | 舵机 PWM 控制 | 完成 |
| 任务四 | 串口 JustFloat 协议收发 | 完成 |
| 任务五 | 3508 电机 PID 角度闭环 | 完成 |

## 硬件平台

- 大疆 RoboMaster 开发板 C 型（STM32F407IGH6）
- ST-Link V2、USB 转 TTL（CH340）
- M3508 电机 + C620 电调

## 软件架构

- **Core/**：HAL 驱动 + FreeRTOS 框架
- **模块层**：Buzzer / Led / Servo / Comm / Motor
- **应用层**：freertos.c 中每个功能一个任务

## 代码规范

- 函数名：大驼峰（`Led_SetColor`）
- 变量名：小驼峰（`rx_index`）
- 宏定义：全大写（`ENCODER_RES`）