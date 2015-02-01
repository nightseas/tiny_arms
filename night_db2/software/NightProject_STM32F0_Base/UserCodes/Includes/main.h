/**
	******************************************************************************
	* @file		GPIO/GPIO_IOToggle/main.h 
	* @author	MCD Application Team
	* @version V1.3.0
	* @date		16-January-2014
	* @brief	 Header for main.c module
	******************************************************************************
	* @attention
	*
	* <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
	*
	* Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
	* You may not use this file except in compliance with the License.
	* You may obtain a copy of the License at:
	*
	*				http://www.st.com/software_license_agreement_liberty_v2
	*
	* Unless required by applicable law or agreed to in writing, software 
	* distributed under the License is distributed on an "AS IS" BASIS, 
	* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	* See the License for the specific language governing permissions and
	* limitations under the License.
	*
	******************************************************************************
	*/
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"

/* Exported types ------------------------------------------------------------*/
struct net_eth_addr {
  uint8_t addr[6];
};

struct net_eth_hdr {
  struct net_eth_addr dest;
  struct net_eth_addr src;
  uint16_t type;
};

struct net_arp_hdr {
  struct net_eth_hdr ethhdr;
  uint16_t hwtype;
  uint16_t protocol;
  uint8_t hwlen;
  uint8_t protolen;
  uint16_t opcode;
  struct net_eth_addr shwaddr;
  uint16_t sipaddr[2];
  struct net_eth_addr dhwaddr;
  uint16_t dipaddr[2];
};

/* The ICMP and IP headers. */
struct net_icmpip_hdr {
	/* IPv4 header. */
	struct net_eth_hdr ethhdr;
	uint8_t vhl,
		tos,
		len[2],
		ipid[2],
		ipoffset[2],
		ttl,
		proto;
	uint16_t ipchksum;
	uint16_t srcipaddr[2],
		destipaddr[2];
	
	/* ICMP (echo) header. */
	uint8_t type, icode;
	uint16_t icmpchksum;
	uint8_t flags, reserved1, reserved2, reserved3;
	uint8_t icmp6data[16];
	uint8_t options[1];
};


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define UIP_ETHTYPE_ARP 0x0806
#define UIP_ETHTYPE_IP  0x0800
#define UIP_ETHTYPE_IP6 0x86dd

#define ARP_REQUEST 1
#define ARP_REPLY   2

#define ARP_HWTYPE_ETH 1

#define ICMP_ECHO_REPLY 0
#define ICMP_ECHO       8

#define UIP_LLH_LEN     14

#define UIP_PROTO_ICMP  1
#define UIP_PROTO_TCP   6
#define UIP_PROTO_UDP   17
#define UIP_PROTO_ICMP6 58
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
