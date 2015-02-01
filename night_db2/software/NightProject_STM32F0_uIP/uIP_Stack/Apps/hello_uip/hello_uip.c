/**
  ******************************************************************************
  * @file    hello_uip.c
  * @author  Nightmare
  * @version V1.0.0
  * @date    2015.2.1
  * @brief   This is a samplest illustration of setting up TCP server of uIP.
  ******************************************************************************
  *
  ******************************************************************************
  */

#include "appconfig.h"

int FlagConnected = 0;

void hello_uip_init(void)
{
	uip_listen(HTONS(1234));
	printf("Hello_uIP: hello_uip init complete!\n");
}


void hello_uip_appcall(void)
{
	if(uip_newdata())
	{
		printf("Hello_uIP: Got new data: %s\n", uip_buf);
		uip_send("ok\n", 3);
	}
	if(uip_rexmit())
	{
		printf("Hello_uIP: Last reply missing, retransmit reply.\n");
		uip_send("ok\n", 3);
	}
}
