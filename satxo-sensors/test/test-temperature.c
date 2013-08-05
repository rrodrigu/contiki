#include <stdio.h>
#include "contiki.h"
#include "dev/leds.h"
#include "dev/temperature-sensor.h"
#include "dev/pwr_ctrl.h"


/*---------------------------------------------------------------------------*/
PROCESS(test_temp_process, "Temperature Sensor Test");
AUTOSTART_PROCESSES(&test_temp_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_temp_process, ev, data)
{
  static struct etimer et;
  static int raw_temp;

  PROCESS_BEGIN();

  /* turn on sensors power */
  pwr_ctrl(1);
  SENSORS_ACTIVATE(temperature_sensor);

  while(1) {
    etimer_set(&et, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    leds_toggle(LEDS_GREEN);
    leds_toggle(LEDS_YELLOW);

    raw_temp = temperature_sensor.value(0);

    printf("Temperature = (%x) %d.%d Celcius\n",
        raw_temp, (raw_temp >> 4), (raw_temp & 0xf) * 625);

  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
