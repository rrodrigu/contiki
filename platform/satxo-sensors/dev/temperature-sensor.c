#include "dev/temperature-sensor.h"
#include "dev/twi.h"
#include "contiki.h"

#define TMP175_ADDR        0x90

const struct sensors_sensor temperature_sensor;

static int value(int type)
{
  int8_t ret;
  uint8_t buf[2];
  struct twi_msg msg = {
    .addr = TMP175_ADDR,
    .flags = TWI_M_RD,
    .len = 2,
    .buf = buf,
  };

  ret = twi_transfer(&msg);
  if (ret < 0)
    return 0x7fff;
  return (int) (buf[0] << 8 | buf[1]) >> 4;
}

static int configure(int type, int c)
{
  int8_t ret;
  uint8_t buf1[] = { 0x01, 0x60 };
  uint8_t buf2[] = { 0x00 };
  struct twi_msg msg[] = {
    {
      .addr = TMP175_ADDR,
      .flags = 0,
      .len = 2,
      .buf = buf1,  /* 12 bit resolution */
    },
    {
      .addr = TMP175_ADDR,
      .flags = 0,
      .len = 1,
      .buf = buf2,        /* pointer register = temp */
    }
  };

  twi_init(TWI_CLK100);

  ret = twi_transfer(&msg[0]);
  if (ret < 0)
    return ret;
  ret = twi_transfer(&msg[1]);
  if (ret < 0)
    return ret;

  return 0;
}
/*---------------------------------------------------------------------------*/
static int
status(int type)
{
  switch (type) {
  case SENSORS_READY:
  case SENSORS_ACTIVE:
    return 1;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(temperature_sensor, TEMPERATURE_SENSOR,
               value, configure, status);
