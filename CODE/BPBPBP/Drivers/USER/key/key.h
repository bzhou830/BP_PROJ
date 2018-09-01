#ifndef __KEY_H__
#define __KEY_H__

#include "./platform/platform.h"



#define K1_READ()         HAL_GPIO_ReadPin(K_PORT, K1_PIN)
#define K2_READ()         HAL_GPIO_ReadPin(K_PORT, K2_PIN)
#define K3_READ()         HAL_GPIO_ReadPin(K_PORT, K3_PIN)
#define K4_READ()         HAL_GPIO_ReadPin(K_PORT, K4_PIN)




#define BUTTON_K1           1
#define BUTTON_K2           2
#define BUTTON_K3           3
#define BUTTON_K4           4





uint8_t k_read(void);




#endif




