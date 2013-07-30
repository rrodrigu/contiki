#include <avr/io.h>
#include "dev/battery-sensor.h"
#include "dev/adc.h"


#define POWER_MON_ENABLE_PORT	(PORTG)
#define POWER_MON_ENABLE_DPORT	(DDRG)
#define POWER_MON_ENABLE_PIN	(PG1)

#define VBAT_ADC_CHAN	(ADC_CHAN_ADC1)
#define VPV_ADC_CHAN	(ADC_CHAN_ADC0)

#define ADC_REF		(ADC_REF_INT16)


const struct sensors_sensor battery_sensor;
static uint8_t active;

static void activate(void)
{
  open_adc(ADC_PS_64, ADC_REF);

  /* Enable battery sensor reading */
  POWER_MON_ENABLE_DPORT |= _BV(POWER_MON_ENABLE_PIN);
  POWER_MON_ENABLE_PORT |= _BV(POWER_MON_ENABLE_PIN);

  active = 1;
}

static void deactivate(void)
{
  POWER_MON_ENABLE_PORT &= ~_BV(POWER_MON_ENABLE_PIN);

  close_adc();

  active = 0;
}

static int value(int type)
{
  switch (type) {
  case 0:
    return read_adc(VBAT_ADC_CHAN);
  case 1:
    return read_adc(VPV_ADC_CHAN);
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

SENSORS_SENSOR(battery_sensor, BATTERY_SENSOR,
	       value, configure, status);

