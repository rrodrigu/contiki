#include "contiki.h"
#include "dev/owi.h"
#include <string.h>

uint8_t serial_nr[8];

int get_serial_nr()
{
  uint8_t acc, crc, family, i;

  owi_init(OWI_PIN_SERIAL);
  owi_read_rom(serial_nr,OWI_PIN_SERIAL);
  
  family = serial_nr[0];
  /* verify family number, must be 0x01 for ds2411 */
  if(family != 0x01) {
    goto fail;
  }
  
  crc = serial_nr[7];
  /* verify CRC match */
  acc = 0;
  for(i = 0; i < 7; i++){
    acc = owi_compute_crc8(serial_nr[i],acc);
  }
  if(acc == crc){
    return 1; /* CRC matched */
  }

  fail:
  memset(serial_nr, 0x0, sizeof(serial_nr));
  return 0; /* CRC or family match failed */
}

