/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os.h"
#include "led.h"
#include "servo.h"
#include "comm.h"
#include "motor.h"
#include "buzzer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* ============================================================
 *  任务一：蜂鸣器
 *  - 上电音：在 main.c 的 USER CODE BEGIN 2 里完成
 *  - 按键 PA0：轮换播放两种报错音
 * ============================================================ */
void StartBuzzerTask(void *argument)
{
  Buzzer_Init();

  uint8_t last_key = 1;
  uint8_t play_index = 0;

  for(;;)
  {
      uint8_t key = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);

      if (last_key == 1 && key == 0)
      {
          osDelay(20);
          if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == 0)
          {
              if (play_index == 0) {
                  Buzzer_PlayErrorTone(BUZZER_ERR_1); 
                  play_index = 1;
              } else {
                  Buzzer_PlayErrorTone(BUZZER_ERR_2); 
                  play_index = 0;
              }
              while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == 0) {
                  osDelay(10);
              }
          }
      }
      last_key = key;
      osDelay(20);
  }
}

/* ============================================================
 *  任务二：LED 流水灯
 * ============================================================ */
void StartLedTask(void *argument)
{
  Led_Init();
  for(;;)
  {
      Led_SetColor(1000, 0, 0);  osDelay(200);
      Led_SetColor(0, 1000, 0);  osDelay(200);
      Led_SetColor(0, 0, 1000);  osDelay(200);
      Led_AllOff();              osDelay(100);
  }
}

/* ============================================================
 *  任务三：舵机扫动
 * ============================================================ */
void ServoSweepTask(void *argument)
{
  Servo_Init();
  for(;;)
  {
      for(uint8_t angle = 0; angle <= 180; angle += 45)
      {
          Servo_SetAngle(angle);
          osDelay(1000);
      }
      osDelay(1000);
      Servo_SetAngle(0);
      osDelay(1000);
  }
}

/* ============================================================
 *  任务四：串口收发
 * ============================================================ */
void StartCommTask(void *argument)
{
  Comm_Init();
  for(;;)
  {
      osDelay(100);
  }
}

/* ============================================================
 *  任务五：3508 电机角度闭环
 * ============================================================ */
void StartMotorCtrlTask(void *argument)
{
  Motor_Init();
  HAL_CAN_Start(&hcan1);
  HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
  osDelay(500);

  uint32_t tick = 0;
  for(;;)
  {
      if (tick < 2000) {
          pid_angle.setpoint = 90.0f * tick / 2000.0f;
      } else if (tick < 4000) {
          pid_angle.setpoint = 90.0f - 180.0f * (tick - 2000) / 2000.0f;
      } else {
          pid_angle.setpoint = -90.0f;
      }
      Motor_ControlLoop();
      tick++;
      osDelay(1);
  }
}

/* USER CODE END Application */

