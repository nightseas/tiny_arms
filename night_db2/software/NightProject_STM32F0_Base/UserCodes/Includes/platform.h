/**
  ******************************************************************************
  * @file    appconfig.h
  * @author  Nightmare
  * @version V1.0.0
  * @date    2014.5.6
  * @brief   Project config file.
  ******************************************************************************
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_H
#define __PLATFORM_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void Platform_Config(void);
extern void GPIO_Config(void);
extern void SysTickTimer_Config(void);
extern void SysTickTimer_Stop(void);
extern void Sys_Decrease_US(void);
extern void Sys_Delay_US(__IO uint32_t time);
extern void Sys_Delay_MS(__IO uint32_t time);
extern void Sys_LED_On(int num);
extern void Sys_LED_Off(int num);
#endif /* __MAIN_H */

/************************ END OF FILE****/


