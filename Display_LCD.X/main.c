/*
 * File:   main.c
 * Author: johan
 *
 * Created on 25 de Maio de 2021, 20:36
 */

#define F_CPU 16000000      //define a frequencia do uC para 16MHz
#include <avr/io.h>         //biblioteca de mapeamento dos registradores
#include <util/delay.h>     //biblioteca que gera atraso
#include <avr/interrupt.h>
#include <avr/pgmspace.h>   //para o uso do PROGMEM, gravação de dados na memória flash
#include "LCD.h"            //biblioteca do LCD
#include "ADC.h"            //biblioteca do conversor AD
#include <stdio.h> //biblioteca para o uso da função sprintf

//Definições de macros para trabalho com bits
#define	set_bit(y,bit)	(y|=(1<<bit))	//coloca em 1 o bit x da variável Y
#define	clr_bit(y,bit)	(y&=~(1<<bit))	//coloca em 0 o bit x da variável Y
#define cpl_bit(y,bit) 	(y^=(1<<bit))	//troca o estado lógico do bit x da variável Y
#define tst_bit(y,bit) 	(y&(1<<bit))	//retorna 0 ou 1 conforme leitura do bit

//definição para acessar a memória flash
PROGMEM const char flash_msg[] = "FLASH - IFSP"; //mensagem armazenada na memória flash
char ram_msg[] = "RAM - Ola mundo";
//----------------------------------------------------------------------------//

int aula01() {
    DDRD = 0b11111111; //PORTD como saída
    DDRB = 0b00101000; //PB5 e PB3 como saída

    inic_LCD_4bits(); //iniciliza o LCD
    escreve_LCD(ram_msg); //string armazenada na RAM
    cmd_LCD(0xc0, 0); //desloca cursor para a segunda linha
    escreve_LCD_Flash(flash_msg); //string armazenada no flash
    while (1) {
    }
}

//definição para acessar a memória RAM
char msg1[] = "E vai           ";
char msg2[] = "         e volta";

int aula02() {
    DDRD = 0b11111111; //PORTD como saída
    DDRB = 0b00101000; //PB5 e PB3 como saída

    inic_LCD_4bits(); //iniciliza o LCD

    while (1) {
        cmd_LCD(0x01, 0); //apaga o display
        escreve_LCD(msg1); //escreve a msg1
        for (int i = 0; i < 12; i++) {
            _delay_ms(150);
            cmd_LCD(0x18, 0); //desloca a msg para a direita
        }
        cmd_LCD(0x01, 0); //apaga o display
        escreve_LCD(msg2); //escreve msg2
        for (int i = 0; i < 10; i++) {
            _delay_ms(150);
            cmd_LCD(0x1C, 0); //Desloca a msg para a esquerda - olhar datasheet
        }
    }
}


char msg3[] = "CONTAGEM = ";
char buffer[4] = " "; //Buffer para armazenamento da variável convertida
//--------------------------------------------------------------------------------------

int aula03() {
    DDRD = 0b11111111; //PORTD inteiro como saída
    DDRB = 0b00101000; //PB3 e PB5 como saída

    inic_LCD_4bits(); //inicializa o LCD
    escreve_LCD(msg3); //string armazenada na RAM

    while (1) {
        for (int i = 0; i < 999; i++) {
            _delay_ms(500);
            sprintf(buffer, "%.3d", i); //Converte a variável em uma string
            cmd_LCD(0x8B, 0); //Retorna o cursor para o início do número
            escreve_LCD(buffer); //Escreve a variável no display
        }
    }
}

int aula04() {

    DDRD = 0b11111111; //PORTD inteiro como saída
    DDRB = 0b00101000; //PB3 e PB5 como saída

    float i = 0;

    i = 3.15;

    inic_LCD_4bits(); //inicializa o LCD

    sprintf(buffer, "%.3f", i); //Converte a variável em uma string
    cmd_LCD(0x8b, 0); //Retorna o curso para o início do caracter
    escreve_LCD(buffer); //Escreve a variável no display

    while (1) {

    }
}

void aula05() {
    DDRD = 0b11111111; //PORTD inteiro como saída
    DDRB = 0b00101000; //PB3 e PB5 como saída

    DDRC = 0b00000000; //PORTC definido como saída
    PORTC = 0b00001100; //PC2 e PC3 com pull-up ativado

    char inicio[] = "Display On!";

    char botao_pressionado[] = "Botao on";

    char botao_solto[] = "Botao off";

    int flag1 = 1;
    int flag2 = 1;

    inic_LCD_4bits();
    escreve_LCD(inicio);

    _delay_ms(200);



    while (1) {

        if ((!(tst_bit(PINC, PC2))) && tst_bit(PINC, PC3) && flag1 == 1) {
            cmd_LCD(0x01, 0); //apaga no display e retorna o cursor
            escreve_LCD(botao_pressionado);
            cmd_LCD(0xc0, 0); //vai pra linha de baixo
            escreve_LCD(botao_solto);
            _delay_ms(20);
            flag1 = 0;
        } else if ((!(tst_bit(PINC, PC3))) && tst_bit(PINC, PC2) && flag2 == 1) {
            cmd_LCD(0x01, 0);
            escreve_LCD(botao_solto);
            cmd_LCD(0xc0, 0);
            escreve_LCD(botao_pressionado);
            _delay_ms(20);
            flag2 = 0;
        } else if (tst_bit(PINC, PC3) && tst_bit(PINC, PC2) && (flag1 == 0 || flag2 == 0)) {
            cmd_LCD(0x01, 0);
            escreve_LCD(botao_solto);
            cmd_LCD(0xc0, 0);
            escreve_LCD(botao_solto);
            _delay_ms(20);
            flag1 = 1;
            flag2 = 1;
        }







    }
}

