#include "contiki-conf.h"
#include "dev/leds.h"


#include <avr/io.h>

#define ATMEGA0V2


#ifdef ATMEGA0V1

#define LED_PORT                        (PORTE)
#define LED_PORT_DIR                    (DDRE)

#define LED_PIN_0                       (PE3)



/*---------------------------------------------------------------------------*/
void
leds_arch_init(void)
{
  /*LED_PORT0 |= (1 << LED_PIN_0);
  LED_PORT_DIR0 |= (1 << LED_PIN_0);*/
  LED_PORT |= (1 << LED_PIN_0);
  LED_PORT_DIR |= (1 << LED_PIN_0);
  /*LED_PORT |= (1 << LED_PIN_2);
  LED_PORT_DIR |= (1 << LED_PIN_2);*/
}
/*---------------------------------------------------------------------------*/
unsigned char
leds_arch_get(void)
{
  unsigned char result = 0;

  /*if ( LED_PORT0 & _BV(LED_PIN_0 ) )
    result |= LEDS_GREEN;*/
  if ( LED_PORT & _BV(LED_PIN_0 ) )
    result |= LEDS_YELLOW;
  /*if ( LED_PORT & _BV(LED_PIN_2 ) )
    result |= LEDS_RED;*/

  return result;
}

/*---------------------------------------------------------------------------*/
void
leds_arch_set(unsigned char leds)
{
  uint8_t led_port = LED_PORT;

  /*LED_PORT0 &= ~_BV(LED_PIN_0);
  if ( leds & LEDS_GREEN )
    LED_PORT0 |= _BV(LED_PIN_0);*/

  led_port &= ~_BV(LED_PIN_0);
  if ( leds & LEDS_YELLOW )
    led_port |= _BV(LED_PIN_0);

  /*led_port &= ~_BV(LED_PIN_2);
  if ( leds & LEDS_RED )
    led_port |= _BV(LED_PIN_2);*/

  LED_PORT = led_port;
}

#endif


#ifdef ATMEGA0V2

#define LED_PORT		(PORTB)
#define LED_PORT_DIR		(DDRB)

#define LED_PIN_0		(PB5)
#define LED_PIN_1		(PB6)


/*---------------------------------------------------------------------------*/
void
leds_arch_init(void)
{
  /*LED_PORT0 |= (1 << LED_PIN_0);
  LED_PORT_DIR0 |= (1 << LED_PIN_0);*/
  LED_PORT |= (1 << LED_PIN_0) | (1 << LED_PIN_1);
  LED_PORT_DIR |= (1 << LED_PIN_0) | (1 << LED_PIN_1);
  /*LED_PORT |= (1 << LED_PIN_2);
  LED_PORT_DIR |= (1 << LED_PIN_2);*/
}
/*---------------------------------------------------------------------------*/
unsigned char
leds_arch_get(void)
{
  unsigned char result = 0;

  /*if ( LED_PORT0 & _BV(LED_PIN_0 ) )
    result |= LEDS_GREEN;*/
  if ( LED_PORT & _BV(LED_PIN_0 ) )
    result |= LEDS_GREEN;
  if ( LED_PORT & _BV(LED_PIN_1 ) )
    result |= LEDS_YELLOW;
  /*if ( LED_PORT & _BV(LED_PIN_2 ) )
    result |= LEDS_RED;*/

  return result;
}

/*---------------------------------------------------------------------------*/
void
leds_arch_set(unsigned char leds)
{
  uint8_t led_port = LED_PORT;

  led_port &= ~_BV(LED_PIN_0);
  if ( leds & LEDS_GREEN )
    led_port |= _BV(LED_PIN_0);

  led_port &= ~_BV(LED_PIN_1);
  if ( leds & LEDS_YELLOW )
    led_port |= _BV(LED_PIN_1);

  /*led_port &= ~_BV(LED_PIN_2);
  if ( leds & LEDS_RED )
    led_port |= _BV(LED_PIN_2);*/

  LED_PORT = led_port;
}




#endif


/*---------------------------------------------------------------------------*/
