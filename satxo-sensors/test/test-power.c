#include "contiki.h"
#include "dev/leds.h"
#include "dev/battery-sensor.h"
#include <stdio.h>


float floor(float x)
{
  if(x >= 0.0f) {
    return (float) ((int) x);
  } else {
    return (float) ((int) x - 1);
  }
}

/*---------------------------------------------------------------------------*/
PROCESS(test_battery_process, "Battery and Photovoltaiv Sensor Test");
AUTOSTART_PROCESSES(&test_battery_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_battery_process, ev, data)
{
  static struct etimer et;
  float mv;
  uint16_t i;

  PROCESS_BEGIN();

  leds_on(LEDS_GREEN);

  while(1) {
    SENSORS_ACTIVATE(battery_sensor);

    etimer_set(&et, CLOCK_SECOND / 2);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    leds_off(LEDS_GREEN);

    i = battery_sensor.value(0);
    mv = (i * 1.600 * 3) / 1024;
    printf("Battery: %i (%ld.%03d V)\n", i, (long) mv,
	   (unsigned) ((mv - floor(mv)) * 1000));

    i = battery_sensor.value(1);
    mv = (i * 1.600 * 3) / 1024;
    printf("Photovoltaic: %i (%ld.%03d V)\n", i, (long) mv,
	   (unsigned) ((mv - floor(mv)) * 1000));

    SENSORS_DEACTIVATE(battery_sensor);

    etimer_set(&et, CLOCK_SECOND * 4 + CLOCK_SECOND / 2);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    leds_on(LEDS_GREEN);
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
