/*---------------------------------------------------------------
 * BIBLIOTECA PARA UTILIZACAO DO CONVERSOR AD DO AVR
 * Modificada por: PROF. André Costa Canella
 * 08/2021
 * -----------------------------------------------------------------*/

void ADC_init (void) {
     //Fosc = 16MHz -> Tosc = 62,5ns
    //Fadc_max = 200kHz -> Tadc_min = 5us
    // Tadc_min / Tosc = 80 -> CK/128 - TADC_resultante = 8us
    //Primeira conversão = 25 ciclos de AD = 25 * 8us
    //Demais conversões = 13 ciclos de AD = 13 *8us
    //Tempo total para todas as conversões = (25*8) + (13*8*6) = 720us
    //720us sem contar os ciclos de máquina para executar as funções e o switch
    //Logo, atentar à temporização de 1ms
    
    ADMUX = 0x40; //0100-0000
    ADCSRA = 0x8f; //1000-1111
    ADCSRB = 0x00; // 0000-0000
    DIDR0 = 0x3f;// 0011-1111
     
}

void ADC_conv_ch (unsigned char canal) {
    
    ADMUX &= 0xf0;
    ADMUX |= (canal & 0x0f);
    
    ADCSRA |= 0x40;
            
}

int ADC_ler ( void ) {
    
    int dado = ADCL;
    dado |= ADCH << 8;
    
    return dado;
}