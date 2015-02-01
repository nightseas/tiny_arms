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
#define MAX_DAT_LEN		1500

#define HTONS(n) (uint16_t)((((uint16_t) (n)) << 8) | (((uint16_t) (n)) >> 8))
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t hostMac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
uint16_t hostIpaddr[2]={0x0A01, 0x0102};

uint8_t dstMac[6];
uint16_t dstIpaddr[2];

uint8_t isLinkedNow = 0, isLinkedBefore = 0;
uint8_t pktBuffer[MAX_DAT_LEN];
uint32_t pktLen = 0;

struct net_eth_hdr *eth_hdr;
struct net_arp_hdr *arp_hdr;
struct net_icmpip_hdr *icmp_hdr;

	
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


extern uint32_t UpDownFlag;

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

	//Init NetDev
	ENC28J60_Init(hostMac);		
	printf("NetDev init complete!\n");	
	
	//Init packet buffer of NIC
	arp_hdr = (struct net_arp_hdr *) malloc (sizeof(struct net_arp_hdr));
	icmp_hdr = (struct net_icmpip_hdr *) malloc (sizeof(struct net_icmpip_hdr));
	
	memset(pktBuffer, 0, sizeof(pktBuffer));
	for(int i=0; i<200; i++)
	{
		pktBuffer[i] = i;
	}
	
	//Init heart beat LED
	TIM1_PWM_Config(1000000);
	TIM1_PWM_SetDuty(1, 0);
	TIM1_PWM_SetDuty(2, 0);	
	TIM15_General_Config(1000);
	
	printf("=== ARP & ICMP Test Program ===\n  By Nights 2015-01-31\n");	
	
	while(1)
	{
		//Check Link
		{
			isLinkedNow = ENC28J60_GetLinkStatus();
			
			if(isLinkedBefore == ENC_LINK_DOWN && isLinkedNow == ENC_LINK_UP)
				printf("NetDev: link up!\n");
			else if(isLinkedBefore == ENC_LINK_UP && isLinkedNow == ENC_LINK_DOWN)
				printf("NetDev: link down!\n");	
			
			isLinkedBefore = isLinkedNow;
		}
		
		//ARP & ICMP responding.
		if(isLinkedNow == ENC_LINK_UP)
		{
			pktLen = ENC28J60_PacketReceive(pktBuffer, sizeof(pktBuffer));
			
			if(pktLen)
			{
//				// Dump the pkt.
//				printf("NetDev: received data:");
//				for(int i=0; i<pktLen; i++)
//				{
//					printf(" %02X", pktBuffer[i]);
//				}
//				printf("\n---\n");
				
				// Check if it's an ARP pkt.
				memcpy(arp_hdr, pktBuffer, sizeof(struct net_arp_hdr));				
				//arp_hdr = (struct net_arp_hdr *) &pktBuffer[0];
				if(arp_hdr->ethhdr.type == HTONS(UIP_ETHTYPE_ARP) && arp_hdr->opcode == HTONS(ARP_REQUEST))
				{
					printf("NetDev: received ARP request from %d.%d.%d.%d\n", 
						(arp_hdr->sipaddr[0] & 0xFF), (arp_hdr->sipaddr[0] >> 8), (arp_hdr->sipaddr[1] & 0xFF), (arp_hdr->sipaddr[0] >> 8));
					/* The reply opcode is 2. */
					arp_hdr->opcode = HTONS(2);

					memcpy(arp_hdr->dhwaddr.addr, arp_hdr->shwaddr.addr, 6);
					memcpy(arp_hdr->shwaddr.addr, hostMac, 6);
					memcpy(arp_hdr->ethhdr.src.addr, hostMac, 6);
					memcpy(arp_hdr->ethhdr.dest.addr, arp_hdr->dhwaddr.addr, 6);
					
					arp_hdr->dipaddr[0] = arp_hdr->sipaddr[0];
					arp_hdr->dipaddr[1] = arp_hdr->sipaddr[1];
					arp_hdr->sipaddr[0] = HTONS(hostIpaddr[0]);
					arp_hdr->sipaddr[1] = HTONS(hostIpaddr[1]);

					arp_hdr->opcode = HTONS(ARP_REPLY);
					
					memcpy(pktBuffer, arp_hdr, sizeof(struct net_arp_hdr));
					printf("NetDev: sending ARP reply...");
					ENC28J60_PacketSend(pktBuffer, sizeof(struct net_arp_hdr));
					printf("done!\n");
				}
				// Check if it's an ICMP pkt.
				else if(arp_hdr->ethhdr.type == HTONS(UIP_ETHTYPE_IP))
				{
					memcpy(icmp_hdr, pktBuffer, sizeof(struct net_icmpip_hdr));
					if(icmp_hdr->proto == UIP_PROTO_ICMP && icmp_hdr->type == ICMP_ECHO)
					{
						printf("NetDev: received ICMP request from %d.%d.%d.%d\n", 
						(icmp_hdr->srcipaddr[0] & 0xFF), (icmp_hdr->srcipaddr[0] >> 8), (icmp_hdr->srcipaddr[1] & 0xFF), (icmp_hdr->srcipaddr[0] >> 8));
						// Dill with MAC part
						memcpy(icmp_hdr->ethhdr.dest.addr, icmp_hdr->ethhdr.src.addr, 6);		
						memcpy(icmp_hdr->ethhdr.src.addr, hostMac, 6);						
						
						// Dill with IP&ICMP part
						memcpy(icmp_hdr->destipaddr, icmp_hdr->srcipaddr, sizeof(icmp_hdr->srcipaddr));
						icmp_hdr->srcipaddr[0] = HTONS(hostIpaddr[0]);
						icmp_hdr->srcipaddr[1] = HTONS(hostIpaddr[1]);
						icmp_hdr->type = ICMP_ECHO_REPLY;
						if(icmp_hdr->icmpchksum >= HTONS(0xffff - (ICMP_ECHO << 8)))
						{
							icmp_hdr->icmpchksum += HTONS(ICMP_ECHO << 8) + 1;
						}
						else
						{
							icmp_hdr->icmpchksum += HTONS(ICMP_ECHO << 8);
						}	
						memcpy(pktBuffer, icmp_hdr, sizeof(struct net_icmpip_hdr));
						
						printf("NetDev: sending ICMP reply...");
						ENC28J60_PacketSend(pktBuffer, sizeof(struct net_icmpip_hdr) + UIP_LLH_LEN);
						printf("done!\n");
					}
					else
						printf("NetDev: Unknown IP pkt.\n");
				}
				else
					printf("NetDev: Unknown type of pkt.\n");
				
			}
		}
	}
	
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
