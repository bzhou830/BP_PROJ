#ifndef __DELAY_H__
#define __DELAY_H__

#include "stm32f1xx.h"


inline void delay_xus(uint32_t xus) 
{
    while(--xus);
}


void delay_xms(uint32_t xms);


#endif




