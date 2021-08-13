/*
 * File:   main.c
 * Author: johannamorim
 *
 * Created on 8 de Agosto de 2021, 21:23
 */




//definições das bibliotecas
//========================
#define F_CPU 16000000        //define a frequencia do uC para 16MHz
#include <avr/io.h>           //Biblioteca geral dos AVR
#include <avr/interrupt.h>    //Biblioteca de interrupção
#include <stdio.h>            //Bilioteca do C
#include <util/delay.h>       //Biblioteca geradora de atraso
//Definições das macros
//====================
#define botao1  PB0
#define botao2  PB1
#define botao3  PB2

//variáveis de comando para os registradores
#define set_bit(y,bit) (y|=(1<<bit)) //coloca em 1 o bit x da variável Y
#define clr_bit(y,bit) (y&=~(1<<bit)) //coloca em 0 o bit x da variável Y
#define cpl_bit(y,bit) (y^=(1<<bit)) //troca o estado lógico do bit x da variável Y
#define tst_bit(y,bit) (y&(1<<bit)) //retorna 0 ou 1 conforme leitura do bit
//==============================================================


//Variáveis globais
//================
int valor = 0;
//Protótipo das funções
//================

ISR(PCINT0_vect) {
    static int flag1 = 0, flag2 = 0, flag3 = 0;

    if (!tst_bit(PINB, botao1)) //botão1 pressionado?
    {
        flag1 = 1;
    }

    if (!tst_bit(PINB, botao2)) //botão2 pressionado?
    {
        flag2 = 1;

    }

    if (tst_bit(PINB, botao1) && flag1)
    {
        flag1 = 0;
        valor = 1;
    }

    if (tst_bit(PINB, botao2) && flag2)
    {
        flag2 = 0;
        valor = 2;
    }
    
    if (!tst_bit(PINB, botao3))     //botao3 pressionado?
    {
        flag3 = 1;
    }

    if (tst_bit(PINB, botao2) && flag3)
    {
        flag3 = 0;
        valor = 0;
    }

}

void main() {
    DDRD = 0x00; //Todo PORTD como saída
    PORTD = 0x00; //inicia todos do PORTD em LOW
    DDRB &= ~(1 << botao1) &~(1 << botao2) &~(1 << botao3); //PORTB como entrada
    PORTB = 0x00; //resistores de pull-up internos desabilitados

    PCICR = 0x01; //Habilita interrupção do PCINT0
    PCMSK0 = 0x07; //Habilita PCINT0 e PCINT1 e PCINT2

    sei();

    while (1) {
        char display [] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07
            , 0x7F, 0x6F, 0x77, 0x7F, 0x39, 0x3F, 0x79, 0x71};

        for (int i = 0; i < 16; i++) {
            switch (valor) {
                case 0:
                    PORTD = display[i];
                    
                case 1: //imprime números primos em hexa  
                    switch (i) {
                        case 2:
                            PORTD = display[2];
                            break;

                        case 3:
                            PORTD = display[3];
                            break;

                        case 5:
                            PORTD = display[5];
                            break;

                        case 7:
                            PORTD = display[7];
                            break;

                        case 11:
                            PORTD = display[11];
                            break;

                        case 13:
                            PORTD = display[13];
                            break;

                    }
                    break;

                case 2://imprime números não primos
                    switch (i) {
                        case 0:
                            PORTD = display[0];
                            break;

                        case 1:
                            PORTD = display[1];
                            break;

                        case 4:
                            PORTD = display[4];
                            break;

                        case 6:
                            PORTD = display[6];
                            break;

                        case 8:
                            PORTD = display[8];
                            break;

                        case 9:
                            PORTD = display[9];
                            break;

                        case 10:
                            PORTD = display[10];
                            break;

                        case 12:
                            PORTD = display[12];
                            break;

                        case 14:
                            PORTD = display[14];
                            break;

                        case 15:
                            PORTD = display[15];
                            break;

                    }


            }
            _delay_ms(500);
        }

        valor = 0;

    }
}
