#include "key.h"
#include "./delay/delay.h"




uint8_t k_read()
{
    uint8_t key_value = 0;
    if(K1_READ() == GPIO_PIN_RESET)
    {
        delay_xms(5);
        if(K1_READ() == GPIO_PIN_RESET)
        {
            key_value = BUTTON_K1;
            while(K1_READ() == GPIO_PIN_RESET);
        }
        
    }
    
    if(K2_READ() == GPIO_PIN_RESET)
    {
        delay_xms(5);
        if(K2_READ() == GPIO_PIN_RESET)
        {
            key_value = BUTTON_K2;
            while(K2_READ() == GPIO_PIN_RESET);
        }
    }

    if(K3_READ() == GPIO_PIN_RESET)
    {
        delay_xms(5);
        if(K3_READ() == GPIO_PIN_RESET)
        {
            key_value = BUTTON_K3;
            while(K3_READ() == GPIO_PIN_RESET);
        }
    }

    if(K4_READ() == GPIO_PIN_RESET)
    {
        delay_xms(5);
        if(K4_READ() == GPIO_PIN_RESET)
        {
            key_value = BUTTON_K4;
            while(K4_READ() == GPIO_PIN_RESET);
        }
    }    
    return key_value;
}











