/*
 * File:   main.c
 * Author: johann
 *
 * Created on 26 de Maio de 2021, 17:27
 */

/*tempo =65536 * Prescaler/Fosc = 65536 * 1024/16000000 = 4, 19s   
 * tempo = X_bit_timer * Prescaler/Fosc
 Valor inicial de contagem = 256 - tempo_desejado*Fosc/Prescaler = 256 - 0,01*16000000/1024 = 98,75 = 99
 Valor inicial de contagem = X_bit_timer - tempo_desejado*Fosc/Prescaler*/
//65536 -2*16000000/1024

#define F_CPU 16000000  //define a frequencia do uC para 16MHz
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define set_bit(y,bit) (y|=(1<<bit)) //coloca em 1 o bit x da variável Y
#define clr_bit(y,bit) (y&=~(1<<bit)) //coloca em 0 o bit x da variável Y
#define cpl_bit(y,bit) (y^=(1<<bit)) //troca o estado lógico do bit x da variável Y
#define tst_bit(y,bit) (y&(1<<bit)) //retorna 0 ou 1 conforme leitura do bit
#define LED PD2

volatile unsigned int contador = 0;

ISR(TIMER0_OVF_vect) //interrupção do TC0
{
    contador++;
    if (contador == 10) {
        cpl_bit(PORTD, LED);
        contador = 0;
    }
    //o led vai piscar a aproximadamente 164ms
}


ISR(TIMER1_OVF_vect) //interrupção do TC1
{
    TCNT1 = 34286; //Recarrega o registrador para gerar 1s novamente
    cpl_bit(PORTD, PD6); //Inverte o estado do LED
}

void aula01(){
    DDRD = 0b11111111; //somente pino do LED como saída
    PORTD = 0b00000000; //apaga LEDs

    TCCR0B = 0b00000101; //TC0 com prescaler de 1024, a 16 MHz gera uma interrupção a cada 16,384 ms
    TIMSK0 = 0b00000001; //habilita a interrupção do TC0
    
    TCCR1B = 0b00000101; //TC1 com prescaler de 1024, a 16 MHz gera uma interrupção a cada 1s
    TCNT1 = 34286; //Inicia a contagem em 49911 para, no final, gerar 1s
    TIMSK1 = 0b00000001; //habilita a interrupção do TC1
    
    sei(); //habilita a chave de interrupção global

    while (1) {
        cpl_bit(PORTD, PD4);
        _delay_ms(500);
        /*A cada estouro do Timer 0 o programa desvia para ISR(TIMER0_OVF_vect)*/
    }    
}


/*ISR(TIMER1_OVF_vect){
    TCNT1 = 34286; //Inicia a contagem em 49911 para, no final, gerar 2s
    cpl_bit(PORTD,PD0);
}*/

void aula02(){
    DDRD = 0xFF;        //todos como saída
    PORTD = 0x00;       //Inicializa em LOW
    TCCR1B = 0b00000101; //TC1 com prescaler de 1024, a 16 MHz gera uma interrupção a cada 2s
    TCNT1 = 34286; //Inicia a contagem em 49911 para, no final, gerar 2s
    TIMSK1 = 0b00000001; //habilita a interrupção do TC1
    while(1){
        cpl_bit(PORTD, PB1);
        _delay_ms(2000);
    }
}

void main() {
    aula01();
    
    //aula02();
}