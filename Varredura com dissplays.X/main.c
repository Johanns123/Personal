/*
 * File:   main.c
 * Author: johan
 *
 * Created on 29 de Maio de 2021, 09:03
 */


#define F_CPU 16000000      //define a frequencia do uC para 16MHz
#include <avr/io.h>         //biblioteca de mapeamento dos registradores
#include <util/delay.h>     //biblioteca que gera atraso
#include <avr/interrupt.h>

//Definições de macros para trabalho com bits
#define	set_bit(y,bit)	(y|=(1<<bit))	//coloca em 1 o bit x da variável Y
#define	clr_bit(y,bit)	(y&=~(1<<bit))	//coloca em 0 o bit x da variável Y
#define cpl_bit(y,bit) 	(y^=(1<<bit))	//troca o estado lógico do bit x da variável Y
#define tst_bit(y,bit) 	(y&(1<<bit))	//retorna 0 ou 1 conforme leitura do bit

//protótipo das funções de interrupção
ISR(TIMER1_OVF_vect);
ISR(TIMER0_OVF_vect);

//Vetor de codificação do display, depende das postas conectadas 0-F
unsigned char vetor_disp[16] = {0xE7, 0x84, 0xD3, 0xD6, 0xB4, 0x76, 0x77, 0xC4,
                                0xF7, 0xF6, 0xF5, 0x37, 0x63, 0x97, 0x73, 0x71};

volatile int contador = 0;      //são volatile pois serão utilizadas na interrupção
volatile char unidade = 0;
volatile char dezena = 0;
volatile char disp = 0;

int main(void) {
    DDRB =  0b00000011;  //PB0 e PB1 como saída
    PORTB = 0b00000000; //liga o display de 7 segmentos
    
    DDRC =  0b000000000;  //PORTC como entrada
    PORTC = 0b000001100;  //Habillita pull-up dos botoes
    
    DDRD =  0b111111111;  //PORTD definido como saída
    PORTD = 0b000000000;     //Inicializa apagado
    
    TCCR1B = 0b00000101;    //TC1 com prescaler de 1024
    TCNT1 = 49911;          //inicia a contagem em 49911 para gerar 1s
    TIMSK1 = 0b00000001;    //habilita a interrupção do TC1
    
    TCCR0B = 0b00000101;    //TC0 com precaler de 1024
    TCNT0 = 100;             //Inicia a contagem em 100 para gerar 10ms
    TIMSK0 = 0b00000001;     //Habilita a interrupção do TC0
    
    sei();                  //habilita a chave de interrupção global
    
    
    while (1) {
        
    }
}

ISR(TIMER1_OVF_vect){
    TCNT1 = 49911;
    unidade++;
    if(unidade == 10){
        unidade =0;
        dezena++;
        if(dezena == 10){
            dezena = 0;
        }
    }
}

ISR(TIMER0_OVF_vect){
    contador++;
    TCNT0 = 100;
    if (contador == 1){
        contador = 0;
        switch(disp){
            case 0: clr_bit(PORTB,PB1);
                    set_bit(PORTB, PB0);
                    PORTB = vetor_disp[unidade];
                    disp = 1;
                    break;
            case 1: clr_bit(PORTB,PB0);
                    set_bit(PORTB, PB1);
                    PORTB = vetor_disp[dezena];
                    disp = 0;
                    break;
        }
    }
}

