#ifndef __USART_1_H
#define __USART_1_H

#include "at32f403a_407.h"  
#include "at32f403a_407_usart.h"

#define N								 3
#define USART3_TX_BUFFER_SIZE            (4 * N + 4)

#define RX_BUFFER_SIZE  64  // DMA接收缓冲区大小

#define PRINT_UART                       USART3
#define PRINT_UART_CRM_CLK               CRM_USART3_PERIPH_CLOCK
#define PRINT_UART_TX_PIN                GPIO_PINS_10
#define PRINT_UART_TX_GPIO               GPIOB
#define PRINT_UART_TX_GPIO_CRM_CLK       CRM_GPIOB_PERIPH_CLOCK

// 定义结构体来存储 PID 参数
typedef struct {
    float Current_KP;
    float Current_KI;
    float Current_KD;
	float Iq;  // 新增的 LQ 参数
    float Id;  // 新增的 Ld 参数
	uint8_t Current_set_flag;
	
	float Speed_KP;
    float Speed_KI;
    float Speed_KD;
	float Speed;
	uint8_t Speed_set_flag;
} PID_Params;

extern PID_Params pid_params_1;
extern PID_Params pid_params_2;

void USART3_SendFloatArray(float *data, uint8_t size);
void USART3_SendSinWaveData(float *data);
void ProcessCommand(char *cmd);
void usartdmarecv(u8 *data,u16 len);
void parse_and_set_pid(const char *input, PID_Params *pid_params_1, PID_Params *pid_params_2);

extern uint8_t uart3_tx_buffer[USART3_TX_BUFFER_SIZE] ;
extern volatile uint8_t usart3_tx_dma_status;

extern uint8_t uart3_rx_buffer[RX_BUFFER_SIZE] ;
extern volatile uint8_t usart3_rx_dma_status;
extern volatile uint8_t rx1_flag;

#endif
