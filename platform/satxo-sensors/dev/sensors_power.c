/* sensors power control */

#include <avr/io.h>
#include "dev/sensors_power.h"

/* init port pin */
void sensors_power_init(void)
{
	/* set pin to output */
	SENSORS_POWER_DPORT |= _BV(SENSORS_POWER_DPIN);

	/* set pin low */
	sensors_power(0);
}

/* set port pin */
void sensors_power(unsigned char state)
{
	if (state)
		SENSORS_POWER_PORT |= _BV(SENSORS_POWER_PIN);
	else
		SENSORS_POWER_PORT &= ~_BV(SENSORS_POWER_PIN);
}



