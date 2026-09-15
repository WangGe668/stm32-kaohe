#include "comm.h"

#define RX_BUFFER_SIZE 64

static uint8_t  rx_byte;
static uint8_t  rx_buffer[RX_BUFFER_SIZE];
static uint16_t rx_index = 0;

static float Parse_KP(const char* str)
{
    while (*str != '=' && *str != '\0') str++;
    if (*str == '=') str++;

    int negative = 0;
    if (*str == '-') {
        negative = 1;
        str++;
    }

    float result = 0.0f;
    float decimal_factor = 0.1f;
    int has_decimal = 0;

    while ((*str >= '0' && *str <= '9') || *str == '.') {
        if (*str == '.') {
            has_decimal = 1;
        } else {
            if (has_decimal) {
                result += (*str - '0') * decimal_factor;
                decimal_factor *= 0.1f;
            } else {
                result = result * 10.0f + (*str - '0');
            }
        }
        str++;
    }
    return negative ? -result : result;
}

//初始化
void Comm_Init(void)
{
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1); 
}

//JustFloat 协议发送
void JustFloat_Send(float *data, uint8_t ch_count)
{
    uint8_t tail[4] = {0x00, 0x00, 0x80, 0x7F};
    HAL_UART_Transmit(&huart1, (uint8_t*)data, ch_count * sizeof(float), 100);  
    HAL_UART_Transmit(&huart1, tail, 4, 100);                                     
}

//接收完成回调
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) { 
        if (rx_byte == '\r') {
            rx_buffer[rx_index] = '\0';
            float kp_value = Parse_KP((char*)rx_buffer);
            JustFloat_Send(&kp_value, 1);
            rx_index = 0;
        } else {
            if (rx_index < RX_BUFFER_SIZE - 1) {
                rx_buffer[rx_index++] = rx_byte;
            }
        }
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);  
    }
}

//错误回调：出错后重新启动接收，防止卡死
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) { 
        __HAL_UART_CLEAR_OREFLAG(huart);
        __HAL_UART_CLEAR_FEFLAG(huart);
        __HAL_UART_CLEAR_NEFLAG(huart);
        __HAL_UART_CLEAR_PEFLAG(huart);
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1); 
    }
}