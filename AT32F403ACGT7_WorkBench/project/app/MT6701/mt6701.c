#include "mt6701.h"  

void MT6701_StartRead(MT6701_t *encoder)
{
    /* 拉低 CS */
    gpio_bits_reset(encoder->CS_GPIO, encoder->CS_Pin);
    
    /* 清除标志 */
    encoder->data_ready = 0;
    
    /* 使能 DMA */
    dma_channel_enable(encoder->DMA_TX, TRUE);
    dma_channel_enable(encoder->DMA_RX, TRUE);
    
    /* 启动 SPI 发送 (dummy byte) */
    while (spi_i2s_flag_get(encoder->SPIx, SPI_I2S_TDBE_FLAG) == RESET);
    spi_i2s_data_transmit(encoder->SPIx, 0x00);
}

float MT6701_GetAngle(MT6701_t *encoder)
{
    MT6701_StartRead(encoder);
    
    while (!encoder->data_ready);
    
    float angle = 0.0f;
    if (encoder->data_ready)
    {
        /* 解析角度数据 */
        uint32_t temp = ((uint16_t)encoder->rx_buf[0] << 6) | (encoder->rx_buf[1] >> 2);
        //angle = ((float)temp * 360) / 16384.0f;
        //angle = angle * 6.28318f / 360.0f;
        angle = (float)temp;
        /* 清除标志 */
        //encoder->data_ready = 0;
    }
    
    return angle;
}

MT6701_t mt6701 = {SPI1, GPIOB, GPIO_PINS_0, DMA1_CHANNEL2, DMA1_CHANNEL1, {0x00, 0x00, 0x00}, {0}, 0};




