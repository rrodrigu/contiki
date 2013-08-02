#include <avr/io.h>
#include "dev/uv-sensor.h"
#include "dev/adc.h"

#define UV_ADC_CHAN	(ADC_CHAN_ADC3)
#define ADC_REF		(ADC_REF_INT16)

const struct sensors_sensor humidity_sensor;
static uint8_t active;

static void activate(void)
{
  /* open_adc(ADC_PS_64, ADC_REF); */

  active = 1;
}

static void deactivate(void)
{
  /* close_adc(); */

  active = 0;
}

static int value(int type)
{
  switch (type) {
  case 0:
    return read_adc(UV_ADC_CHAN);
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

SENSORS_SENSOR(uv_sensor, UV_SENSOR,
	       value, configure, status);

