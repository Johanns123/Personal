#include <avr/io.h>

unsigned int AD_pins [1];

void adc_setup(void);
void adc_conversion_ch_service(unsigned char channel);
unsigned int adc_read_service(void);
void tratar_leitura_do_ADC(void);