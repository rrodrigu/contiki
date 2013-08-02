#include <stdio.h>
#include "contiki.h"
#include "dev/leds.h"
#include "dev/adc.h"
#include "dev/temperature-sensor.h"
#include "dev/humidity-sensor.h"
#include "dev/uv-sensor.h"
#include "dev/battery-sensor.h"
#include "dev/rain-sensor.h"
#include "dev/wind-sensor.h"
#include "dev/pwr_ctrl.h"

static struct {
  float vbat;   /* in v */
  float vpv;    /* in v */
  float temp;    /* in celcius */
  float humid;  /* in percentage */
  uint16_t uv;
  uint8_t wind_dir;
  float wind_speed;
  float rain_gauge;
} sensor_data;

int decimal(float i, uint16_t m)
{
  int d = (int) i;
  float f = i - d;
  f = f * m;
  return (int) f;
}

/*---------------------------------------------------------------------------*/
PROCESS(test_all_process, "All sensors Test");
AUTOSTART_PROCESSES(&test_all_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_all_process, ev, data)
{
  static struct etimer et;
  static int raw;
  static float f;

  PROCESS_BEGIN();

  /* turn on sensors power */
  pwr_ctrl(1);

  /* enable adc */
  open_adc(ADC_PS_64, ADC_REF_INT16);

  /* activate sensors */
  SENSORS_ACTIVATE(battery_sensor);
  SENSORS_ACTIVATE(temperature_sensor);
  SENSORS_ACTIVATE(humidity_sensor);
  SENSORS_ACTIVATE(uv_sensor);
  SENSORS_ACTIVATE(wind_sensor);
  SENSORS_ACTIVATE(rain_sensor);

  while(1) {
#define VREF 1.600
#define GAIN 3.000
    /* read vbat */
    sensor_data.vbat = (battery_sensor.value(0) * VREF * GAIN) / 1024;

    /* read vpv */
    sensor_data.vpv = (battery_sensor.value(1) * VREF * GAIN) / 1024;

    /* read temperature */
    sensor_data.temp = ((float)temperature_sensor.value(0)) / 16.0;

    /* read humidity */
    raw = humidity_sensor.value(0);

#define VSUP 3.300
#define K1   0.00636
#define K2   0.1515
    /* Vout = Vsup * (0.00636 * (sensor RH) + 0.1515) */
    f = ((raw * VREF * GAIN) / (1024 * VSUP) - K2) / K1;
#define K3   1.0546
#define K4   0.00216
    /* true RH = (sensor RH) / (1.0546 - 0.00216 * temp) */
    sensor_data.humid = f / (K3 - K4 * sensor_data.temp);

    /* read UV */
    sensor_data.uv = uv_sensor.value(0);

    /* read wind direction */
    sensor_data.wind_dir = wind_sensor.value(0);

    /* read wind speed */
    sensor_data.wind_speed = wind_sensor.value(1) * 2.4 / 10;

    /* read rain gauge */
    sensor_data.rain_gauge = rain_sensor.value(0) * 0.2794;

    printf("\nVbat        = %d.%03d V\n",
        (int) sensor_data.vbat,
        decimal(sensor_data.vbat, 1000));

    printf("Vpv         = %d.%03d V\n",
        (int) sensor_data.vpv,
        decimal(sensor_data.vpv, 1000));

    printf("Temperature = %d.%02d Celcius\n",
        (int) sensor_data.temp,
        decimal(sensor_data.temp, 100));

    printf("Humidity    = %d.%01d %%\n",
        (int) sensor_data.humid,
        decimal(sensor_data.humid, 10));

    printf("UV          = %d\n", sensor_data.uv);

    printf("Wind dir    = %d\n", sensor_data.wind_dir);

    printf("Wind speed  = %d.%01d km/h\n",
        (int) sensor_data.wind_speed,
        decimal(sensor_data.wind_speed, 10));

    printf("Rain gauge  = %d.%01d mm/min\n",
        (int) sensor_data.rain_gauge,
        decimal(sensor_data.rain_gauge, 10));


    etimer_set(&et, CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    leds_toggle(LEDS_GREEN);
    leds_toggle(LEDS_YELLOW);
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
