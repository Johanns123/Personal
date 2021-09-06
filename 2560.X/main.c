#define F_CPU 16000000 //define a frequência do microcontrolador - 16MHz
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>//para o uso de interrupções
#include "UART.h"
#include <stdio.h>
#include <stdlib.h>
#define set_bit(y,bit) (y|=(1<<bit))
#define clr_bit(y,bit) (y&=~(1<<bit))
#define cpl_bit(y,bit) (y^=(1<<bit))
#define tst_bit(y,bit) (y&(1<<bit))


//Variáveis globais da calibração de sensores
unsigned int valor_max[6] = {0, 0, 0, 0, 0, 0};
unsigned int valor_min[6] = {1023, 1023, 1023, 1023, 1023, 1023};
unsigned int valor_max_abs = 0;
unsigned int valor_min_abs = 1023;

char buffer[5]; //String que armazena valores de entrada para serem printadas
volatile char ch; //armazena o caractere lido
volatile char flag_com = 0; //flag que indica se houve recepção de dado

void setup (void);
void setup_logica();
void loop (void);
void INT_init (void);
void ADC_init (void);
void ADC_maq (void);
void ADC_conv_ch (unsigned char canal);
unsigned char ADC_ler (void);
void calibra_sensores();
void seta_calibracao();
void sensores();

unsigned char ADC_dados[6]; 

ISR(ADC_vect) 
{   
    ADC_maq();
}

/*Interrupções*/
ISR(USART0_RX_vect) 
{
    ch = UDR0; //Faz a leitura do buffer da serial

    UART_enviaCaractere(ch); //Envia o caractere lido para o computador
    //flag_com = 1; //Aciona o flag de comunicação
}

ISR(TIMER0_OVF_vect)
{   
    TCNT0 = 6;
    static int counter = 0, counter1 = 0;
    counter++;
    counter1++;
    if(counter == 2)
    {   
        sensores();
        for(int i = 0; i < 6; i++)
        {
            //sprintf(buffer, "%4d", ADC_dados[i]);
            //UART_enviaString(buffer);
            UART_enviaHex(ADC_dados[i]);
            UART_enviaCaractere('\t');
        }
        UART_enviaCaractere('\n');
        counter = 0;
        
    }
    
    if(counter1 == 500)
    {
        PORTB ^= (1<<PB7) | (1<<PB6) | (1<<PB5);
        counter1 = 0x00;
    }
}
//----------------------------------------------------------------------------------

int main (void) {
    setup();
    while(1) loop();
    return 0;
}

void setup (void) {

    MCUCR &= 0xef;      //habilita pull up quando configurado e desabilita algumas configurações prévias do MCU
    DDRB = 0xff;
    PORTB = 0x00;
    ADC_init();
    UART_config(16); //Inicializa a comunicação UART com 57.6kbps
    
    sei();
    //----> Calibração dos Sensores frontais <----//
    ADC_maq();
    calibra_sensores(); //calibração dos sensores //A calibração vai conseguir acompanhar o AD
                                                  //ou pode ser que o vetor não seja preenchido a tempo?
                                                  //É necessário colocar um contador
                                                  //para depois chamar a função de calibração?
    
    seta_calibracao(); //estabelece o limiar dos sensores através dos valores da função de cima
    sensores(); //determina o limiar dos sensores e printa seus valores na tela
    
    _delay_ms(500);
    
    INT_init();
}

void loop (void) {
    
}

void INT_init (void) {
    
    TCCR0B = 0b00000011; //TC0 com prescaler de 64
    TCNT0 = 6; //Inicia a contagem em 100 para, no final, gerar 1ms
    TIMSK0 = 0b00000001; //habilita a interrupção do TC0
}

