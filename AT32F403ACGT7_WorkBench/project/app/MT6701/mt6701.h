#ifndef __MT6701_H
#define __MT6701_H
#include "at32f403a_407.h"              // Device header

#define MT6701_BUF_SIZE	3

typedef struct {
    spi_type *SPIx;       // SPI �˿�
    gpio_type *CS_GPIO;   // Ƭѡ GPIO �˿�
    uint16_t CS_Pin;         // Ƭѡ GPIO ����
    dma_channel_type *DMA_TX; // ���� DMA ͨ��
    dma_channel_type *DMA_RX; // ���� DMA ͨ��
    uint8_t tx_buf[MT6701_BUF_SIZE]; // ���ͻ�����
    uint8_t rx_buf[MT6701_BUF_SIZE]; // ���ջ�����
    volatile uint8_t data_ready;     // ����׼����־
} MT6701_t;

extern MT6701_t mt6701;

float MT6701_GetAngle(MT6701_t *encoder);

#endif
