/*
 * Copyright (c) 2006, Technical University of Munich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */
#define PRINTF(FORMAT,args...) printf_P(PSTR(FORMAT),##args)

#define ANNOUNCE_BOOT 1    //adds about 600 bytes to program size
#if ANNOUNCE_BOOT
#define PRINTA(FORMAT,args...) printf_P(PSTR(FORMAT),##args)
#else
#define PRINTA(...)
#endif

#define DEBUG 0
#if DEBUG
#define PRINTD(FORMAT,args...) printf_P(PSTR(FORMAT),##args)
#else
#define PRINTD(...)
#endif

#include <avr/pgmspace.h>
#include <avr/fuse.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <string.h>
#include <dev/watchdog.h>
#include <avr/power.h>

#include "loader/symbols-def.h"
#include "loader/symtab.h"

#include "radio/rf230bb/rf230bb.h"
#include "net/mac/frame802154.h"
#include "net/mac/framer-802154.h"
#include "net/sicslowpan.h"

#include <dev/leds.h>
#include "contiki.h"
#include "contiki-net.h"
#include "contiki-lib.h"

#include "dev/rs232.h"
#include "dev/serial-line.h"
#include "dev/slip.h"

#include "net/rime.h"





#include <init-net.h>



/* Track interrupt flow through mac, rdc and radio driver */
//#define DEBUGFLOWSIZE 32
#if DEBUGFLOWSIZE
uint8_t debugflowsize,debugflow[DEBUGFLOWSIZE];
#define DEBUGFLOW(c) if (debugflowsize<(DEBUGFLOWSIZE-1)) debugflow[debugflowsize++]=c
#else
#define DEBUGFLOW(c)
#endif

/* Get periodic prints from idle loop, from clock seconds or rtimer interrupts */
/* Use of rtimer will conflict with other rtimer interrupts such as contikimac radio cycling */
/* STAMPS will print ENERGEST outputs if that is enabled. */
#define PERIODICPRINTS 0
#if PERIODICPRINTS
//#define PINGS 64
#define ROUTES 600
#define STAMPS 60
#define STACKMONITOR 1024
uint32_t clocktime;
#define TESTRTIMER 0
#if TESTRTIMER
uint8_t rtimerflag=1;
struct rtimer rt;
void rtimercycle(void) {rtimerflag=1;}
#endif
#endif

uint16_t ledtimer;

/*-------------------------------------------------------------------------*/
/*----------------------Configuration of the .elf file---------------------*/
#if 1
/* The proper way to set the signature is */
#include <avr/signature.h>
#else
/* Older avr-gcc's may not define the needed SIGNATURE bytes. Do it manually if you get an error */
typedef struct {const unsigned char B2;const unsigned char B1;const unsigned char B0;} __signature_t;
#define SIGNATURE __signature_t __signature __attribute__((section (".signature")))
SIGNATURE = {
  .B2 = 0x01,//SIGNATURE_2, //ATMEGA128rfa1
  .B1 = 0xA7,//SIGNATURE_1, //128KB flash
  .B0 = 0x1E,//SIGNATURE_0, //Atmel
};
#endif

#if 1
/* JTAG, SPI enabled, Internal RC osc, Boot flash size 4K, 6CK+65msec delay, brownout disabled */
FUSES ={.low = 0xe2, .high = 0x99, .extended = 0xff,};
#else
/* JTAG+SPI, Boot 4096 words @ $F000, Internal  oscillator, startup 6 CK +0 ms, Brownout 1.8 volts */
FUSES ={.low = 0xC2, .high = 0x99, .extended = 0xfe,};
#endif

uint8_t
rng_get_uint8(void) {
#if 1
  /* Upper two RSSI reg bits (RND_VALUE) are random in rf231 */
  uint8_t j;
  j = (PHY_RSSI&0xc0) + ((PHY_RSSI>>2)&0x30) + ((PHY_RSSI>>4)&0x0c) + ((PHY_RSSI>>6)&0x03);
#else
/* Get a pseudo random number using the ADC */
  uint8_t i,j;
  ADCSRA=1<<ADEN;             //Enable ADC, not free running, interrupt disabled, fastest clock
  for (i=0;i<4;i++) {
    ADMUX = 0;                //toggle reference to increase noise
    ADMUX =0x1E;              //Select AREF as reference, measure 1.1 volt bandgap reference.
    ADCSRA|=1<<ADSC;          //Start conversion
    while (ADCSRA&(1<<ADSC)); //Wait till done
	j = (j<<2) + ADC;
  }
  ADCSRA=0;                   //Disable ADC
#endif
  PRINTD("rng issues %d\n",j);
  return j;
}

/*-------------------------Low level initialization------------------------*/
/*------Done in a subroutine to keep main routine stack usage small--------*/
void initialize(void)
{
  watchdog_init();
  watchdog_start();
  leds_init();
  leds_off(LEDS_YELLOW);
  leds_on(LEDS_GREEN);

  //clock_prescale_set(0);
  
  rs232_init(RS232_PORT_0, USART_BAUD_38400,USART_PARITY_NONE | USART_STOP_BITS_1 | USART_DATA_BITS_8);
  rs232_redirect_stdout(RS232_PORT_0);
  rs232_set_input(RS232_PORT_0, serial_line_input_byte);

  
  clock_init();

  if(MCUSR & (1<<PORF )) PRINTD("Power-on reset.\n");
  if(MCUSR & (1<<EXTRF)) PRINTD("External reset!\n");
  if(MCUSR & (1<<BORF )) PRINTD("Brownout reset!\n");
  if(MCUSR & (1<<WDRF )) PRINTD("Watchdog reset!\n");
  if(MCUSR & (1<<JTRF )) PRINTD("JTAG reset!\n");


  PRINTA("\n*******Booting %s*******\n",CONTIKI_VERSION_STRING);


  /* Get a random seed for the 802.15.4 packet sequence number.
  * Some layers will ignore duplicates found in a history (e.g. Contikimac)
  * causing the initial packets to be ignored after a short-cycle restart.
  */
  random_init(rng_get_uint8());

  /* Initialize process subsystem */
  process_init();
  /* etimers must be started before ctimer_init */
  process_start(&etimer_process, NULL);
  ctimer_init();
  /* rtimers needed for radio cycling */
  rtimer_init();
  
  
  serial_line_init();
  

  init_net(0);

  /* Autostart other processes */
  autostart_start(autostart_processes);
  PRINTF("Processes running\n");
  leds_off(LEDS_YELLOW);
}

/*-------------------------------------------------------------------------*/
/*------------------------- Main Scheduler loop----------------------------*/
/*-------------------------------------------------------------------------*/
int
main(void)
{
  initialize();

  while(1) {
    process_run();
    watchdog_periodic();
  }
  return 0;
}

/*---------------------------------------------------------------------------*/

void log_message(char *m1, char *m2)
{
  PRINTF("%s%s\n", m1, m2);
}
