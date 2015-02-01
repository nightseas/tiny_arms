/**
  ******************************************************************************
  * @file    adc_temp.h
  * @author  Nightmare
  * @version V1.0.0
  * @date    2014.5.6
  * @brief   Project config file.
  ******************************************************************************
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_TEMP_H
#define __ADC_TEMP_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void ADC_Temp_Config(void);
extern uint16_t ADC_Temp_GetVolt(void);
extern int ADC_Temp_GetTemp(void);

#endif /* __ADC_TEMP_H */

/************************ END OF FILE****/




