#include <avr/io.h>

unsigned char AD_pins [2];

void adc_setup(void);
void adc_conversion_ch_service(unsigned char channel);
unsigned char adc_read_service(void);
void tratar_leitura_do_ADC(void);