/**
  ******************************************************************************
  * @file    tim16.h
  * @author  Nightmare
  * @version V1.0.0
  * @date    2014.5.6
  * @brief   Project config file.
  ******************************************************************************
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM16_H
#define __TIM16_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void TIM16_General_Config(uint32_t freq);
void TIM16_General_Disable(void);
void TIM16_IRQHandler(void);

void TIM16_PWM_Config(uint32_t freq);
void TIM16_PWM_SetDuty(uint8_t ch, uint32_t duty);

#endif /* __TIM16_H */

/************************ END OF FILE****/




