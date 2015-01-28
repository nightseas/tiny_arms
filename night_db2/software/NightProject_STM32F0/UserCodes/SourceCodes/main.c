/**
  ******************************************************************************
  * @file    main.c
  * @author  Nightmare
  * @version V1.0.0
  * @date    2014.5.6
  * @brief   
  ******************************************************************************
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "appconfig.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define RW_SIZE				0xFF
#define MAX_DAT_LEN		10

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
unsigned char ReadBuffer[0xFF]={0};
unsigned char WriteBuffer[0xFF]={0};
int CmdNum;
int DevAddr, RegAddr, DataLen;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void blink_led(void)
{
	for(int i=0;i<3;i++)
	{
		Sys_LED_On(1);
		Sys_Delay_MS(200);
		Sys_LED_Off(1);
		Sys_Delay_MS(200);
	}
}


void Cmd_MainMenu(void)
{
	int i, ret;
	int TempData;
	printf("\n===== Night Debuger (I2C Mode) =====\n");
	printf("1.Write Cmd\n");
	printf("2.Write Data\n");
	printf("3.Random Read Data\n");
	printf("4.Current Read Data\n");
	printf("5.Scan I2C Bus\n");
	printf("Input>");
	scanf("%d", &CmdNum);
	printf("\n");
	switch(CmdNum)
	{
		case 1:
			printf("\nDev Addr>");
			scanf("%x", &DevAddr);
			printf("\nCmd Data>");
			scanf("%x", &RegAddr);
			ret = I2C2_WriteCmd((uint8_t)(DevAddr<<1), (uint8_t)RegAddr);
			if(ret != I2C_SUCCESS)
			{
				printf("\nOperation failed, error no %d.\n", ret);
				break;
			}
		break;
			
		case 2:
			printf("\nDev Addr>");
			scanf("%x", &DevAddr);
			printf("\nReg Addr>");
			scanf("%x", &RegAddr);
			printf("\nData Num(1-8)>");
			scanf("%d", &DataLen);
			if(DataLen>0 && DataLen<=MAX_DAT_LEN)
			{
				for(i=0;i<DataLen;i++)
				{
					printf("\nData[%d]>", i+1);
					scanf("%x", &TempData);
					WriteBuffer[i] = (uint8_t)TempData;
				}
				ret = I2C2_WriteBuffer((uint8_t)(DevAddr<<1), (uint8_t)RegAddr, WriteBuffer, (uint16_t)DataLen);		
				if(ret != I2C_SUCCESS)
				{
					printf("\nOperation failed, error no %d.\n", ret);
					break;
				}
			}
			else
			{
				printf("\nErr: Wrong Num!\n");
			}			
		break;
			
		case 3:
			printf("\nDev Addr>");
			scanf("%x", &DevAddr);
			printf("\nReg Addr>");
			scanf("%x", &RegAddr);
			printf("\nData Num(1-8)>");
			scanf("%d", &DataLen);
			if(DataLen>0 && DataLen<=MAX_DAT_LEN)
			{
				ret = I2C2_RandomRead((uint8_t)(DevAddr<<1), (uint8_t)RegAddr, ReadBuffer, (uint16_t)DataLen);		
				if(ret != I2C_SUCCESS)
				{
					printf("\nOperation failed, error no %d.\n", ret);
					break;
				}
				for(i=0;i<DataLen;i++)
				{
					printf("\nData[%d]=0x%02x", i+1, (int)ReadBuffer[i]);
				}				
			}
			else
			{
				printf("\nErr: Wrong Num!\n");
			}			
		break;
		
		case 4:
			printf("\nDev Addr>");
			scanf("%x", &DevAddr);
			printf("\nData Num(1-8)>");
			scanf("%d", &DataLen);
			if(DataLen>0 && DataLen<=MAX_DAT_LEN)
			{
				ret = I2C2_CurrentRead((uint8_t)(DevAddr<<1), ReadBuffer, (uint16_t)DataLen);		
				if(ret != I2C_SUCCESS)
				{
					printf("\nOperation failed, error no %d.\n", ret);
					break;
				}
				for(i=0;i<DataLen;i++)
				{
					printf("\nData[%d]=0x%02x", i+1, (int)ReadBuffer[i]);
				}				
			}
			else
			{
				printf("\nErr: Wrong Num!\n");
			}			
		break;
			
		case 5:
			printf("\nScaning I2C bus...\n");
			for(i=0;i<=0x7F;i++)		
			{
				if(I2C2_GetStatus(i<<1) == I2C_SUCCESS)
					printf("Find dev 0x%02x!\n", i);
			}
		break;	
			
		default:
			printf("\nIncorrect Number!\n\n");
		break;
	}
	printf("\nPress any key...\n");
	Uart_GetChar();
}

unsigned char tmp;

void crc_test(void)
{
	uint32_t crc_src32[]={
		0x00001021, 0x20423063, 0x408450a5, 0x60c670e7, 0x9129a14a, 0xb16bc18c,
    0xd1ade1ce, 0xf1ef1231, 0x32732252, 0x52b54294, 0x72f762d6, 0x93398318,
    0xa35ad3bd, 0xc39cf3ff, 0xe3de2462, 0x34430420, 0x64e674c7, 0x44a45485,
    0xa56ab54b, 0x85289509, 0xf5cfc5ac, 0xd58d3653, 0x26721611, 0x063076d7,
    0x569546b4, 0xb75ba77a, 0x97198738, 0xf7dfe7fe, 0xc7bc48c4, 0x58e56886,
    0x78a70840, 0x18612802, 0xc9ccd9ed, 0xe98ef9af, 0x89489969, 0xa90ab92b,
    0x4ad47ab7, 0x6a961a71, 0x0a503a33, 0x2a12dbfd, 0xfbbfeb9e, 0x9b798b58,
    0xbb3bab1a, 0x6ca67c87, 0x5cc52c22, 0x3c030c60, 0x1c41edae, 0xfd8fcdec,
    0xad2abd0b, 0x8d689d49, 0x7e976eb6, 0x5ed54ef4, 0x2e321e51, 0x0e70ff9f,
    0xefbedfdd, 0xcffcbf1b, 0x9f598f78, 0x918881a9, 0xb1caa1eb, 0xd10cc12d,
    0xe16f1080, 0x00a130c2, 0x20e35004, 0x40257046, 0x83b99398, 0xa3fbb3da,
    0xc33dd31c, 0xe37ff35e, 0x129022f3, 0x32d24235, 0x52146277, 0x7256b5ea,
    0x95a88589, 0xf56ee54f, 0xd52cc50d, 0x34e224c3, 0x04817466, 0x64475424,
    0x4405a7db, 0xb7fa8799, 0xe75ff77e, 0xc71dd73c, 0x26d336f2, 0x069116b0,
    0x76764615, 0x5634d94c, 0xc96df90e, 0xe92f99c8, 0xb98aa9ab, 0x58444865,
    0x78066827, 0x18c008e1, 0x28a3cb7d, 0xdb5ceb3f, 0xfb1e8bf9, 0x9bd8abbb,
    0x4a755a54, 0x6a377a16, 0x0af11ad0, 0x2ab33a92, 0xed0fdd6c, 0xcd4dbdaa,
    0xad8b9de8, 0x8dc97c26, 0x5c644c45, 0x3ca22c83, 0x1ce00cc1, 0xef1fff3e,
    0xdf7caf9b, 0xbfba8fd9, 0x9ff86e17, 0x7e364e55, 0x2e933eb2, 0x0ed11ef0
	};
	printf("\nCRC Test...\n");
	
	CRC_Config_32Bit(0);
	Sys_Delay_MS(100);
	printf("CRC32=0x%08X\nExpect: 0xEBF5058C\n", CRC_Calc_32Bits(crc_src32, sizeof(crc_src32)/4));
	Sys_Delay_MS(100);
}

void TIM1_Test()
{
	printf("\nTIM1 PWM Test...\n");
	TIM1_PWM_Config(1000000);
	while(1)
	{
		for(int i=0;i<1000;i++)
		{
			TIM1_PWM_SetDuty(1, i);
			TIM1_PWM_SetDuty(2, 1000-i);
			Sys_Delay_MS(1);
		}	
		for(int i=1000;i>0;i--)
		{
			TIM1_PWM_SetDuty(1, i);
			TIM1_PWM_SetDuty(2, 1000-i);
			Sys_Delay_MS(1);
		}	
	}
}

void TIM_General_Test()
{
	printf("\nTIM General Test...\n");
	TIM15_General_Config(1000);
	TIM16_General_Config(1000);
	TIM17_General_Config(1000);
}

void TIM_PWM_Test()
{
	printf("\nTIM PWM Output Test...\n");
	TIM15_PWM_Config(1000000);
	TIM16_PWM_Config(50000);
	TIM17_PWM_Config(10000000);
	
	TIM16_PWM_SetDuty(1, 251);
	TIM17_PWM_SetDuty(1, 763);

	while(1)
	{
		for(int i=0;i<1000;i++)
		{
			TIM15_PWM_SetDuty(1, i);
			Sys_Delay_MS(1);
		}	
		for(int i=1000;i>0;i--)
		{
			TIM15_PWM_SetDuty(1, i);
			Sys_Delay_MS(1);
		}	
	}
}

unsigned char  mymac[6]={0x00,0x11,0x22,0x33,0x44,0x55};
uint8_t isLinkedNow = 0, isLinkedBefore = 0;

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{	
	Platform_Config();
	printf("System init complete!\n");	
	
	enc28j60Init(mymac);	
	printf("ENC28J60 version: 0x%02X\n", enc28j60getrev());
	
	enc28j60RegDump();

	TIM1_PWM_Config(1000000);
	TIM1_PWM_SetDuty(1, 0);
	TIM1_PWM_SetDuty(2, 0);
	
	TIM15_General_Config(1000);
	
	while(1)
	{
		if(enc28j60_mac_is_linked() == 0)
			isLinkedNow = 1;
		else
			isLinkedNow = 0;
		
		if(isLinkedBefore == 0 && isLinkedNow == 1)
			printf("ENC28J60 link up!\n");
		else if(isLinkedBefore == 1 && isLinkedNow == 0)
			printf("ENC28J60 link down!\n");
		
		isLinkedBefore = isLinkedNow;
		
		Sys_Delay_MS(100);
	}
	
//	while(1)
//		Cmd_MainMenu();
}





#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
