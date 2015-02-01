/**
  ******************************************************************************
  * @file    at24c128.h
  * @author  Nightmare
  * @version V1.0.0
  * @date    2014.5.6
  * @brief   Project config file.
  ******************************************************************************
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AT24C128_H
#define __AT24C128_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
int EEP_WriteData(uint8_t DevAddr, uint16_t RegAddr, uint8_t* DataBuf, uint16_t DataLen);
int EEP_ReadData(uint8_t DevAddr, uint16_t RegAddr, uint8_t* DataBuf, uint16_t DataLen);
#endif /* __MAIN_H */

/************************ END OF FILE****/




