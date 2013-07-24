#include "collect-view.h"
/*#include "dev/cc2420.h"
#include "dev/leds.h"
#include "dev/light-sensor.h"
#include "dev/battery-sensor.h"
#include "dev/sht11-sensor.h"*/
#include "dev/temperature-sensor.h"

#include "collect-view-satxo.h"

/*---------------------------------------------------------------------------*/
void
collect_view_arch_read_sensors(struct collect_view_data_msg *msg)
{
  
  
  SENSORS_ACTIVATE(temperature_sensor);
  msg->sensors[TEMP_SENSOR] = temperature_sensor.value(0); // 3960 = 0.0C, +1 = +0.01C
  SENSORS_DEACTIVATE(temperature_sensor);
  
  
  /*SENSORS_ACTIVATE(light_sensor);
  SENSORS_ACTIVATE(battery_sensor);
  SENSORS_ACTIVATE(sht11_sensor);

  msg->sensors[BATTERY_VOLTAGE_SENSOR] = battery_sensor.value(0);
  msg->sensors[BATTERY_INDICATOR] = sht11_sensor.value(SHT11_SENSOR_BATTERY_INDICATOR);
  msg->sensors[LIGHT1_SENSOR] = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);
  msg->sensors[LIGHT2_SENSOR] = light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR);
  msg->sensors[TEMP_SENSOR] = sht11_sensor.value(SHT11_SENSOR_TEMP);
  msg->sensors[HUMIDITY_SENSOR] = sht11_sensor.value(SHT11_SENSOR_HUMIDITY);


  SENSORS_DEACTIVATE(light_sensor);
  SENSORS_DEACTIVATE(battery_sensor);
  SENSORS_DEACTIVATE(sht11_sensor);*/
  
  
}
/*---------------------------------------------------------------------------*/
