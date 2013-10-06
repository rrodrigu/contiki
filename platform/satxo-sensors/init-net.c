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
#include "dev/slip.h"
#include "dev/serial_nr.h"

#if WITH_UIP6
#include "net/uip-ds6.h"
#endif /* WITH_UIP6 */

#if WITH_UIP
#include "net/uip.h"
#include "net/uip-fw.h"
#include "net/uip-fw-drv.h"
#include "net/uip-over-mesh.h"
static struct uip_fw_netif slipif =
  {UIP_FW_NETIF(0,0,0,0, 0,0,0,0, slip_send)};
static struct uip_fw_netif meshif =
  {UIP_FW_NETIF(172,16,0,0, 255,255,0,0, uip_over_mesh_send)};
#endif /* WITH_UIP */

#define DEBUG 1
#if DEBUG
#define PRINTF(FORMAT,args...) printf_P(PSTR(FORMAT),##args)
#else
#define PRINTF(...)
#endif

#define UIP_OVER_MESH_CHANNEL 8
#if WITH_UIP
static uint8_t is_gateway;
#endif /* WITH_UIP */

/* RIC
uint8_t mac_address[8] EEMEM = {0x02, 0x11, 0x22, 0xff, 0xfe, 0x33, 0x44, 0x05};

static void
get_mac_from_eeprom(uint8_t* macptr)
{
  eeprom_read_block ((void *)macptr,  &mac_address, 8);
}
*/

/*--------------------------------------------------------------------------*/
static void
set_rime_addr(void)
{
  rimeaddr_t addr;
  int i, snr_ret;

  snr_ret = get_serial_nr();

  if(snr_ret == 0){
    printf("error reading serial number");
  } else {
    /* XXX hack: Fix it so that the 802.15.4 MAC address is compatible
    with an Ethernet MAC address - byte 0 (byte 2 in the DS ID)
    cannot be odd. */
    serial_nr[2] &= 0xfe;
    printf("MAC 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x\n", 
      serial_nr[0], serial_nr[1], serial_nr[2], serial_nr[3], 
      serial_nr[4], serial_nr[5], serial_nr[6], serial_nr[7]);
  } 

  memset(&addr, 0, sizeof(rimeaddr_t));
#if UIP_CONF_IPV6
  memcpy(addr.u8, serial_nr, sizeof(addr.u8));
#else
  for(i = 0; i < sizeof(rimeaddr_t); ++i) {
    addr.u8[i] = serial_nr[6 - i];
  }
#endif
  rimeaddr_set_node_addr(&addr);
  printf("Rime started with address ");
  for(i = 0; i < sizeof(addr.u8) - 1; i++) {
    printf("%d.", addr.u8[i]);
  }
  printf("%d\n", addr.u8[i]);
}
#if WITH_UIP
/*--------------------------------------------------------------------------*/
static void
set_gateway(void)
{
  if(!is_gateway) {
    printf("making myself the IP network gateway.\n\n");
    printf("IPv4 address of the gateway: %d.%d.%d.%d\n\n",
	   uip_ipaddr_to_quad(&uip_hostaddr));
    uip_over_mesh_set_gateway(&rimeaddr_node_addr);
    uip_over_mesh_make_announced_gateway();
    is_gateway = 1;
  }
}
#endif /* WITH_UIP */
/*---------------------------------------------------------------------------*/
void
init_net(uint8_t node_id)
{
  /*uint16_t shortaddr;
  uint64_t longaddr;
  rimeaddr_t addr;*/
#if WITH_UIP6
  uip_ds6_addr_t *lladdr;
  uip_ipaddr_t ipaddr;
#endif
  
  uint8_t i;

#if WITH_UIP
  slip_arch_init(0);
#endif /* WITH_UIP */

  //memset(&addr, 0, sizeof(rimeaddr_t));
  //get_mac_from_eeprom(addr.u8);
  set_rime_addr(); 

  queuebuf_init();
  
  /* Start radio and radio receive process */
  NETSTACK_RADIO.init();

  rf230_set_channel(RF_CHANNEL);
  //rf230_set_pan_addr(IEEE802154_PANID, 0, (uint8_t *)&addr.u8);
  rf230_set_pan_addr(IEEE802154_PANID, 0, (uint8_t *)&serial_nr);
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
 
  //memcpy(&uip_lladdr.addr, &addr.u8, sizeof(rimeaddr_t));
  memcpy(&uip_lladdr.addr, &serial_nr, sizeof(rimeaddr_t));
  process_start(&tcpip_process, NULL);

  lladdr = uip_ds6_get_link_local(-1);
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

#if WITH_UIP

  PRINTF("IPV4\n");

  slip_set_input_callback(set_gateway);

  /* RIC */
  process_start(&tcpip_process, NULL);
  process_start(&uip_fw_process, NULL);	/* Start IP output */
  process_start(&slip_process, NULL);
  {
    uip_ipaddr_t hostaddr, netmask;

    uip_init();

    uip_ipaddr(&hostaddr, 172,16,
               rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[1]);
    uip_ipaddr(&netmask, 255,255,0,0);
    uip_ipaddr_copy(&meshif.ipaddr, &hostaddr);

    uip_sethostaddr(&hostaddr);
    uip_setnetmask(&netmask);
    uip_over_mesh_set_net(&hostaddr, &netmask);
    /*    uip_fw_register(&slipif);*/
    uip_over_mesh_set_gateway_netif(&slipif);
    uip_fw_default(&meshif);
    uip_over_mesh_init(UIP_OVER_MESH_CHANNEL);
    printf("uIP started with IP address %d.%d.%d.%d\n",
	   uip_ipaddr_to_quad(&hostaddr));
  }
 /* RIC */
#endif /* WITH_UIP */ 

}
