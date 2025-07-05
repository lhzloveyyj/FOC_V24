#ifndef __MT6701_H
#define __MT6701_H
#include "at32f403a_407.h"              

typedef struct {
    spi_type *SPIx;       
    gpio_type *CS_GPIO;   
    uint16_t CS_Pin;         
	float angle;
} MT6701_t;

extern MT6701_t mt6701;

float MT6701_GetAngle(MT6701_t *encoder);
float MT6701_GetAngleWrapper(void);

#endif
