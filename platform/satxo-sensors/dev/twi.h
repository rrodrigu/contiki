#ifndef __TWI_H__
#define __TWI_H__


/* clock rate */
#define TWI_CLK100  0x00
#define TWI_CLK400  0x01

/* message FLAGS */
#define TWI_M_RD 0x01

/* twi message */
struct twi_msg {
  uint8_t addr;
  uint8_t flags;
  uint8_t len;
  uint8_t *buf;
};


void twi_init(uint8_t clkrate);
int8_t twi_transfer(struct twi_msg *msg);

#endif
