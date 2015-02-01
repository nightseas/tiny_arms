/**
  ******************************************************************************
  * @file    usart.c
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
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void USART_Config(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	USART_InitTypeDef  USART_InitStructure;
	
	// USART1_TX -> PA9 
  // USART1_RX -> PA10
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 50M
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	

  /* USART1 configuration ----------------------------------------------------*/
  /* USART1 configured as follow:
  - BaudRate = 115200 baud  
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1,&USART_InitStructure);
		
  USART_Cmd(USART1,ENABLE);
}


void Uart_PutChar(char data)
{
  /* Loop until the end of transmission */
	USART_SendData(USART1, (char) data); 
  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);  
}

void Uart_Putch(char data)
{
	if(data=='\n')
	{
		Uart_PutChar('\r');
		Uart_PutChar('\n');
	}
	else
	{
		Uart_PutChar(data);
	}
}

void Uart_SendString(char *pt)
{
    while(*pt)
        Uart_Putch(*pt++);
}


void Uart_Printf(const char *fmt,...)
{
    va_list ap;
    char string[50];

    va_start(ap, fmt);
    vsprintf(string, fmt, ap);
    va_end(ap);
    Uart_SendString(string);
}


char Uart_GetChar(void)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE)==RESET);
	return(USART_ReceiveData(USART1));
}

char Uart_GetKey(void)
{
	if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==SET)
	{
		return(USART_ReceiveData(USART1));
	}
	return 0;
}

/*===================以下是串口终端设备接口函数库==========================*/

#define LINE_LENGTH 20          /* 行缓冲区大小，决定每行最多输入的字符数*/

/*标准终端设备中，特殊ASCII码定义，请勿修改*/
#define In_BACKSP 0x08          /* ASCII  <--  (退格键)  */
#define In_DELETE 0x7F          /* ASCII <DEL> (DEL 键)  */
#define In_EOL '\r'             /* ASCII <CR>  (回车键)  */
#define In_SKIP '\3'            /* ASCII control-C */
#define In_EOF '\x1A'           /* ASCII control-Z */

#define Out_DELETE "\x8 \x8"    /* VT100 backspace and clear */
#define Out_SKIP "^C\n"         /* ^C and new line */
#define Out_EOF "^Z"            /* ^Z and return EOF */

/************************************printf***************/
PUTCHAR_PROTOTYPE
{
    Uart_Putch((uint8_t) ch);
    return ch;
}

 /********************scanf*********************************/

GETCHAR_PROTOTYPE
{
  static char io_buffer[LINE_LENGTH + 2];     /* Where to put chars */
  static int ptr;                             /* Pointer in buffer */
  char c;

  for (;;)
  {
    if (io_buffer[ptr])
      return (io_buffer[ptr++]);
    ptr = 0;
    for (;;)
    {
      if ((c = Uart_GetChar()) == In_EOF && !ptr)
      {
        Uart_SendString(Out_EOF);
        return EOF;
      }
      if ((c == In_DELETE)||(c==In_BACKSP))
      {
        if (ptr)
        {
          ptr--;
          Uart_SendString(Out_DELETE);
        }
      }
      else if (c == In_SKIP)
      {
        Uart_SendString(Out_SKIP);
        ptr = LINE_LENGTH + 1;  /* Where there always is a zero... */
        break;
      }
      else if (c == In_EOL)
      {
        Uart_PutChar(io_buffer[ptr++] = '\n');
        io_buffer[ptr] = 0;
        ptr = 0;
        break;
      }
      else if (ptr < LINE_LENGTH)
      {
        if (c >= ' ')
        {
          Uart_PutChar(io_buffer[ptr++] = c);
        }
      }
      else
      {
        Uart_PutChar('\7');
      }
    }
  }
}