void ADC_init (void) {
     //Fosc = 16MHz -> Tosc = 62,5ns
    //Fadc_max = 200kHz -> Tadc_min = 5us
    // Tadc_min / Tosc = 80 -> CK/128 - TADC_resultante = 8us
    //Primeira conversão = 25 ciclos de AD = 25 * 8us
    //Demais conversões = 13 ciclos de AD = 13 *8us
    //Tempo total para todas as conversões = (25*8) + (13*8*6) = 720us
    //720us sem contar os ciclos de máquina para executar as funções e o switch case
    //Logo, atentar à temporização de 1ms
    
    
    
    //FADC = 1MHz
    //Tadc = 1/1MHz = 1us
    //Primeira Conversão = 25*1 = 25us
    //Demais conversões = 14*1 = 14us
    //Tempo total do prieiro ciclo = (25*1) + (14*1*5) = 95us
    //Tempo das demais conversões = 14*1*6 = 84us
    //Utilizando teoria de amostragem -> 168->190us
    
    
    //FADC = 500kHz
    //Tadc = 1/500kHz = 2us
    //Primeira Conversão = 25*2 = 50us
    //Demais conversões = 14*2 = 28us
    //Tempo total do prieiro ciclo = (25*2) + (14*2*5) = 190us
    //Tempo das demais conversões = 14*2*6 = 168us
    //Utilizando teoria de amostragem -> 336->380us
    
    ADMUX = 0x40; //0100-0000
    ADCSRA = 0x8c; //1000-1100
    ADCSRB = 0x00; // 0000-0000
    DIDR0 = 0xff;// 1111-1111
    DIDR2 = 0xff;//1111-1111
     
}

void ADC_maq (void) {
    
    static unsigned char estado = 10;
    
    switch (estado) {
        
        case 0:
            estado = 1;
            ADC_dados[0] = ADC_ler();
            ADC_conv_ch(1);
            break;
            
        case 1:
            estado = 2;
            ADC_dados[1] = ADC_ler();
            ADC_conv_ch(2);
            break;
            
        case 2:
            estado = 3;
            ADC_dados[2] = ADC_ler();
            ADC_conv_ch(3);
            break;
            
        case 3:
            estado = 4;
            ADC_dados[3] = ADC_ler();
            ADC_conv_ch(4);
            break;
            
        case 4:
            estado = 5;
            ADC_dados[4] = ADC_ler();
            ADC_conv_ch(5);
            break;
            
        case 5:
            estado = 6;
            ADC_dados[5] = ADC_ler();
            ADC_conv_ch(0);
            break;
            
        default:
            estado = 0;
            ADC_conv_ch(0);
            ADC_dados[0] = ADC_ler();
            break; 
    }
    
}

void ADC_conv_ch (unsigned char canal)
{
    
    if(canal < 8)
    {
        ADMUX &= 0xf0;
        ADMUX  |= (canal & 0x0f);
        ADCSRB = 0x00;
    }
    
    else
    {
        canal &= 0x07;  //apago o bit 4 -> 0000-1000
        ADMUX &= 0xf0;
        ADMUX  |= (canal & 0x0f);
        ADCSRB = 0x08;  //0000-1000
    }
    ADCSRA |= 0x40;
            
}

unsigned char ADC_ler ( void ) {
    
    /*int dado = ADCL;
    dado |= ADCH << 8;
    return dado;*/
    return ADCL;
}



void calibra_sensores() {
    //=====Função que inicializa a calibração====//
    for (int i = 0; i < 120; i++) {
        for (int i = 0; i < 6; i++) {
            if (ADC_dados[i] < valor_min [i]) {
                valor_min[i] = ADC_dados[i];
            }
            if (ADC_dados[i] > valor_max [i]) {
                valor_max[i] = ADC_dados[i];
            }

        }
        
        
        _delay_ms(10);  //tempo o suficiente para o pessoa calibrar os sensores mecanicamente
        
    }

}

void seta_calibracao() {
    //----> Calibração dos Sensores frontais <----//

    //função que seta o limiar dos sensores
    for (int i = 0; i < 6; i++) {
        if (valor_min [i] < valor_min_abs && valor_min[i] !=0 ) {
            valor_min_abs = valor_min [i];
        } 
        
        if (valor_max [i] > valor_max_abs) {
            valor_max_abs = valor_max [i];
        }

    }
            
}

void sensores() {

    //======Estabelece o limiar da leitura dos sensores====//
    //função de correção da calibração
    for (int i = 0; i < 6; i++) {
        if (ADC_dados[i] < valor_min_abs) {
            ADC_dados[i] = valor_min_abs;
        }
        if (ADC_dados[i] > valor_max_abs) {
            ADC_dados[i] = valor_max_abs;
        }

    }
}

