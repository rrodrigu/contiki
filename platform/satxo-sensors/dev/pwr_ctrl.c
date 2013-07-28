/* sensors power control */

#include <avr/io.h>
#include "dev/pwr_ctrl.h"

/* init port pin */
void pwr_ctrl_init(void)
{
	/* set pin to output */
	PWR_CTRL_DPORT |= _BV(PWR_CTRL_PIN);

	/* set pin low (shutdown) */
	sensors_power(0);
}

/* set port pin */
void pwr_ctrl(unsigned char state)
{
	if (state)
		PWR_CTRL_PORT |= _BV(PWR_CTRL_PIN);
	else
		PWR_CTRL_PORT &= ~_BV(PWR_CTRL_PIN);
}

