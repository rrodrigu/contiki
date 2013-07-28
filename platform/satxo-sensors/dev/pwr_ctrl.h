/* sensors power control */

#ifndef __PWR_CTRL_H__
#define __PWR_CTRL_H__

#include <avr/io.h>


#define PWR_CTRL_PORT	(PORTG)
#define PWR_CTRL_DPORT	(DDRG)
#define PWR_CTRL_PIN	(PG0)

void pwr_ctrl_init(void);
void pwr_ctrl(unsigned char state);

#endif /* __PWR_CTRL_H__ */
