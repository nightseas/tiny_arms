/**
  ******************************************************************************
  * @file    at24c128.c
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
#define EEP_SHORT_TIMEOUT			((uint32_t)0x1000)
#define EEP_LONG_TIMEOUT			((uint32_t)(10 * EEP_SHORT_TIMEOUT))  
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t  EEP_Timeout = EEP_LONG_TIMEOUT; 

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
int EEP_WriteData(uint8_t DevAddr, uint16_t RegAddr, uint8_t* DataBuf, uint16_t DataLen)
{
	EEP_Timeout = EEP_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY))
	{
		if(EEP_Timeout-- == 0)
			return ERROR;
	}	
	
	I2C_TransferHandling(I2C2,DevAddr,2,I2C_Reload_Mode,I2C_Generate_Start_Write);
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_TXIS) == RESET)
	{
		if(EEP_Timeout-- == 0)
			return ERROR;
	}	
	
	I2C_SendData(I2C2,RegAddr>>8);
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_TXIS) == RESET)
	{
		if(EEP_Timeout-- == 0)
			return ERROR;
	}		
	
	I2C_SendData(I2C2,RegAddr&0xFF);
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_TCR) == RESET)
	{
		if(EEP_Timeout-- == 0)
			return ERROR;
	}	
	
	I2C_TransferHandling(I2C2,DevAddr,DataLen,I2C_AutoEnd_Mode,I2C_No_StartStop);
	while(DataLen--)
	{
		while(I2C_GetFlagStatus(I2C2,I2C_FLAG_TXIS) == RESET)
	{
		if(EEP_Timeout-- == 0)
			return ERROR;
	}	
		I2C_SendData(I2C2, *DataBuf);
		DataBuf++;
	}
	
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_STOPF) == RESET)
	{
		if(EEP_Timeout-- == 0)
			return ERROR;
	}	
	I2C_ClearFlag(I2C2, I2C_ICR_STOPCF);
	return SUCCESS;
}


int EEP_ReadData(uint8_t DevAddr, uint16_t RegAddr, uint8_t* DataBuf, uint16_t DataLen)
{ 
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY))
	{
		if(EEP_Timeout-- == 0)
			return ERROR;
	}	
	
	I2C_TransferHandling(I2C2,DevAddr,2,I2C_SoftEnd_Mode,I2C_Generate_Start_Write);
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_TXIS) == RESET)
	{
		if(EEP_Timeout-- == 0)
			return ERROR;
	}	
	
	I2C_SendData(I2C2,RegAddr>>8);
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_TXIS) == RESET)
	{
		if(EEP_Timeout-- == 0)
			return ERROR;
	}	
	
	I2C_SendData(I2C2,RegAddr&0xFF);
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_TC) == RESET)
	{
		if(EEP_Timeout-- == 0)
			return ERROR;
	}	
	
	I2C_TransferHandling(I2C2,DevAddr,DataLen,I2C_AutoEnd_Mode,I2C_Generate_Start_Read);
	while(DataLen)
	{
		while(I2C_GetFlagStatus(I2C2,I2C_FLAG_RXNE) == RESET)
		{
			if(EEP_Timeout-- == 0)
				return ERROR;
		}	
		*DataBuf = I2C_ReceiveData(I2C2);
		DataBuf++;
		DataLen--;
	}
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_STOPF) == RESET)
	{
		if(EEP_Timeout-- == 0)
			return ERROR;
	}
	return SUCCESS;
}
