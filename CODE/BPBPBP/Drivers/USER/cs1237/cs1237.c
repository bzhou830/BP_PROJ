#include "cs1237.h"
#include "stdio.h"
#include "string.h"
#include "wholeconfig.h"

/**
 simple delay function. xuS delay
*/
void cs1237_delay(uint8_t lucN)  //2us~150us
{
	uint16_t tmp;
	
	tmp = 100*lucN;
	while(tmp--);
}

void cs1237_sda_dir(uint8_t dir)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if(dir==IO_INPUT)											
	{
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}
	else if(dir==IO_OUTPUT)
	{
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	}
	GPIO_InitStruct.Pin = CS1237_SDA_PIN;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(CS1237_PORT, &GPIO_InitStruct);
}

void cs1237_sda_output(uint8_t status)
{
	if(status == 0)
		HAL_GPIO_WritePin(CS1237_PORT, CS1237_SDA_PIN, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(CS1237_PORT, CS1237_SDA_PIN, GPIO_PIN_SET);
}

void cs1237_scl_output(uint8_t status)
{
	if(status == 0)
		HAL_GPIO_WritePin(CS1237_PORT, CS1237_SCL_PIN, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(CS1237_PORT, CS1237_SCL_PIN, GPIO_PIN_SET);
}

/**
*@brief read sda status
*/
uint8_t cs1237_sda_read(void)
{
	return HAL_GPIO_ReadPin(CS1237_PORT, CS1237_SDA_PIN);
}

uint8_t cs1237_wrtdata(uint8_t lucConfigData)
{
	uint8_t lucRet = 0;
	uint8_t lucData = 0;
	uint8_t lucBitCnt = 0;
	
	cs1237_sda_dir(IO_INPUT); 
	cs1237_scl_output(0);
	do
	{
		cs1237_delay(1);
	}while(cs1237_sda_read()==RESET);
	
	do
	{
		cs1237_delay(1);
	}
	while(cs1237_sda_read()!=RESET);
	cs1237_delay(1);
	
	for(lucBitCnt=1; lucBitCnt < 30; lucBitCnt++)
	{
		cs1237_scl_output(1);
		cs1237_delay(1);
		cs1237_scl_output(0);
		cs1237_delay(1);
		if(lucBitCnt == 25)
		{
			if(cs1237_sda_read()!=RESET)
			{
				lucRet = 1;
			}
			else
			{
				lucRet = 0;
			}
		}
	}	

	cs1237_sda_dir(IO_OUTPUT);  //写数据时SDA脚设为输出
	lucData = 0x65;             //写配置，读则0x56
	lucData <<= 1;
	for(lucBitCnt=30; lucBitCnt < 37; lucBitCnt++)
	{	
		cs1237_scl_output(1);
		if((lucData&0x80) == 0x80)
		{
			cs1237_sda_output(1);
		}
		else
		{
			cs1237_sda_output(0);
		}
		cs1237_delay(1);
		cs1237_scl_output(0);
		cs1237_delay(1);
	  lucData <<= 1;
	}
	
	for(lucBitCnt=37; lucBitCnt < 38; lucBitCnt++)
	{
		cs1237_scl_output(1);
		cs1237_delay(1);
		cs1237_scl_output(0);
		cs1237_delay(1);
	}
	
	lucData = lucConfigData;
	for(lucBitCnt=38; lucBitCnt < 46; lucBitCnt++) //38_45
	{  
		cs1237_scl_output(1);
		if(lucData&0x80)
		{
			cs1237_sda_output(1);
		}
		else
		{
			cs1237_sda_output(0);
		}
		cs1237_delay(1);
		cs1237_scl_output(0);
    cs1237_delay(1);
		lucData <<= 1;
	}
	
	for(lucBitCnt=46; lucBitCnt<47; lucBitCnt++)
	{
		cs1237_scl_output(1);
		cs1237_delay(1);
		cs1237_scl_output(0);
		cs1237_delay(1);
	}
	cs1237_sda_dir(IO_INPUT);  
	return lucRet;
}

/**
*@brief read cs1237 data
*/
uint8_t CS1237_ReadData(uint32_t *lulData)
{
	uint16_t lucBitCnt = 0;
	
	cs1237_sda_dir(IO_INPUT); 
	cs1237_scl_output(0);
	do
	{
		cs1237_delay(1);
		lucBitCnt++;
		if(lucBitCnt>1000)
		{
			printf("Error 01 \n");
			return 0;
		}
	}while(cs1237_sda_read()==GPIO_PIN_RESET);
	
  lucBitCnt = 0;
	do
	{
		cs1237_delay(1);
		lucBitCnt++;
		if(lucBitCnt>1000)
		{
			printf("Error 02 \n");
			return 0;
		}
	}
	while(cs1237_sda_read()!=GPIO_PIN_RESET);
	cs1237_delay(5);
	
	*lulData = 0;
	for(lucBitCnt=0; lucBitCnt<24; lucBitCnt++)
	{
		*lulData<<=1;
		cs1237_scl_output(1);
		cs1237_delay(1);
		cs1237_scl_output(0);
		cs1237_delay(1);
		
		if(cs1237_sda_read()!=GPIO_PIN_RESET)
		{
			*lulData |= 0x01;
		}
		else
		{
			*lulData &= 0x00FFFFFE;
		}
	}
	return 1;
}

/**
*@brief test cs1237 data
*/
void cs1237_RdDataTest(void)
{
	static uint32_t lulTim = 0;
	static uint8_t  lucStep = 0;
	uint32_t lulAdcVal = 0;
	
	if(lucStep==0)
	{
		if(cs1237_wrtdata(0x2C))  //0x62
		{
			b_log("cs1237 init ok \n");
			lucStep = 1;
		}
	}
	else if(lucStep==1)
	{
		if(CS1237_ReadData(&lulAdcVal))
		{
			if(lulAdcVal&0x00800000)  
			{		
				lulAdcVal = ~lulAdcVal + 1;
				lulAdcVal &= 0x00FFFFFF;
				b_log("cs1237:- %d\n", lulAdcVal);
			}
			else 
			{
//				lulAdcVal = ~lulAdcVal + 1;
				lulAdcVal &= 0x00FFFFFF;
				b_log("cs1237: %d\n", lulAdcVal);
			}
			
			lulTim = HAL_GetTick();
		  lucStep = 2;
		}
	}
	else if(lucStep==2)
	{
		if(HAL_GetTick()-lulTim>=500)
		{
			lucStep = 1;
		}
	}
	
}
