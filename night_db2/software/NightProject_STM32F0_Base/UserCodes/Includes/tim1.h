/**
  ******************************************************************************
  * @file    TIM1.h
  * @author  Nightmare
  * @version V1.0.0
  * @date    2014.5.6
  * @brief   Project config file.
  ******************************************************************************
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM1_H
#define __TIM1_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void TIM1_General_Config(uint32_t freq);
void TIM1_General_Disable(void);
void TIM1_BRK_UP_TRG_COM_IRQHandler(void);

void TIM1_PWM_Config(uint32_t freq);
void TIM1_PWM_SetDuty(uint8_t ch, uint32_t duty);

#endif /* __TIM1_H */

/************************ END OF FILE****/




