#include <avr/io.h>
#include "dev/adc.h"


void open_adc(adc_ps_t prescale, adc_ref_t ref)
{
  /* Configure clock prescale and voltage reference */
  ADCSRA = prescale;
  ADMUX = (uint8_t) ref;

  /* Disable ADC interrupts. */
  ADCSRA &= ~( _BV(ADIE) | _BV(ADIF) );

  /* Enable ADC */
  ADCSRA |= _BV(ADEN);
}

uint16_t read_adc(adc_chan_t chan)
{
  uint16_t reading;

  /* Setup input channel */
  ADMUX &= 0xc0;
  ADMUX |= (uint8_t) chan;

  /* Start conversion. */
  ADCSRA |= _BV(ADSC);

  /* Wait until conversion is completed. */
  while ( ADCSRA & _BV(ADSC) );

  /* Read ADC output */
  reading = (uint16_t) ADC;

  return reading;
}

void close_adc(void)
{
  /* Disable ADC. */
  ADCSRA &= ~_BV(ADEN);
}

