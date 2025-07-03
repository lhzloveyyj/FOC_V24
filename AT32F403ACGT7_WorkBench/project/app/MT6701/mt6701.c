#include "mt6701.h"  

#define SPI1_CS1_L  gpio_bits_reset(GPIOB, GPIO_PINS_0);     //CS1_L
#define SPI1_CS1_H  gpio_bits_set(GPIOB, GPIO_PINS_0);       //CS1_H

unsigned char SPIx_ReadWriteByte(unsigned char byte)
{
	unsigned short retry = 0;
	
	while (spi_i2s_flag_get(SPI1, SPI_I2S_TDBE_FLAG) == RESET)
	{
		if(++retry>200)return 0;
	}
	spi_i2s_data_transmit(SPI1, byte);
	retry = 0;
	while (spi_i2s_flag_get(SPI1, SPI_I2S_RDBF_FLAG) == RESET) 
	{
		if(++retry>200)return 0;
	}
	return spi_i2s_data_receive(SPI1);
}

float ReadAngle(void)
{
	uint8_t back1,back2,back3;
	//uint8_t MagField;
	uint32_t temp;
	float angle ;
	
	SPI1_CS1_L;
	back1 = SPIx_ReadWriteByte(0);
	back2 = SPIx_ReadWriteByte(0);
	back3 = SPIx_ReadWriteByte(0);
	SPI1_CS1_H;
	
	temp = ((back1<<16)|(back2<<8)|back3)>>10;
	//MagField=(temp>>6)&0x0F;
	angle =((float)temp * 360)/16384.0f;
	angle = angle * 6.28318f / 360.0f;
	return angle;
}

