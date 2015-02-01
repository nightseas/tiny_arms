/**
  ******************************************************************************
  * @file    tim15.h
  * @author  Nightmare
  * @version V1.0.0
  * @date    2014.5.6
  * @brief   Project config file.
  ******************************************************************************
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM15_H
#define __TIM15_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void TIM15_General_Config(uint32_t freq);
void TIM15_General_Disable(void);
void TIM15_IRQHandler(void);

void TIM15_PWM_Config(uint32_t freq);
void TIM15_PWM_SetDuty(uint8_t ch, uint32_t duty);

#endif /* __TIM15_H */

/************************ END OF FILE****/




