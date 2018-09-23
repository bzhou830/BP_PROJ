/**
*@brief pwm functions file
*/
#include "wholeconfig.h"
#include "pwm.h"

extern TIM_HandleTypeDef htim1;

static uint8_t control_index = 0xff;

/**
*@brief add pwm channel info
*/
const pwm_info_t pwm_info[PWM_NUMBER] = 
{
	[PWM_1] = {
		.htim = &htim1,
		.Channel = TIM_CHANNEL_1
	},
	[PWM_2] = {
		.htim = &htim1,
		.Channel = TIM_CHANNEL_2
	},
};




/**
*@brief start pwm channel 
*@param xchannel will start 
*/
void pwm_output_start(uint8_t n)
{
	if(n >= PWM_NUMBER)
	{
		return;
	}
	HAL_TIM_PWM_Start(pwm_info[n].htim, pwm_info[n].Channel);
}



/**
*@brief stop pwm channel 
*@param xchannel will stop 
*/
void pwm_output_stop(uint8_t n)
{
	if(n >= PWM_NUMBER)
	{
		return;
	}
	HAL_TIM_PWM_Stop(pwm_info[n].htim, pwm_info[n].Channel);
}



/**
*@brief change pwm value 
*@param xchannel & value (0--100)%
*/
static int pwm_set_duty_ratio(uint8_t n, uint8_t percent)
{
	uint32_t tmp;
	if(n >= PWM_NUMBER || percent > 100)
	{
		return -1;
	}
	tmp = __HAL_TIM_GET_AUTORELOAD(pwm_info[n].htim);
	tmp = (uint32_t)(tmp * percent / 100.0 + 0.5);
	__HAL_TIM_SET_COMPARE(pwm_info[n].htim, pwm_info[n].Channel, tmp);
	return 0;
}



/**
*@brief Inflate / deflate
*/
int inflate_control(uint8_t percent)
{
	int ret = 0;
	if(INFLATE_CHL != control_index)
	{
		ret = pwm_set_duty_ratio(DEFLATE_CHL, 0);
		if(ret < 0)
		{
			return -1;
		}
		control_index = INFLATE_CHL;
	}
	return pwm_set_duty_ratio(INFLATE_CHL, percent);
}

int deflate_control(uint8_t percent)
{
	int ret = 0;
	if(DEFLATE_CHL != control_index)
	{
		ret = pwm_set_duty_ratio(INFLATE_CHL, 0);
		if(ret < 0)
		{
			return -1;
		}
		control_index = DEFLATE_CHL;
	}
	return pwm_set_duty_ratio(DEFLATE_CHL, percent);
}

volatile static uint32_t pwm_p = 50;

void inflate_control_b(uint32_t p)
{
    pwm_p = p;
}

void inflate_init()
{
    HAL_GPIO_WritePin(PWM_PORT, INFLATE_B, GPIO_PIN_SET);
    HAL_GPIO_WritePin(PWM_PORT, INFLATE_A, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_PORT, LED_1_PIN, GPIO_PIN_SET);
    
    HAL_GPIO_WritePin(MOTO_INFLATE_PORT, MOTO_OUT4_P, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTO_INFLATE_PORT, MOTO_OUT3_N, GPIO_PIN_RESET);   
    HAL_GPIO_WritePin(MOTO_INFLATE_PORT, MOTO_OUT2_N, GPIO_PIN_RESET); 
    HAL_GPIO_WritePin(MOTO_DEFLATE_PORT, MOTO_OUT1_P, GPIO_PIN_SET);     
}

void led_control(uint8_t n)
{
    if(n)
    {
        HAL_GPIO_WritePin(LED_PORT, LED_1_PIN, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(LED_PORT, LED_1_PIN, GPIO_PIN_SET);
    }
}


void deflate_onoff(uint8_t n)
{
    if(n)
    {
        HAL_GPIO_WritePin(MOTO_DEFLATE_PORT, MOTO_OUT1_P, GPIO_PIN_RESET); 
    }
    else
    {
        HAL_GPIO_WritePin(MOTO_DEFLATE_PORT, MOTO_OUT1_P, GPIO_PIN_SET);  
    }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint32_t p_count = 0;
    p_count++;
    
    if(pwm_p == 0)
    {
        p_count = 0;
    }
    
    if(p_count == pwm_p)
    {
        HAL_GPIO_WritePin(MOTO_INFLATE_PORT, MOTO_OUT4_P, GPIO_PIN_RESET);
    }
    else if(p_count > 100)
    {
        HAL_GPIO_WritePin(MOTO_INFLATE_PORT, MOTO_OUT4_P, GPIO_PIN_SET);
        p_count = 0;
    }
}



