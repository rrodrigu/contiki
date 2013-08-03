#ifndef __ADC_H__
#define __ADC_H__

#include <avr/io.h>

typedef enum {
    ADC_PS_2   = ((0<<ADPS2)|(0<<ADPS1)|(1<<ADPS0)),
    ADC_PS_4   = ((0<<ADPS2)|(1<<ADPS1)|(0<<ADPS0)),
    ADC_PS_8   = ((0<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)),
    ADC_PS_16  = ((1<<ADPS2)|(0<<ADPS1)|(0<<ADPS0)),
    ADC_PS_32  = ((1<<ADPS2)|(0<<ADPS1)|(1<<ADPS0)),
    ADC_PS_64  = ((1<<ADPS2)|(1<<ADPS1)|(0<<ADPS0)),
    ADC_PS_128 = ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0))
} adc_ps_t;


typedef enum {
    ADC_REF_AREF  = ((0<<REFS1)|(0<<REFS0)),
    ADC_REF_AVDD  = ((0<<REFS1)|(1<<REFS0)),
    ADC_REF_INT15 = ((1<<REFS1)|(0<<REFS0)),
    ADC_REF_INT16 = ((1<<REFS1)|(1<<REFS0))
} adc_ref_t;


typedef enum {
    ADC_CHAN_ADC0 = ((0<<MUX4)|(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0)),
    ADC_CHAN_ADC1 = ((0<<MUX4)|(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(1<<MUX0)),
    ADC_CHAN_ADC2 = ((0<<MUX4)|(0<<MUX3)|(0<<MUX2)|(1<<MUX1)|(0<<MUX0)),
    ADC_CHAN_ADC3 = ((0<<MUX4)|(0<<MUX3)|(0<<MUX2)|(1<<MUX1)|(1<<MUX0)),
    ADC_CHAN_ADC4 = ((0<<MUX4)|(0<<MUX3)|(1<<MUX2)|(0<<MUX1)|(0<<MUX0)),
    ADC_CHAN_ADC5 = ((0<<MUX4)|(0<<MUX3)|(1<<MUX2)|(0<<MUX1)|(1<<MUX0)),
    ADC_CHAN_ADC6 = ((0<<MUX4)|(0<<MUX3)|(1<<MUX2)|(1<<MUX1)|(0<<MUX0)),
    ADC_CHAN_ADC7 = ((0<<MUX4)|(0<<MUX3)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0))
} adc_chan_t;


void open_adc(adc_ps_t prescale, adc_ref_t ref);
uint16_t read_adc(adc_chan_t chan);
void close_adc(void);

#endif /* __ADC_H__ */

