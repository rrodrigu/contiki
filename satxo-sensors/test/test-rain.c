#include <stdio.h>
#include "contiki.h"
#include "dev/leds.h"
#include "dev/rain-sensor.h"
#include "dev/pwr_ctrl.h"

float floor(float x)
{
  if(x >= 0.0f) {
    return (float) ((int) x);
  } else {
    return (float) ((int) x - 1);
  }
}

/*---------------------------------------------------------------------------*/
PROCESS(test_rain_process, "Rain Sensor Test");
AUTOSTART_PROCESSES(&test_rain_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_rain_process, ev, data)
{
  static struct etimer et;
  static int raw;
  static float rain;

  PROCESS_BEGIN();

  /* turn on sensors power */
  pwr_ctrl(1);
  SENSORS_ACTIVATE(rain_sensor);

  while(1) {
    etimer_set(&et, CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    leds_toggle(LEDS_GREEN);
    leds_toggle(LEDS_YELLOW);

    raw = rain_sensor.value(0);
    printf("raw rain gauge = %d\n", raw);
    rain = (raw * 0.2794);
    printf("rain gauge = %d.%02d mm/min\n",
        (int) rain, (unsigned) ((rain - floor(rain)) * 100));
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
