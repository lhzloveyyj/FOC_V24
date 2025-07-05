#include "mt6701.h"  

#define SPI1_CS1_L  gpio_bits_reset(GPIOB, GPIO_PINS_0);     //CS1_L
#define SPI1_CS1_H  gpio_bits_set(GPIOB, GPIO_PINS_0);       //CS1_H

unsigned char SPIx_ReadWriteByte(MT6701_t *encoder)
{
	unsigned short retry = 0;
	gpio_bits_reset(encoder->CS_GPIO, encoder->CS_Pin);
	
	while (spi_i2s_flag_get(encoder->SPIx, SPI_I2S_TDBE_FLAG) == RESET)
	{
		if(++retry>10)return 0;
	}
	spi_i2s_data_transmit(encoder->SPIx, 0x00);
	retry = 0;
	while (spi_i2s_flag_get(encoder->SPIx, SPI_I2S_RDBF_FLAG) == RESET) 
	{
		if(++retry>10)return 0;
	}
	return spi_i2s_data_receive(encoder->SPIx);
}



float MT6701_GetAngle(MT6701_t *encoder)
{
	uint8_t back1,back2,back3;
	uint32_t temp;
	float angle ;
	
	gpio_bits_reset(encoder->CS_GPIO, encoder->CS_Pin);
	back1 = SPIx_ReadWriteByte(encoder);
	back2 = SPIx_ReadWriteByte(encoder);
	back3 = SPIx_ReadWriteByte(encoder);
	gpio_bits_set(encoder->CS_GPIO, encoder->CS_Pin);
	
	temp = ((back1<<16)|(back2<<8)|back3)>>10;
	angle =((float)temp * 360)/16384.0f;
	encoder->angle = angle * 6.28318f / 360.0f;
	return encoder->angle;
}

MT6701_t mt6701 = {SPI1, GPIOB, GPIO_PINS_0, };

float MT6701_GetAngleWrapper(void)
{
    return MT6701_GetAngle(&mt6701);
}
