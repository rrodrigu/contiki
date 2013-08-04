#include "collect-view.h"

#include "dev/leds.h"
#include "dev/adc.h"
#include "dev/temperature-sensor.h"
#include "dev/humidity-sensor.h"
#include "dev/uv-sensor.h"
#include "dev/battery-sensor.h"
#include "dev/rain-sensor.h"
#include "dev/wind-sensor.h"
#include "dev/pwr_ctrl.h"

#include "collect-view-satxo.h"

/*---------------------------------------------------------------------------*/
void
collect_view_arch_read_sensors(struct collect_view_data_msg *msg)
{
  
  /* turn on sensors power */
  pwr_ctrl(1);

  /* enable adc */
  open_adc(ADC_PS_64, ADC_REF_INT16);

  /* activate sensors */
  SENSORS_ACTIVATE(battery_sensor);
  SENSORS_ACTIVATE(temperature_sensor);
  SENSORS_ACTIVATE(humidity_sensor);
  SENSORS_ACTIVATE(uv_sensor);
 
  msg->sensors[0] = battery_sensor.value(0);
  msg->sensors[1] = battery_sensor.value(1);
  msg->sensors[2] = temperature_sensor.value(0);
  msg->sensors[3] = humidity_sensor.value(0);
  msg->sensors[4] = uv_sensor.value(0);

  /* deactivate sensors */
  SENSORS_DEACTIVATE(battery_sensor);
  SENSORS_DEACTIVATE(temperature_sensor);
  SENSORS_DEACTIVATE(humidity_sensor);
  SENSORS_DEACTIVATE(uv_sensor);



  /* disable adc */
  close_adc();
  
  pwr_ctrl(0);
}
/*---------------------------------------------------------------------------*/
