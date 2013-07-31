#include <stdio.h>
#include "contiki.h"
#include "dev/leds.h"
#include "dev/humidity-sensor.h"
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
PROCESS(test_hih_process, "Humidity Sensor Test");
AUTOSTART_PROCESSES(&test_hih_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_hih_process, ev, data)
{
  static struct etimer et;
  static int raw;
  static float hih;

  PROCESS_BEGIN();

  /* turn on sensors power */
  pwr_ctrl(1);
  SENSORS_ACTIVATE(humidity_sensor);

  while(1) {
    etimer_set(&et, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    leds_toggle(LEDS_GREEN);
    leds_toggle(LEDS_YELLOW);

    raw = humidity_sensor.value(0);

    /* Vout = Vsup * (0.00636 * (sensor RH) + 0.1515) */

#define GAIN 3.000
#define VREF 1.600
#define VSUP 3.300
#define K1   0.00636
#define K2   0.1515

    hih = ((raw * VREF * GAIN) / (1024 * VSUP) - K2) / K1;

    /* true RH = (sensor RH) / (1.0546 - 0.00216 * temp) */

    printf("Uncompensated humidity = %d.%02d %%\n",
        (int) hih, (unsigned) ((hih - floor(hih)) * 100));

  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
