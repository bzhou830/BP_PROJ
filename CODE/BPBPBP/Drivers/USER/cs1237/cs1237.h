#ifndef __CS_1237_H
#define __CS_1237_H

#include "stm32f1xx.h"


/**
*@brief gpio define
*/
#define CS1237_PORT     GPIOD
#define CS1237_SDA_PIN  GPIO_PIN_8
#define CS1237_SCL_PIN  GPIO_PIN_9

#define FILT_N   34

enum
{
	IO_INPUT,
	IO_OUTPUT,
	ERR,
};



void cs1237_RdDataTest(void);


#endif



