#include <avr/io.h>
#include "contiki.h"
#include "dev/wind-sensor.h"
#include "dev/adc.h"

#define WINDDIR_ADC_CHAN	(ADC_CHAN_ADC2)
#define ADC_REF			(ADC_REF_INT16)

const struct sensors_sensor wind_sensor;
static uint8_t active;

/* wind direction lookup table */
static struct wind_vane {
  uint16_t min;
  uint16_t max;
  uint8_t dir;
} wind_vane_lut[] = {
  {270, 280, 0}, /*   0.0 degree */
  { 60,  62, 1}, /*  22.5 degree */
  { 74,  77, 2}, /*  45.0 degree */
  { 7,    8, 3}, /*  67.5 degree */
  { 9,   10, 4}, /*  90.0 degree */
  { 5,    6, 5}, /* 112.5 degree */
  { 19,  21, 6}, /* 135.0 degree */
  { 12,  15, 7}, /* 157.5 degree */
  { 33,  40, 8}, /* 180.0 degree */
  { 27,  30, 9}, /* 202.5 degree */
  {140, 150, 10}, /* 225.0 degree */
  {125, 130, 11}, /* 247.5 degree */
  {740, 760, 12}, /* 270.0 degree */
  {340, 350, 13}, /* 292.5 degree */
  {480, 500, 14}, /* 315.0 degree */
  {180, 200, 15}, /* 337.5 degree */
};

static uint8_t wind_speed;

clock_time_t last_timer, speed_clk_ticks;

ISR(INT4_vect)
{
  clock_time_t tmp_clk = clock_time();
  speed_clk_ticks = tmp_clk - last_timer;
  last_timer = tmp_clk;
}


/* wind speed process */
PROCESS(wind_speed_process, "Wind speed process");
PROCESS_THREAD(wind_speed_process, ev, data)
{
  static struct etimer et;

  PROCESS_BEGIN();

  while(1) {
    /* measure the number of ticks in a period of 10 sec */
    /* etimer_set(&et, CLOCK_SECOND * 10); */
    etimer_set(&et, CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    if ((clock_time() - last_timer) > 5 * CLOCK_SECOND)
      /* assume no wind if there is no rotation within 5 sec of last tick */
      wind_speed = 0;
    else
      /* in ticks / 10s */
      wind_speed = (CLOCK_SECOND * 10) / speed_clk_ticks;
  }

  PROCESS_END();
}






static void activate(void)
{
  /* wind direction init */
  /* open_adc(ADC_PS_64, ADC_REF); */


  /* wind speed init */
  /* edge triggered: ISC4 = 0x2 (falling edge) */
  EICRB |=  _BV(ISC41);
  EICRB &= ~_BV(ISC40);

  /* prepare last_timer counter */
  last_timer = clock_time();

  /* enable external interrupt 4 */
  EIMSK |= _BV(INT4);

  /* start process */
  process_start(&wind_speed_process, NULL);

  active = 1;
}

static void deactivate(void)
{
  /* close_adc(); */

  /* disable int4 */
  EIMSK &= ~_BV(INT4);

  /* kill process */
  process_exit(&wind_speed_process);

  active = 0;
}

static int value(int type)
{
  uint16_t adc;
  uint8_t i, dir = 255;

  switch (type) {
  case 0:
    adc = read_adc(WINDDIR_ADC_CHAN);
    for (i = 0; i < 16; i++) {
      if ((adc >= wind_vane_lut[i].min) &&
          (adc <= wind_vane_lut[i].max)) {
        dir = wind_vane_lut[i].dir;
        break;
      }
    }
    return dir;
  case 1:
    return wind_speed;
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

SENSORS_SENSOR(wind_sensor, WIND_SENSOR,
	       value, configure, status);

