/**
  ******************************************************************************
  * @file    usart1.h
  * @author  Nightmare
  * @version V1.0.0
  * @date    2014.5.6
  * @brief   Project config file.
  ******************************************************************************
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART1_H
#define __USART1_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void USART_Config(void);

void Uart_PutChar(char data);
void Uart_Putch(char data);
void Uart_SendString(char *pt);
void Uart_Printf(const char *fmt,...);

char Uart_GetChar(void);
char Uart_GetKey(void);
#endif /* __MAIN_H */

/************************ END OF FILE****/




