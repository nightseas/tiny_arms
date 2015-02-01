/**
  ******************************************************************************
  * @file    i2c_master.c
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
#define I2C_OWN_ADDR	0x00
#define I2C_SHORT_TIMEOUT			((uint32_t)0x1000)
#define I2C_LONG_TIMEOUT			((uint32_t)(10 * I2C_SHORT_TIMEOUT))  
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t  I2C2_Timeout = I2C_LONG_TIMEOUT; 

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void I2C2_Config(void)
{
	I2C_InitTypeDef    I2C_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	
	RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_1);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
 
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter = 0x00;
  I2C_InitStructure.I2C_OwnAddress1 = I2C_OWN_ADDR;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_Timing = 0xB0420F13;
  I2C_Init(I2C2, &I2C_InitStructure);
 
  I2C_Cmd(I2C2, ENABLE);
}

int I2C2_WriteCmd(uint8_t DevAddr, uint8_t CmdData)
{
	I2C2_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY))
	{
		if(I2C2_Timeout-- == 0)
			return I2C_BUS_ERR;
	}	
	
	I2C_TransferHandling(I2C2,DevAddr,1,I2C_AutoEnd_Mode,I2C_Generate_Start_Write);
	I2C2_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_TXIS) == RESET)
	{
		if(I2C2_Timeout-- == 0)
			return I2C_ADDR_ERR;
	}
	
	I2C_SendData(I2C2,CmdData);
	I2C2_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_STOPF) == RESET)
	{
		if(I2C2_Timeout-- == 0)
			return I2C_STOP_ERR;
	}
	I2C_ClearFlag(I2C2, I2C_ICR_STOPCF);
	
	return I2C_SUCCESS;
}


int I2C2_WriteByte(uint8_t DevAddr, uint8_t RegAddr, uint8_t RegData)
{
	uint8_t TmpData = RegData;
	return I2C2_BufferWrite(DevAddr, RegAddr, &TmpData, 1);
}

int I2C2_BufferWrite(uint8_t DevAddr, uint8_t RegAddr, uint8_t* DataBuf, uint16_t DataLen)
{
	I2C2_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY))
	{
		if(I2C2_Timeout-- == 0)
			return I2C_BUS_ERR;
	}	
	
	I2C_TransferHandling(I2C2,DevAddr,1,I2C_Reload_Mode,I2C_Generate_Start_Write);
	I2C2_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_TXIS) == RESET)
	{
		if(I2C2_Timeout-- == 0)
			return I2C_ADDR_ERR;
	}
	
	I2C_SendData(I2C2,RegAddr);
	I2C2_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_TCR) == RESET)
	{
		if(I2C2_Timeout-- == 0)
		{
			return I2C_DATA_ERR;
		}		
	}
	
	I2C_TransferHandling(I2C2,DevAddr,DataLen,I2C_AutoEnd_Mode,I2C_No_StartStop);
	while(DataLen--)
	{
		I2C2_Timeout = I2C_LONG_TIMEOUT;
		while(I2C_GetFlagStatus(I2C2,I2C_FLAG_TXIS) == RESET)
		{
			if(I2C2_Timeout-- == 0)
			{
			I2C_GenerateSTOP(I2C2, ENABLE);
			return I2C_DATA_ERR;
		}	
		}
		I2C_SendData(I2C2, *DataBuf);
		DataBuf++;
	}
	
	I2C2_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_STOPF) == RESET)
	{
		if(I2C2_Timeout-- == 0)
			return I2C_STOP_ERR;
	}
	I2C_ClearFlag(I2C2, I2C_ICR_STOPCF);
	
	return I2C_SUCCESS;
}


int I2C2_RandomRead(uint8_t DevAddr, uint8_t RegAddr, uint8_t* DataBuf, uint16_t DataLen)
{  
	I2C2_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY))
	{
		if(I2C2_Timeout-- == 0)
			return I2C_BUS_ERR;
	}	
	
	I2C_TransferHandling(I2C2,DevAddr,1,I2C_SoftEnd_Mode,I2C_Generate_Start_Write);
	I2C2_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_TXIS) == RESET)
	{
		if(I2C2_Timeout-- == 0)
			return I2C_ADDR_ERR;
	}	
	
	I2C_SendData(I2C2,RegAddr);
	I2C2_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_TC) == RESET)
	{
		if(I2C2_Timeout-- == 0)
			return I2C_DATA_ERR;
	}	
	
	I2C_TransferHandling(I2C2,DevAddr,DataLen,I2C_AutoEnd_Mode,I2C_Generate_Start_Read);
	while(DataLen)
	{
		I2C2_Timeout = I2C_LONG_TIMEOUT;
		while(I2C_GetFlagStatus(I2C2,I2C_FLAG_RXNE) == RESET)
		{
			if(I2C2_Timeout-- == 0)
				return I2C_DATA_ERR;
		}	
		*DataBuf = I2C_ReceiveData(I2C2);
		DataBuf++;
		DataLen--;
	}
	
	I2C2_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_STOPF) == RESET)
	{
		if(I2C2_Timeout-- == 0)
			return I2C_STOP_ERR;
	}	
	
	return I2C_SUCCESS;
}


int I2C2_CurrentRead(uint8_t DevAddr, uint8_t* DataBuf,  uint16_t DataLen)
{  
	I2C2_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY))
	{
		if(I2C2_Timeout-- == 0)
			return I2C_BUS_ERR;
	}	
			
	I2C_TransferHandling(I2C2,DevAddr,DataLen,I2C_AutoEnd_Mode,I2C_Generate_Start_Read);
	while(DataLen)
	{
		I2C2_Timeout = I2C_LONG_TIMEOUT;
		while(I2C_GetFlagStatus(I2C2,I2C_FLAG_RXNE) == RESET)
		{
			if(I2C2_Timeout-- == 0)
				return I2C_DATA_ERR;
		}	
		*DataBuf = I2C_ReceiveData(I2C2);
		DataBuf++;
		DataLen--;
	}
	
	I2C2_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_STOPF) == RESET)
	{
		if(I2C2_Timeout-- == 0)
			return I2C_STOP_ERR;
	}	
	
	return I2C_SUCCESS;
}

int I2C2_GetStatus(uint8_t DevAddr)
{
  uint32_t I2C2_Timeout = I2C_LONG_TIMEOUT;
	
	I2C2_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY))
	{
		if(I2C2_Timeout-- == 0)
			return I2C_BUS_ERR;
	}	  
  I2C_TransferHandling(I2C2, DevAddr, 0, I2C_AutoEnd_Mode, I2C_No_StartStop);  
  I2C_ClearFlag(I2C2, I2C_ICR_NACKCF | I2C_ICR_STOPCF);  
  I2C_GenerateSTART(I2C2, ENABLE);    
  while ((I2C_GetFlagStatus(I2C2, I2C_ISR_STOPF) == RESET) && (I2C2_Timeout-- != 0));   
  if ((I2C_GetFlagStatus(I2C2, I2C_ISR_NACKF) != RESET) || (I2C2_Timeout == 0))
  {      
    I2C_ClearFlag(I2C2, I2C_ICR_NACKCF | I2C_ICR_STOPCF);    
    return I2C_ADDR_ERR;
  }
  else
  {
    I2C_ClearFlag(I2C2, I2C_ICR_STOPCF);    
    return I2C_SUCCESS;
  }    
}
