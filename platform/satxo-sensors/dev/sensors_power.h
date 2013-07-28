/* sensors power control */

#ifndef _SENSORS_POWER_H_
#define _SENSORS_POWER_H_

#include <avr/io.h>


#define SENSORS_POWER_PORT	(PORTG)
#define SENSORS_POWER_DPORT	(DDRG)
#define SENSORS_POWER_PIN	(PG0)


void sensors_power_init(void);
void sensors_power(unsigned char state);


#endif

