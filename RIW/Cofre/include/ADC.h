#ifndef ADC_H
#define ADC_H

#include <stdint.h>

uint8_t sensores_de_tensao[1];

void adc_conversion_ch_service(unsigned char channel);
uint8_t adc_read_service(void);
void tratar_leitura_do_ADC(void);
void adc_setup(void);

#endif