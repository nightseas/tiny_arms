/**
  ******************************************************************************
  * @file    hello_uip.h
  * @author  Nightmare
  * @version V1.0.0
  * @date    2014.5.6
  * @brief   Head file modified from hello-world.h.
  ******************************************************************************
  *
  ******************************************************************************
  */

#ifndef __HELLO_UIP_H__
#define __HELLO_UIP_H__

/* Since this file will be included by uip.h, we cannot include uip.h
   here. But we might need to include uipopt.h if we need the u8_t and
   u16_t datatypes. */
#include "uipopt.h"

/* Next, we define the uip_tcp_appstate_t datatype. This is the state
   of our application, and the memory required for this state is
   allocated together with each TCP connection. One application state
   for each TCP connection. */
typedef struct hello_uip_state {
  //struct psock p;
  char inputbuffer[10];
  char name[40];
} uip_tcp_appstate_t;

/* Finally we define the application function to be called by uIP. */
void hello_uip_appcall(void);
#ifndef UIP_APPCALL
#define UIP_APPCALL hello_uip_appcall
#endif /* UIP_APPCALL */

void hello_uip_init(void);

#endif /* __HELLO_WORLD_H__ */
