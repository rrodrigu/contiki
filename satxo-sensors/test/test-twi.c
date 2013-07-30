#include <stdio.h>
#include "contiki.h"
#include "dev/leds.h"
#include "dev/twi.h"
#include "dev/pwr_ctrl.h"


/*---------------------------------------------------------------------------*/
PROCESS(test_twi_process, "TWI interface Test");
AUTOSTART_PROCESSES(&test_twi_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_twi_process, ev, data)
{
  static struct etimer et;
  static struct twi_msg msg;
  static uint8_t buf[5];
  int8_t ret;

  PROCESS_BEGIN();

  /* set 12 bit mode */
  msg.addr = 0x90;
  msg.flags = 0;
  msg.len = 2;
  msg.buf = buf;
  buf[0] = 0x01;
  buf[1] = 0x60;
  ret = twi_transfer(&msg);
  if (ret < 0)
    printf("ret=%d\n", ret);

  /* set pointer reg to temperature register */
  msg.addr = 0x90;
  msg.flags = 0;
  msg.len = 1;
  msg.buf = buf;
  buf[0] = 0x00;
  ret = twi_transfer(&msg);
  if (ret < 0)
    printf("ret=%d\n", ret);

  /* set read mode */
  msg.addr = 0x90;
  msg.flags = TWI_M_RD;
  msg.len = 2;
  msg.buf = buf;

  leds_on(LEDS_GREEN);
  leds_off(LEDS_YELLOW);

  pwr_ctrl_init();
  twi_init(TWI_CLK100);

  /* turn on sensors power */
  pwr_ctrl(1);

  while(1) {
    etimer_set(&et, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    leds_toggle(LEDS_GREEN);
    leds_toggle(LEDS_YELLOW);

    ret = twi_transfer(&msg);
    printf("ret=%d, rx buf = 0x%2x 0x%2x\n", ret, buf[0], buf[1]);
    printf("temp=%d.%04d Celcius\n", buf[0], (buf[1] >> 4) * 625);
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
