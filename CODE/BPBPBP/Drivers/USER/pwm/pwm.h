#ifndef __PWM_H__
#define __PWM_H__


#include "./platform/platform.h"



/**
*@brief pwm chl number
*/
enum
{
	PWM_1,
	PWM_2,
	PWM_NUMBER,
};

/**
*@brief pwm info struct
*/
typedef struct 
{
	TIM_HandleTypeDef *htim;
	uint32_t Channel;
}pwm_info_t;




/**
*@brief functions
*/
void pwm_output_start(uint8_t n);
void pwm_output_stop(uint8_t n);

int inflate_control(uint8_t percent);
int deflate_control(uint8_t percent);

void inflate_control_b(uint32_t p);
#endif




