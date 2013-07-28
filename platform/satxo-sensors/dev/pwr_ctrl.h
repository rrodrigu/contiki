/* sensors power control */

#ifndef __PWR_CTRL_H__
#define __PWR_CTRL_H__

#include <avr/io.h>


void pwr_ctrl_init(void);
void pwr_ctrl(unsigned char state);

#endif /* __PWR_CTRL_H__ */
