#include <stdio.h>
#include "contiki.h"
#include "dev/pwr_ctrl.h"
#include "dev/leds.h"

/*---------------------------------------------------------------------------*/
PROCESS(test_pwrctrl_process, "Test AVDD/IO power control.");
AUTOSTART_PROCESSES(&test_pwrctrl_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_pwrctrl_process, ev, data)
{
  static struct etimer et;
  unsigned char st = 0;

  PROCESS_BEGIN();

  leds_on(LEDS_GREEN);
  leds_off(LEDS_YELLOW);

  pwr_ctrl_init();

  while(1) {
    etimer_set(&et, CLOCK_SECOND);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    leds_toggle(LEDS_GREEN);
    leds_toggle(LEDS_YELLOW);

    pwr_ctrl(st);
    st = ~st;

  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
