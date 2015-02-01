/**
  ******************************************************************************
  * @file    adc.c
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
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void ADC_Temp_Config(void)
{
  ADC_InitTypeDef     ADC_InitStructure;
  
  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  /* ADCs DeInit */  
  ADC_DeInit(ADC1);
  
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);
  
  /* Configure the ADC1 in continuous mode with a resolution equal to 12 bits  */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC_InitStructure); 
  
  /* Convert the ADC1 Channel 0 with 239.5 Cycles as sampling time */ 
  ADC_ChannelConfig(ADC1, ADC_Channel_TempSensor , ADC_SampleTime_239_5Cycles);

  /* ADC Calibration */
  ADC_GetCalibrationFactor(ADC1);
  
  /* Enable the ADC peripheral */
  ADC_Cmd(ADC1, ENABLE);     
  
  /* Wait the ADRDY flag */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 
  
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ADC1);  
}

uint16_t ADC_Temp_GetVolt(void)
{
	uint16_t ADC1ConvertedValue;
	/* Test EOC flag */
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

	/* Get ADC1 converted data */
	ADC1ConvertedValue = ADC_GetConversionValue(ADC1);

	/* Compute the voltage */
	return (ADC1ConvertedValue *3300)/0xFFF;
}

int ADC_Temp_GetTemp(void)
{
	uint16_t ADC1ConvertedValue;
	uint16_t* TempCalibration30 = (uint16_t*)0x1FFFF7B8;
	uint16_t* TempCalibration110 = (uint16_t*)0x1FFFF7C2;
	
	printf("TempCalibration30 = 0x%04x, TempCalibration110 = 0x%04x\n", *TempCalibration30, *TempCalibration110);
	
	return 0;
//	/* Test EOC flag */
//	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

//	/* Get ADC1 converted data */
//	ADC1ConvertedValue = ADC_GetConversionValue(ADC1);

//	/* Compute the voltage */
//	return (ADC1ConvertedValue *3300)/0xFFF;
}
