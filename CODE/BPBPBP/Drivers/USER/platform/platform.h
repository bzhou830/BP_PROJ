#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "stm32f1xx.h"


/**
*@brief gpio define
*/
#define CS1237_PORT     GPIOD
#define CS1237_SDA_PIN  GPIO_PIN_8
#define CS1237_SCL_PIN  GPIO_PIN_9


#define FLASH_PORT      GPIOD
#define FLASH_CLK_PIN   GPIO_PIN_6
#define FLASH_CS_PIN    GPIO_PIN_5
#define FLASH_MOSI_PIN  GPIO_PIN_7
#define FLASH_MISO_PIN  GPIO_PIN_4


#define K_PORT          GPIOB
#define K1_PIN          GPIO_PIN_15
#define K2_PIN          GPIO_PIN_14
#define K3_PIN          GPIO_PIN_13
#define K4_PIN          GPIO_PIN_12



#define PWM_PORT        GPIOE
#define PWM_PIN         GPIO_PIN_9


#define INFLATE_CHL    PWM_1
#define DEFLATE_CHL    PWM_2



#endif


