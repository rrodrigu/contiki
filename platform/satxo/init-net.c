#include <contiki.h>
#include <contiki-net.h>
#include <net/netstack.h>
#include <net/queuebuf.h>
#include <net/mac/frame802154.h>

#include "radio/rf230bb/rf230bb.h"

#include <stdio.h>
#include <string.h>

#include <avr/eeprom.h>

#include "contiki-lib.h"
#include "dev/rs232.h"

#define DEBUG 1
#if DEBUG
#define PRINTF(FORMAT,args...) printf_P(PSTR(FORMAT),##args)
#else
#define PRINTF(...)
#endif


uint8_t mac_address[8] EEMEM = {0x02, 0x11, 0x22, 0xff, 0xfe, 0x33, 0x44, 0x05};

static void
get_mac_from_eeprom(uint8_t* macptr)
{
  eeprom_read_block ((void *)macptr,  &mac_address, 8);
}


/*---------------------------------------------------------------------------*/
void
init_net(uint8_t node_id)
{
  /*uint16_t shortaddr;
  uint64_t longaddr;*/
  rimeaddr_t addr;
#if WITH_UIP6
  uip_ds6_addr_t *lladdr;
  uip_ipaddr_t ipaddr;
#endif
  
  uint8_t i;
  
  memset(&addr, 0, sizeof(rimeaddr_t));
  get_mac_from_eeprom(addr.u8);
  rimeaddr_set_node_addr(&addr);
  
  PRINTF("Rime started with address: ");
  for(i = 0; i < sizeof(addr.u8) - 1; ++i) {
    PRINTF("%d.", addr.u8[i]);
  }
  PRINTF("%d\n", addr.u8[i]);
  
  queuebuf_init();
  
  /* Start radio and radio receive process */
  NETSTACK_RADIO.init();

  rf230_set_channel(RF_CHANNEL);
  rf230_set_pan_addr(IEEE802154_PANID, 0, (uint8_t *)&addr.u8);
  /*rf230_set_txpower(0);*/
  
  /* Initialize stack protocols */
  NETSTACK_RDC.init();
  NETSTACK_MAC.init();
  NETSTACK_NETWORK.init();

  PRINTF("%s %s, channel %u , check rate %u Hz tx power %u\n",
	NETSTACK_MAC.name, NETSTACK_RDC.name,
	rf230_get_channel(),
	CLOCK_SECOND / (NETSTACK_RDC.channel_check_interval() == 0 ? 1:
		       NETSTACK_RDC.channel_check_interval()),
	rf230_get_txpower());	  
#if UIP_CONF_IPV6_RPL
  PRINTF("RPL Enabled\n");
#endif
#if UIP_CONF_ROUTER
  PRINTF("Routing Enabled\n");
#endif


  
#if UIP_CONF_IPV6

  PRINTF("IPV6\n");
  
  
 
/* Add addresses for testing */
#if 0
{  
  uip_ip6addr_t ipaddr;
  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
//  uip_ds6_prefix_add(&ipaddr,64,0);
}
#endif
 
  
  
  
  
  
  memcpy(&uip_lladdr.addr, &addr.u8, sizeof(rimeaddr_t));
  PRINTF("0\n");
  process_start(&tcpip_process, NULL);
  PRINTF("1\n");
  lladdr = uip_ds6_get_link_local(-1);
 PRINTF("2\n");
  PRINTF("Tentative link-local IPv6 address ");
  
  for(i = 0; i < 7; ++i) {
    PRINTF("%02x%02x:", lladdr->ipaddr.u8[i * 2], lladdr->ipaddr.u8[i * 2 + 1]);
  }
  
  PRINTF("%02x%02x\n", lladdr->ipaddr.u8[14], lladdr->ipaddr.u8[15]);
 
  
  
  if(!UIP_CONF_IPV6_RPL) {
    uip_ip6addr(&ipaddr, 0x2001, 0x1418, 0x100, 0x823c, 0, 0, 0, 0);
    uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
    uip_ds6_addr_add(&ipaddr, 0, ADDR_TENTATIVE);

    PRINTF("Tentative global IPv6 address ");

    for(i = 0; i < 7; ++i) {
      PRINTF("%02x%02x:", ipaddr.u8[i * 2], ipaddr.u8[i * 2 + 1]);
    }

    PRINTF("%02x%02x\n", ipaddr.u8[7 * 2], ipaddr.u8[7 * 2 + 1]);
  }
  
  
  
#endif

}