void aula06() {
    DDRD = 0b11111111; //PORTD inteiro como saída
    DDRB = 0b00101000; //PB3 e PB5 como saída

    DDRC = 0b00000000; //PORTC definido como saída
    PORTC = 0b00001100; //PC2 e PC3 com pull-up ativado

    char inicio[] = "Display On!";

    char msg [] = "CONTAGEM:";


    int flag1 = 0;
    int flag2 = 0;

    int contador = 0;

    inic_LCD_4bits();
    escreve_LCD(inicio);

    _delay_ms(200);
    cmd_LCD(0x01, 0); //limpa o cursor

    escreve_LCD(msg);

    while (1) {

        if (!(tst_bit(PINC, PC2))) {
            _delay_ms(10);
            flag1 = 1;
            flag2 = 0;
        } else if (tst_bit(PINC, PC2) && flag1 == 1 && tst_bit(PINC, PC3)) {
            cmd_LCD(0xc6, 0);
            _delay_ms(500);
            contador++;
            if (contador > 999) {
                contador = 0;
            } else if (contador == 0) {
                cmd_LCD(0x01, 0);
                escreve_LCD(msg);
                cmd_LCD(0xc6, 0);
            }
            sprintf(buffer, "%.3d", contador); //Converte a variável em uma string
            escreve_LCD(buffer); //Escreve a variável no display
            cmd_LCD(0x8B, 0);
        } else if (!(tst_bit(PINC, PC3))) {
            _delay_ms(10);
            flag2 = 1;
            flag1 = 0;
        } else if (tst_bit(PINC, PC3) && flag2 == 1 && tst_bit(PINC, PC2)) {
            cmd_LCD(0xc6, 0);
            _delay_ms(500);
            contador--;
            /*if (contador < 1) {
                contador = 999;
            }*/

            sprintf(buffer, "%.3d", contador); //Converte a variável em uma string
            escreve_LCD(buffer); //Escreve a variável no display
            cmd_LCD(0x8B, 0);

        }
    }
}

volatile int flag1 = 0;
volatile int flag2 = 0;
int contador = 0;

void aula07() {
    DDRD = 0b11111111; //PORTD inteiro como saída
    DDRB = 0b00101000; //PB2, PB3 e PB5 como saída

    DDRC = 0b00000000; //PORTC definido como saída
    PORTC = 0b00001100; //PC2 e PC3 com pull-up ativado


    PCICR = 0b00000010; //Ativa o PCINT1 Geral - interrupção externa
    PCMSK1 = 0b00001100; //Habilita o PC2 e PC3 como PCINT (PCINT específico)

    char inicio[] = "Display On!";

    char msg [] = "CONTAGEM:";



    sei();

    inic_LCD_4bits();
    escreve_LCD(inicio);

    _delay_ms(200);
    cmd_LCD(0x01, 0); //limpa o cursor

    escreve_LCD(msg);



    while (1) {
        if (tst_bit(PINC, PC3) && flag2 == 1) {
            cmd_LCD(0xc6, 0);
            _delay_ms(500);
            contador--;
            if (contador < 1) {
                contador = 999;
            }

            sprintf(buffer, "%.3d", contador); //Converte a variável em uma string
            escreve_LCD(buffer); //Escreve a variável no display
            cmd_LCD(0x8B, 0);
        } else if (tst_bit(PINC, PC2) && flag1 == 1) {
            cmd_LCD(0xc6, 0);
            _delay_ms(500);
            contador++;
            if (contador > 999) {
                contador = 0;
            } else if (contador == 0) {
                cmd_LCD(0x01, 0);
                escreve_LCD(msg);
                cmd_LCD(0xc6, 0);
            }
            sprintf(buffer, "%.3d", contador); //Converte a variável em uma string
            escreve_LCD(buffer); //Escreve a variável no display
            cmd_LCD(0x8B, 0);
        }

    }

}

ISR(PCINT1_vect) {

    if (!(tst_bit(PINC, PC3))) {
        _delay_ms(10);
        flag2 = 1;
        flag1 = 0;
    } else if (!(tst_bit(PINC, PC2))) {
        _delay_ms(10);
        flag1 = 1;
        flag2 = 0;
    }

}

void aula08() {
    char msg1[] = "CANAL 0 = ";
    char buffer[5] = " ";

    int main() {
        unsigned int valor = 0;
        DDRD = 0b11111111; //PORTD inteiro como saída
        DDRB = 0b00101000; //PB3 e PB5 como saída (pinos do LCD)
        
        inicializa_ADC(); //inicializa o ADC
        inic_LCD_4bits(); //inicializa o LCD
        escreve_LCD(msg1); //Escreve a mensagem na linha 1 do LCD
        
        while (1) {
            valor = le_ADC(0); //Faz a leitura do canal 0 do ADC
            _delay_ms(250);
            sprintf(buffer, "%.4d", valor); //Converte a variável valor em uma string
            cmd_LCD(0x8A, 0); //Retorna o cursor para o início da linha 1
            escreve_LCD(buffer); //Escreve a variável no display
        }
    }
}

void main() {
    //aula01();
    //aula02();
    //aula03();
    //aula04();
    //aula05();
    //aula06();
    aula07();
    //aula08();
}

