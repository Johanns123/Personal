#include "ADC.h"


// ADC
//==============================================================================
void adc_setup(void)
{
  //FADC = 1MHz
  //Tadc = 1/1MHz = 1us
  //Primeira Conversao = 25*1 = 25us
  //Demais conversoes = 14*1 = 14us
  //Tempo total do prieiro ciclo = (25*1) + (14*1*5) = 95us
  //Tempo das demais conversoes = 14*1*6 = 84us
  //Utilizando teoria de amostragem -> 168->190us
  
  ADMUX = 0x60; //0110-0000   //Refer�ncia no AVCC, deslocado a direita
  ADCSRA = 0x8c; //1000-1100  //ADC habilitado, interrup��o do AD habilitado e prescaler de 16 - 1MHz
  ADCSRB = 0x00; // 0000-0000 //Modo livre
  //DIDR0 = 0x3f;// 0011-1111   //Desabilita a entrada digital desses terminais

  //Aten��o. Segundo o datasheet quanto maior a velocidade,
  //menor a precis�o do AD, logo, utilizar 8bits em freq. elevadas
}

void adc_conversion_ch_service(unsigned char channel)
{
  ADMUX &= 0xf0;
  ADMUX |= (channel & 0x0f);
  
  ADCSRA |= 0x40;
}

unsigned char adc_read_service(void)
{
  unsigned char dado = ADCH;
  return dado;
}
//==============================================================================


void tratar_leitura_do_ADC(void)
{
  static unsigned char estado = 10;
  
  switch (estado) {
      
    case 0:
      estado = 0;
      AD_pins[0] = adc_read_service();
      adc_conversion_ch_service(0);
      break;

    default:
      estado = 0;
      adc_conversion_ch_service(0);
      AD_pins[0] = adc_read_service();
      break; 
  }    
}