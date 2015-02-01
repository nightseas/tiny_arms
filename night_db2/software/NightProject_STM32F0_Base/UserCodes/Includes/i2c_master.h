/**
  ******************************************************************************
  * @file    i2c_master.h
  * @author  Nightmare
  * @version V1.0.0
  * @date    2014.5.6
  * @brief   Project config file.
  ******************************************************************************
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2C2_H
#define __I2C2_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
/* Exported types ------------------------------------------------------------*/
enum I2C_STAT
{
	I2C_SUCCESS = 0,
	I2C_ADDR_ERR = 1,
	I2C_DATA_ERR = 2,
	I2C_BUS_ERR = 3,
	I2C_STOP_ERR = 4,
};
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void I2C2_Config(void);
extern int I2C2_WriteCmd(uint8_t DevAddr, uint8_t CmdData);
extern int I2C2_WriteByte(uint8_t DevAddr, uint8_t RegAddr, uint8_t RegData);
extern int I2C2_BufferWrite(uint8_t DevAddr, uint8_t RegAddr, uint8_t* DataBuf, uint16_t DataLen);
extern int I2C2_RandomRead(uint8_t DevAddr, uint8_t RegAddr, uint8_t* DataBuf, uint16_t DataLen);
extern int I2C2_CurrentRead(uint8_t DevAddr, uint8_t* DataBuf,  uint16_t DataLen);
extern int I2C2_GetStatus(uint8_t DevAddr);
#endif /* __MAIN_H */

/************************ END OF FILE****/




