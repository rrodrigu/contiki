#include <avr/io.h>
#include "contiki.h"
#include "dev/rain-sensor.h"

const struct sensors_sensor rain_sensor;
static uint8_t active;


static uint16_t rain_gauge;

clock_time_t last_timer, rain_clk_ticks;

ISR(INT5_vect)
{
  clock_time_t tmp_clk = clock_time();
  rain_clk_ticks = tmp_clk - last_timer;
  last_timer = tmp_clk;
}


/* rain gauge process */
PROCESS(rain_gauge_process, "Rain gauge process");
PROCESS_THREAD(rain_gauge_process, ev, data)
{
  static struct etimer et;

  PROCESS_BEGIN();

  while(1) {
    etimer_set(&et, CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    if ((clock_time() - last_timer) > 60 * CLOCK_SECOND) {
      /* assume no rain if inactive for 60 sec */
      rain_gauge = 0;
      rain_clk_ticks = 0;
      //last_timer = clock_time();
    } else
      /* in ticks / 60s */
      if (rain_clk_ticks > 0)
        rain_gauge = (CLOCK_SECOND * 60) / rain_clk_ticks;
      else
        rain_gauge = 0;
  }

  PROCESS_END();
}



static void activate(void)
{
  /* edge triggered: ISC5 = 0x2 (falling edge) */
  EICRB |=  _BV(ISC51);
  EICRB &= ~_BV(ISC50);

  /* prepare last_timer counter */
  last_timer = clock_time();

  /* enable external interrupt 4 */
  EIMSK |= _BV(INT5);

  /* start process */
  process_start(&rain_gauge_process, NULL);

  active = 1;
}

static void deactivate(void)
{
  /* disable int4 */
  EIMSK &= ~_BV(INT5);

  /* kill process */
  process_exit(&rain_gauge_process);

  active = 0;
}

static int value(int type)
{
  switch (type) {
  case 0:
    return rain_gauge;
  default:
    return 0;
  }
}

static int configure(int type, int c)
{
  switch(type) {
  case SENSORS_ACTIVE:
    if (c) {
      activate();
    } else {
      deactivate();
    }
  }
  return 0;
}

static int status(int type)
{
  switch (type) {
  case SENSORS_ACTIVE:
  case SENSORS_READY:
    return active;
  }
  return 0;
}

SENSORS_SENSOR(rain_sensor, RAIN_SENSOR,
	       value, configure, status);

