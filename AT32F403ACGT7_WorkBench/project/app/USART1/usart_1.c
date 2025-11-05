#include "usart_1.h"                  // Device header 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint8_t uart3_tx_buffer[USART3_TX_BUFFER_SIZE] = {0};
volatile uint8_t usart3_tx_dma_status = 0;

uint8_t uart3_rx_buffer[RX_BUFFER_SIZE] = {0};
volatile uint8_t usart3_rx_dma_status = 0;

volatile uint8_t rx1_flag = 0;

PID_Params pid_params_1 = {
    .Current_KP = 0.04f,
    .Current_KI = 0.04f,
    .Current_KD = 0.0f,
    .Iq = 0.0f, 
    .Id = 0.0f,  
    .Current_set_flag = 1,

    .Speed_KP = 0.3f,
    .Speed_KI = 0.01f,
    .Speed_KD = 0.0f,
    .Speed = 0.0f,
    .Speed_set_flag = 1
};

PID_Params pid_params_2 = {
    .Current_KP = 0.04f,
    .Current_KI = 0.04f,
    .Current_KD = 0.0f,
    .Iq = 0.0f,  
    .Id = 0.0f,  
    .Current_set_flag = 1,

    .Speed_KP = 0.3f,
    .Speed_KI = 0.01f,
    .Speed_KD = 0.0f,
    .Speed = 0.0f,
    .Speed_set_flag = 1
};

void parse_and_set_pid(const char *input, PID_Params *pid_params_1, PID_Params *pid_params_2);

int fputc(int ch, FILE *f)
{
  while(usart_flag_get(PRINT_UART, USART_TDBE_FLAG) == RESET);
  usart_data_transmit(PRINT_UART, (uint16_t)ch);
  while(usart_flag_get(PRINT_UART, USART_TDC_FLAG) == RESET);
  return ch;
}


void USART3_SendFloatArray(float *data, uint8_t size) {

	
    uint8_t *bytePtr = (uint8_t *)data; 
    
  
    for (uint8_t i = 0; i < size * 4; i++) {
        uart3_tx_buffer[i] = bytePtr[i];
    }


    uart3_tx_buffer[size * 4] = 0x00;
    uart3_tx_buffer[size * 4 + 1] = 0x00;
    uart3_tx_buffer[size * 4 + 2] = 0x80;
    uart3_tx_buffer[size * 4 + 3] = 0x7F;
	
	while(usart3_tx_dma_status == 0){};

    dma_channel_enable(DMA1_CHANNEL2, TRUE);
}


 
void usartdmarecv(u8 *data,u16 len)
{
 
    dma_flag_clear(DMA1_FDT1_FLAG);           
	usart_dma_receiver_enable(USART1,FALSE);       
    dma_channel_enable(DMA1_CHANNEL1, FALSE);       
    DMA1_CHANNEL1->dtcnt=RX_BUFFER_SIZE;                
    DMA1_CHANNEL1->maddr=(uint32_t)data;
    usart_dma_receiver_enable(USART1,TRUE);        
    dma_channel_enable(DMA1_CHANNEL1, TRUE);       
}


void parse_and_set_pid(const char *input, PID_Params *pid_params_1, PID_Params *pid_params_2) {
   
    const char *colon_pos = strchr(input, ':');
    if (colon_pos == NULL) {
        printf("无效的输入格式\n");
        return;
    }

    //计算名称长度
    size_t name_len = colon_pos - input;
    if (name_len >= 20) { // 限制最大长度，防止溢出
        printf("参数名过长\n");
        return;
    }

    // 提取名称部分
    char name[20];
    strncpy(name, input, name_len);
    name[name_len] = '\0';

    // 解析组号
    PID_Params *target_pid = NULL;
    char *suffix = strrchr(name, '_'); // 查找最后的 '_'
    if (suffix != NULL && (*(suffix + 1) == '1' || *(suffix + 1) == '2')) {
        int group = *(suffix + 1) - '0';
        *suffix = '\0'; // 截断 `_1` 或 `_2`
        target_pid = (group == 1) ? pid_params_1 : pid_params_2;
    } else {
        printf("未知的 PID 组: %s\n", name);
        return;
    }

    // 解析数值部分
    if (strcmp(name, "Current_SET") == 0) {
        target_pid->Current_set_flag = (uint8_t)strtol(colon_pos + 1, NULL, 10);
        printf("Current_SET %d\n", target_pid->Current_set_flag);
    } else if (strcmp(name, "Current_KP") == 0) {
        target_pid->Current_KP = strtof(colon_pos + 1, NULL);
    } else if (strcmp(name, "Current_KI") == 0) {
        target_pid->Current_KI = strtof(colon_pos + 1, NULL);
    } else if (strcmp(name, "Current_KD") == 0) {
        target_pid->Current_KD = strtof(colon_pos + 1, NULL);
    } else if (strcmp(name, "Iq") == 0) {
        target_pid->Iq = strtof(colon_pos + 1, NULL);
    } else if (strcmp(name, "Id") == 0) {
        target_pid->Id = strtof(colon_pos + 1, NULL);
    } else if (strcmp(name, "Speed_SET") == 0) {
        target_pid->Speed_set_flag = (uint8_t)strtol(colon_pos + 1, NULL, 10);
        printf("Speed_SET %d\n", target_pid->Speed_set_flag);
    } else if (strcmp(name, "Speed_KP") == 0) {
        target_pid->Speed_KP = strtof(colon_pos + 1, NULL);
    } else if (strcmp(name, "Speed_KI") == 0) {
        target_pid->Speed_KI = strtof(colon_pos + 1, NULL);
    } else if (strcmp(name, "Speed_KD") == 0) {
        target_pid->Speed_KD = strtof(colon_pos + 1, NULL);
    } else if (strcmp(name, "Speed") == 0) {
        target_pid->Speed = strtof(colon_pos + 1, NULL);
    } else {
        printf("UNknow name %s\n", name);
        return;
    }

    printf("set %s is %s\n", name, colon_pos + 1);
}
