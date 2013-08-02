#include <stdio.h>
#include "contiki.h"
#include "dev/leds.h"
#include "dev/adc.h"
#include "dev/uv-sensor.h"
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
PROCESS(test_uv_process, "UV Sensor Test");
AUTOSTART_PROCESSES(&test_uv_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_uv_process, ev, data)
{
  static struct etimer et;
  static int raw;

  PROCESS_BEGIN();

  /* turn on sensors power */
  pwr_ctrl(1);
  SENSORS_ACTIVATE(uv_sensor);

  /* enable adc */
  open_adc(ADC_PS_64, ADC_REF_INT16);

  while(1) {
    etimer_set(&et, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    leds_toggle(LEDS_GREEN);
    leds_toggle(LEDS_YELLOW);

    raw = uv_sensor.value(0);

    printf("raw UV reading = %d\n", raw);

  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
