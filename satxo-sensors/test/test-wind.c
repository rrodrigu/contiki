#include <stdio.h>
#include "contiki.h"
#include "dev/leds.h"
#include "dev/wind-sensor.h"
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
PROCESS(test_wind_process, "Wind Sensor Test");
AUTOSTART_PROCESSES(&test_wind_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_wind_process, ev, data)
{
  static struct etimer et;
  static int raw;
  static float wspeed;

  PROCESS_BEGIN();

  /* turn on sensors power */
  pwr_ctrl(1);
  SENSORS_ACTIVATE(wind_sensor);

  while(1) {
    etimer_set(&et, CLOCK_SECOND / 2);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    leds_toggle(LEDS_GREEN);
    leds_toggle(LEDS_YELLOW);

    raw = wind_sensor.value(0);
    printf("Wind direction = %d\n", raw);

    raw = wind_sensor.value(1);
    printf("raw wind speed = %d\n", raw);
    wspeed = (raw * 2.4) / 10.0;
    printf("wind speed = %d.%02d km/h\n",
        (int) wspeed, (unsigned) ((wspeed - floor(wspeed)) * 100));
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
