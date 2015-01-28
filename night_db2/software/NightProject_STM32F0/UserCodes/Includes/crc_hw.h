/**
  ******************************************************************************
  * @file    crc_hw.h
  * @author  Nightmare
  * @version V1.0.0
  * @date    2014.5.6
  * @brief   Project config file.
  ******************************************************************************
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CRC_HW_H
#define __CRC_HW_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern const uint32_t CRC_POLY_32BIT;
extern const uint32_t CRC_POLY_16BIT;
extern const uint32_t CRC_POLY_8BIT;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void CRC_Config_32Bit(uint32_t init);
uint32_t CRC_Calc_32Bits(uint32_t* data, uint32_t size);

void CRC_Config(uint32_t width, uint32_t poly); //Only support in STM32F07x & STM32F04x
uint16_t CRC_Calc_16Bits(uint16_t* data, uint32_t size); //Only support in STM32F07x & STM32F04x
uint8_t CRC_Calc_8Bits(uint8_t* data, uint32_t size); //Only support in STM32F07x & STM32F04x

#endif /* __MAIN_H */

/************************ END OF FILE****/




