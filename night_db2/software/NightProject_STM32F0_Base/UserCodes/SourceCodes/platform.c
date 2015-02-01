/**
  ******************************************************************************
  * @file    platform.c
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
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef        GPIO_InitStructure;
static __IO uint32_t Sys_Tick_Cnt = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void Platform_Config(void)
{
	GPIO_Config();
	SysTickTimer_Config();
	USART_Config();
	SPI_Config();
	//I2C2_Config();
	//I2C1_Config();
}

void GPIO_Config(void)
{
/* GPIOC Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

  /* Configure PB5 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	Sys_LED_Off(1);
	Sys_LED_Off(2);	
}

void SysTickTimer_Config(void)
{
	SysTick_Config(SystemCoreClock / 1000);
}

void SysTickTimer_Stop(void)
{
	SysTick_Config(0);
}

void Sys_Decrease_US(void)
{
	if(Sys_Tick_Cnt > 0)
		Sys_Tick_Cnt--;
}

void Sys_Delay_MS(__IO uint32_t time)
{
	Sys_Tick_Cnt = time;
	while(Sys_Tick_Cnt != 0);
}

void Sys_LED_On(int num)
{
	switch(num)
	{
		case 1:
			GPIO_ResetBits(GPIOB, GPIO_Pin_13);
		break;
			
		case 2:
			GPIO_ResetBits(GPIOB, GPIO_Pin_14);
		break;
	}	
}

void Sys_LED_Off(int num)
{
	switch(num)
	{
		case 1:
			GPIO_SetBits(GPIOB, GPIO_Pin_13);
		break;
			
		case 2:
			GPIO_SetBits(GPIOB, GPIO_Pin_14);
		break;
	}	
}
