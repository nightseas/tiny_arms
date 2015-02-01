/**
  ******************************************************************************
  * @file    i2c_slave.c
  * @author  Nightmare
  * @version V1.0.0
  * @date    2014.5.6
  * @brief   
  ******************************************************************************
  *
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "appconfig.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define I2C_OWN_ADDR	0x70
#define I2C_REG_LEN		0xFF
#define I2C_SHORT_TIMEOUT			((uint32_t)0x1000)
#define I2C_LONG_TIMEOUT			((uint32_t)(10 * I2C_SHORT_TIMEOUT))  
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t  I2C1_Timeout = I2C_LONG_TIMEOUT; 
uint8_t I2C_Register_Buff[I2C_REG_LEN];
uint8_t I2C_RegAddr = 0;
int I2C_RegAddr_Flag = 1;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void I2C1_Config(void)
{
	I2C_InitTypeDef		I2C_InitStructure;
	GPIO_InitTypeDef	GPIO_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_1);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
 
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter = 0x00;
  I2C_InitStructure.I2C_OwnAddress1 = I2C_OWN_ADDR<<1;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_Timing = 0xB0420F13;
  I2C_Init(I2C1, &I2C_InitStructure);
 
  I2C_Cmd(I2C1, ENABLE);
	
	I2C_ITConfig(I2C1, I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_TXI | I2C_IT_NACKI | I2C_IT_STOPI, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = I2C1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	memset(I2C_Register_Buff, 0, I2C_REG_LEN);
}

void I2C1_IRQHandler(void)
{
	if(I2C_GetITStatus(I2C1, I2C_IT_ADDR) == SET)
	{
		//printf("\nIT_ADDR!\n");
		I2C_RegAddr_Flag = 1;
		I2C_ClearITPendingBit(I2C1, I2C_IT_ADDR);
	}
	else if(I2C_GetITStatus(I2C1, I2C_IT_RXNE) == SET)
	{
		//printf("\nIT_RXNE!\n");
		if(I2C_RegAddr_Flag)
		{
			I2C_RegAddr = I2C_ReceiveData(I2C1);
			I2C_RegAddr_Flag = 0;
		}
		else
		{
			if(I2C_RegAddr < I2C_REG_LEN)
				I2C_Register_Buff[I2C_RegAddr] = I2C_ReceiveData(I2C1);
			else
				I2C_ReceiveData(I2C1);
			
			I2C_RegAddr++;
			if(I2C_RegAddr >= I2C_REG_LEN)
				I2C_RegAddr = 0;
		}
	}
	else if(I2C_GetITStatus(I2C1, I2C_IT_TXIS) == SET)
	{
		//printf("\nIT_TXIS!\n");
		if(I2C_RegAddr < I2C_REG_LEN)
		{			
			I2C_SendData(I2C1, I2C_Register_Buff[I2C_RegAddr]);
			I2C_RegAddr++;
			if(I2C_RegAddr >= I2C_REG_LEN)
				I2C_RegAddr = 0;
			I2C_ClearITPendingBit(I2C1, I2C_IT_TXIS);
		}
		else
		{
			I2C_SendData(I2C1, 0xFF);	
		}		
	}
	else if(I2C_GetITStatus(I2C1, I2C_IT_NACKF) == SET)
	{
		//printf("\nIT_NACKF!\n");

		I2C_RegAddr--;
		if(I2C_RegAddr >= I2C_REG_LEN)
			I2C_RegAddr = I2C_REG_LEN-1;
		I2C_ClearITPendingBit(I2C1, I2C_IT_NACKF);
	}
	else if(I2C_GetITStatus(I2C1, I2C_IT_STOPF) == SET)
	{
		//printf("\nIT_STOPF!\n");
		I2C1->ISR |= I2C_ISR_TXE;
		I2C_ClearITPendingBit(I2C1, I2C_IT_STOPF);
	}
	else if(I2C_GetITStatus(I2C1, I2C_IT_TIMEOUT) == SET)
	{
		//printf("\nIT_TIMEOUT!\n");
		I2C_ClearITPendingBit(I2C1, I2C_IT_TIMEOUT);
		
	}
}

