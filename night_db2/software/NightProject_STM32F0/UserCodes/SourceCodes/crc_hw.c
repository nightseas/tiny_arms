/**
  ******************************************************************************
  * @file    crc_hw.c
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
const uint32_t CRC_POLY_32BIT = 0x04C11DB7;
const uint32_t CRC_POLY_16BIT = 0x8005;
const uint32_t CRC_POLY_8BIT = 0xD5;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void CRC_Config_32Bit(uint32_t init)
{
  /* Enable CRC AHB clock interface */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);

  /* DeInit CRC peripheral */
  CRC_DeInit();
  
  /* Init the INIT register */
  CRC_SetInitRegister(init);
}

void CRC_Config(uint32_t width, uint32_t poly)
{
  /* Enable CRC AHB clock interface */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);

  /* DeInit CRC peripheral */
  CRC_DeInit();
  
  /* Init the INIT register */
  CRC_SetInitRegister(0);
  
	/* Select polynomial size */
	if(width == 32)
		CRC_PolynomialSizeSelect(CRC_PolSize_32);  
	else if(width == 16)
		CRC_PolynomialSizeSelect(CRC_PolSize_16);
	else
		CRC_PolynomialSizeSelect(CRC_PolSize_8);
	
	/* Set the polynomial coefficients */
	CRC_SetPolynomial(poly);
}

uint32_t CRC_Calc_32Bits(uint32_t* data, uint32_t size)
{
	CRC_ResetDR();
	
	return CRC_CalcBlockCRC(data, size);
}

uint16_t CRC_Calc_16Bits(uint16_t* data, uint32_t size)
{
  uint16_t* dataEnd = data+size;

  /* Reset CRC data register to avoid overlap when computing new data stream */
  CRC_ResetDR();

  while(data < dataEnd)
  {
    /* Write the input data in the CRC data register */
    CRC_CalcCRC16bits(*data++);
  }
  /* Return the CRC value */
  return (uint16_t)CRC_GetCRC();
}

uint8_t CRC_Calc_8Bits(uint8_t* data, uint32_t size)
{
  uint8_t* dataEnd = data+size;

  /* Reset CRC data register to avoid overlap when computing new data stream */
  CRC_ResetDR();

  while(data < dataEnd)
  {
    /* Write the input data in the CRC data register */
    CRC_CalcCRC8bits(*data++);
  }
  /* Return the CRC value */
  return (uint8_t)CRC_GetCRC();
}
