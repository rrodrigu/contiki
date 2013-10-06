#include <stdio.h>
#include <util/delay.h>
#include "contiki.h"
#include "dev/leds.h"
//#include "dev/owi.h"
//#include "dev/ds2411_x.h"
#include "mac.h"
#include "dev/dht22.h"
#include "dev/sht11.h"
#include "dev/temperature-sensor.h"
#include "dev/adc.h"
#include "dev/humidity-sensor.h"
#include "dev/pwr_ctrl.h"

#define GAIN 3.000
#define VREF 1.250
#define VSUP 3.300

#define DEBUG_OWI

/*---------------------------------------------------------------------------*/
PROCESS(test_owi_process, "OWI interface Test");
AUTOSTART_PROCESSES(&test_owi_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_owi_process, ev, data)
{

  static struct etimer et;

  // TMP75
  static int tmp75_t_raw;
  static float tmp75_t;

  // HIH
  static int hih_h_raw;
  static float hih_h;

  // DHT22
  static int8_t dht22_t_int;
  static uint8_t dht22_t_dec;
  static uint8_t dht22_h_int;
  static uint8_t dht22_h_dec;

  // SHT11
  uint8_t sht11_ret;
  uint8_t sht11_h;
  int sht11_t;
  
  unsigned int sht11_t_raw, sht11_h_raw;

  int mac_ret;

  PROCESS_BEGIN();
  
  pwr_ctrl_init();
  
  /* turn on sensors power */
  pwr_ctrl(1);
  
  /* enable adc */
  open_adc(ADC_PS_64, ADC_REF_AREF);
  
#ifndef DEBUG_OWI
  SENSORS_ACTIVATE(temperature_sensor);
  SENSORS_ACTIVATE(humidity_sensor);
#endif
 
  /* read serial number by 1-wire interface */
/*  owi_init(OWI_PIN_SERIAL); 
  owi_read_rom(rom,OWI_PIN_SERIAL);
  printf("rom = 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x\n", 
     rom[7], rom[6], rom[5], rom[4], rom[3], rom[2], rom[1], rom[0]);
*/

  mac_ret = get_mac_addr();
  if(mac_ret == 0){
    printf("error reading serial number");
  } else {
    printf("rom = 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x\n", 
      mac_addr[7], mac_addr[6], mac_addr[5], mac_addr[4], 
      mac_addr[3], mac_addr[2], mac_addr[1], mac_addr[0]);
  }

#ifndef DEBUG_OWI
  while(1) {
    etimer_set(&et, CLOCK_SECOND * 10);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    leds_toggle(LEDS_GREEN);
    leds_toggle(LEDS_YELLOW);

    // TMP75
    tmp75_t_raw = temperature_sensor.value(0);
    tmp75_t =  (int)(tmp75_t_raw >> 4) + (int)(tmp75_t_raw & 0xf) * 0.0625;

    // HIH
    hih_h_raw = humidity_sensor.value(0);
    hih_h = ((hih_h_raw * VREF * GAIN) / (1024 * VSUP) - 0.1515) / 0.00636;
    hih_h = hih_h/(1.0546-0.00216*tmp75_t);

    printf("A:t=%d.%d h=%d\n", 
        (int)tmp75_t, (int)( (tmp75_t-(int)(tmp75_t))*10), (int) hih_h);

    // DHT22
    readDHT22(&dht22_t_int, &dht22_t_dec, &dht22_h_int, &dht22_h_dec);
    printf("D:t=%d.%d h=%d.%d\n",
        dht22_t_int , dht22_t_dec, dht22_h_int, dht22_h_dec);

    // SHT11
    sht11_ret = s_measure(MEASURE_TEMP,1);
    while(!s_resultready(1)){
      _delay_ms(10);
    }
    sht11_ret = s_readmeasurement( &sht11_t_raw,1);
    sht11_t = calc_sth11_temp(sht11_t_raw);
    
    sht11_ret = s_measure(MEASURE_HUMI,1);
    while(!s_resultready(1)){
      _delay_ms(10);
    }
    sht11_ret = s_readmeasurement( &sht11_h_raw,1);
    sht11_h=calc_sth11_humi(sht11_h_raw,sht11_t);
    
    printf("S:t=%d.%d h=%d\n",
        (int)(sht11_t / 10),(int)(sht11_t % 10),sht11_h);

    printf("-\n");

  }
#endif

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
