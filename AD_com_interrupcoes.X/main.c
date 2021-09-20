/*Bibliotecas e frequência do uc*/
#define F_CPU 16000000        //define a frequencia do uC para 16MHz
#include <avr/io.h>           //Biblioteca geral dos AVR
#include <avr/interrupt.h>    //Biblioteca de interrupção
#include <stdio.h>            //Bilioteca do C
#include <util/delay.h>       //Biblioteca geradora de atraso
#include <avr/io.h>
#include "ADC.h"
#include "UART.h"
#include <stdlib.h>

//variáveis de comando para os registradores
#define set_bit(y,bit) (y|=(1<<bit)) //coloca em 1 o bit x da variável Y
#define clr_bit(y,bit) (y&=~(1<<bit)) //coloca em 0 o bit x da variável Y
#define cpl_bit(y,bit) (y^=(1<<bit)) //troca o estado lógico do bit x da variável Y
#define tst_bit(y,bit) (y&(1<<bit)) //retorna 0 ou 1 conforme leitura do bit
//==============================================================

/*Mapeamento de Hardware*/

/*Estruturas*/


//Variáveis globais da calibração de sensores
unsigned int valor_max[6] = {0, 0, 0, 0, 0, 0};
unsigned int valor_min[6] = {255, 255, 255, 255, 255, 255};
unsigned int valor_max_abs = 255;
unsigned int valor_min_abs = 0;
//Variáveis globais
//================
int ADC_dados[6];

volatile char ch; //armazena o caractere lido
char buffer[5]; //String que armazena valores de entrada para serem printadas

//Protótipo das funções
//================
void setup_hardware(void);
void setup(void);
void INT_init(void);
void ADC_maq();
void loop();
void calibra_sensores();
void seta_calibracao();
void sensores();


ISR(USART_RX_vect) 
{
    ch = UDR0; //Faz a leitura do buffer da serial

    UART_enviaCaractere(ch); //Envia o caractere lido para o computador
    //flag_com = 1; //Aciona o flag de comunicação
}

ISR(TIMER0_OVF_vect)
{   
    TCNT0 = 240;
    static int counter = 0;
    counter++;
    if(counter == 4)
    {
        sensores();
        for(int i = 0; i < 6; i++)
        {
            //sprintf(buffer, "%4d", ADC_dados[i]);
            UART_enviaHex(ADC_dados[i]);
            //UART_enviaString(buffer);
            UART_enviaCaractere('\t');
        }
        UART_enviaCaractere('\n'); 
        counter = 0;
    }

}

ISR(PCINT1_vect)
{



}

ISR(ADC_vect)
{
    ADC_maq();
}


int main() {
    
    setup_hardware();
    ADC_init();
    //inic_LCD_4bits();
    sei();
    setup();
    INT_init();

    while (1) {
        loop();

    }
    return 0;
}
void setup_hardware(void)
{
    MCUCR &= 0xef;  //Pull up interno habilitado
    DDRB = 0xf0;    //Todo PORTB como saída
    PORTB = 0x00;   //inicia todos do PORTD em LOW
    DDRC = 0x00;    //PC0-PC3 como entrada
    PORTC = 0xff;   //entradas com pull up
    //DIDR0 = 0xff;   //desabilita entrada digital de PC1
    
}

void setup(void)
{
    UART_config(16);
    ADC_maq();
    calibra_sensores();
    seta_calibracao();
    set_bit(PORTB, PB5);
    _delay_ms(500);
    clr_bit(PORTB, PB5);
    _delay_ms(500);
    set_bit(PORTB, PB5);
    _delay_ms(500);
    clr_bit(PORTB, PB5);
    _delay_ms(500);
    set_bit(PORTB, PB5);
    _delay_ms(500);
    clr_bit(PORTB, PB5);
    _delay_ms(500);
    set_bit(PORTB, PB5);
    _delay_ms(2000);
    
    
}

void loop()
{
    

}
void INT_init(void)
{
    TCCR0B = 0b00000101; //TC0 com prescaler de 1024
    TCNT0 = 240; //Inicia a contagem em 56 para, no final, gerar 1ms
    TIMSK0 = 0b00000001; //habilita a interrupção do TC0
    
    
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
        
        
        _delay_ms(20);  //tempo o suficiente para o pessoa calibrar os sensores mecanicamente
        
    }

}

void seta_calibracao() {
    //----> Calibração dos Sensores frontais <----//

    //função que seta o limiar dos sensores
    for (int i = 0; i < 6; i++) {
        if (valor_min [i] > valor_min_abs && valor_min[i] !=0 ) {
            valor_min_abs = valor_min [i];
        } 
        
        if (valor_max [i] < valor_max_abs) {
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

void ADC_maq () 
{
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
            estado = 0;
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
