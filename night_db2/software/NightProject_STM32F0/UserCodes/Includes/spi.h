/**
  ******************************************************************************
  * @file    spi.h
  * @author  Nightmare
  * @version V1.0.0
  * @date    2014.5.6
  * @brief   Project config file.
  ******************************************************************************
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI2_H
#define __SPI2_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
// CS    SCLK   MISO   MOSI
//GPA4   GPA5   GPA6   GPA7
#define SPI_GPIO_CS                  	GPIOA
#define RCC_AHBPeriph_SPI_GPIO_CS   	RCC_AHBPeriph_GPIOA
#define SPI_GPIO_Pin_CS              	GPIO_Pin_4 
 
/* Select SPI Device: Chip Select pin low  */
#define SPI_CS_LOW()       GPIO_ResetBits(SPI_GPIO_CS, SPI_GPIO_Pin_CS)
/* Deselect SPI Device: Chip Select pin high */
#define SPI_CS_HIGH()      GPIO_SetBits(SPI_GPIO_CS, SPI_GPIO_Pin_CS)
/* Exported functions ------------------------------------------------------- */
void SPI_Config(void);
uint8_t SPI_ReadByte(void);
uint8_t SPI_SendByte(uint8_t byte);

#endif /* __SPI2_H */

/************************ END OF FILE****/




