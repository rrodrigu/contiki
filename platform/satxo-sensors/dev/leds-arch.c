/* LED setup for satxo-sensors */

#include <avr/io.h>
#include "contiki-conf.h"
#include "dev/leds.h"


#define LED_PORT	(PORTB)
#define LED_DPORT	(DDRB)

#define LED_GREEN_PIN	(PB5)
#define LED_YELLOW_PIN	(PB6)


/*---------------------------------------------------------------------------*/
void
leds_arch_init(void)
{
  LED_PORT  |= _BV(LED_GREEN_PIN) | _BV(LED_YELLOW_PIN);
  LED_DPORT |= _BV(LED_GREEN_PIN) | _BV(LED_YELLOW_PIN);
}
/*---------------------------------------------------------------------------*/
unsigned char
leds_arch_get(void)
{
  unsigned char result = 0;

  if ( LED_PORT & _BV(LED_GREEN_PIN) )
    result |= LEDS_GREEN;
  if ( LED_PORT & _BV(LED_YELLOW_PIN) )
    result |= LEDS_YELLOW;

  return result;
}
/*---------------------------------------------------------------------------*/
void
leds_arch_set(unsigned char leds)
{
  uint8_t led_port = LED_PORT;

  led_port &= ~_BV(LED_GREEN_PIN);
  if ( leds & LEDS_GREEN )
    led_port |= _BV(LED_GREEN_PIN);

  led_port &= ~_BV(LED_YELLOW_PIN);
  if ( leds & LEDS_YELLOW )
    led_port |= _BV(LED_YELLOW_PIN);

  LED_PORT = led_port;
}
/*---------------------------------------------------------------------------*/

