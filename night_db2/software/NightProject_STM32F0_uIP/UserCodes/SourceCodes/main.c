/**
  ******************************************************************************
  * @file    main.c
  * @author  Nightmare
  * @version V1.0.0
  * @date    2015.2.1
  * @brief   
  ******************************************************************************
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "appconfig.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t hostMac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
uint16_t hostIpaddr[2]={0x0A01, 0x0102};	

uint8_t isLinkedNow = 0, isLinkedBefore = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void uip_main(void);

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{	
	//Init system and pheripherals
	Platform_Config();
	printf("System init complete!\n");	

	TIM15_General_Config(1000);	
	printf("TIM15 configured as uIP base clock!\n");	
		
	printf("=== uIP Test Program ===\n  By Nights 2015-01-31\n");	
	
	while(1)
	{
		//Check Link
		isLinkedNow = ENC28J60_GetLinkStatus();
		
		if(isLinkedBefore == ENC_LINK_DOWN && isLinkedNow == ENC_LINK_UP)
			printf("NetDev: link up!\n");
		else if(isLinkedBefore == ENC_LINK_UP && isLinkedNow == ENC_LINK_DOWN)
			printf("NetDev: link down!\n");	
		
		isLinkedBefore = isLinkedNow;

		if(isLinkedNow)
		{
			uip_main();
		}
	}	
}


/*---------------------------------------------------------------------------*/
void uip_main(void)
{
  int i;
  uip_ipaddr_t ipaddr;
  struct uip_eth_addr ethaddr;
  struct timer periodic_timer, arp_timer;

  timer_set(&periodic_timer, CLOCK_SECOND / 2);
  timer_set(&arp_timer, CLOCK_SECOND * 10);
  
  tapdev_init();
  uip_init();

  for(int i=0; i<6; i++)
  	ethaddr.addr[i] = hostMac[i];
  uip_setethaddr(ethaddr);
  
  uip_ipaddr(ipaddr, 10,1,1,2);
  uip_sethostaddr(ipaddr);
  uip_ipaddr(ipaddr, 10,1,1,1);
  uip_setdraddr(ipaddr);
  uip_ipaddr(ipaddr, 255,0,0,0);
  uip_setnetmask(ipaddr);

  //Init apps
  hello_uip_init();
  
  while(1) {
    uip_len = tapdev_read();
    if(uip_len > 0) {
      if(BUF->type == htons(UIP_ETHTYPE_IP)) {
	uip_arp_ipin();
	uip_input();
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  tapdev_send();
	}
      } else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
	uip_arp_arpin();
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  tapdev_send();
	}
      }

    } else if(timer_expired(&periodic_timer)) {
      timer_reset(&periodic_timer);
      for(i = 0; i < UIP_CONNS; i++) {
	uip_periodic(i);
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  tapdev_send();
	}
      }

#if UIP_UDP
      for(i = 0; i < UIP_UDP_CONNS; i++) {
	uip_udp_periodic(i);
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  tapdev_send();
	}
      }
#endif /* UIP_UDP */
      
      /* Call the ARP timer function every 10 seconds. */
      if(timer_expired(&arp_timer)) {
	timer_reset(&arp_timer);
	uip_arp_timer();
      }
    }
  }
}
/*---------------------------------------------------------------------------*/
void
uip_log(char *m)
{
  printf("uIP log message: %s\n", m);
}

/************************ (C) COPYRIGHT Nightmare *****END OF FILE****/
