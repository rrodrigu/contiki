#include <stdio.h>
#include "contiki.h"
#include "dev/leds.h"

/*---------------------------------------------------------------------------*/
PROCESS(test_leds_process, "Test onboard LEDs.");
AUTOSTART_PROCESSES(&test_leds_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_leds_process, ev, data)
{
  static struct etimer et;
  PROCESS_BEGIN();

  leds_on(LEDS_GREEN);
  leds_off(LEDS_YELLOW);

  while(1) {
    etimer_set(&et, CLOCK_SECOND/2);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    leds_toggle(LEDS_GREEN);
    leds_toggle(LEDS_YELLOW);

  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
