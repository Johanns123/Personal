/*
 * File:   main.c
 * Author: johan
 *
 * Created on 29 de Maio de 2021, 10:11
 */


#define F_CPU 16000000      //define a frequencia do uC para 16MHz
#include <avr/io.h>         //biblioteca de mapeamento dos registradores
#include <util/delay.h>     //biblioteca que gera atraso
#include <avr/interrupt.h>
#include "LCD.h"            //biblioteca do LCD
#include "ADC.h"            //biblioteca do conversor AD
#include <stdio.h> //biblioteca para o uso da função sprintf
volatile unsigned int resultado_AD;

ISR(ADC_vect) {
    resultado_AD = ADC;
}

void aula01() {
    DDRD = 0b11111111; //PORTD inteiro como saída
    //Habilita a referência de tensão interna do ADC e ativa o canal 0
    //ADMUX = (1<<REFS0); //Outra maneira de manipular o registrador
    ADMUX = 0b01000000;
    //Habilita o ADC e configura o prescaler para 128
    //ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
    ADCSRA |= 0b10001111; //habilita o ADIE para gerar interrupção

    sei();
    while (1) {
        //ADCSRA |= (1<<ADSC);
        ADCSRA |= 0b01000000; //Inicia a conversão
        PORTD = resultado_AD >> 2;
    }
}

void aula02() {
    char msg1[] = "CANAL 0 = ";
    char buffer1[1] = " ";
    char buffer2[1] = ".";
    char buffer3[1] = " ";
    char buffer4[1] = " ";
    char buffer5[1] = "V";
    int conversao;
    int centena, dezena, unidade;
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
            conversao = (valor * 500)/1023;
            centena = conversao / 100;
            dezena = (conversao % 100) / 10;
            unidade = (conversao/10) %10;
            sprintf(buffer1, "%d", centena); //Converte a variável valor em uma string
            sprintf(buffer3, "%d", dezena);
            sprintf(buffer4, "%d", unidade);
            cmd_LCD(0x8A, 0); //Retorna o cursor para o início da linha 1
            escreve_LCD(buffer1); //Escreve a variável no display
            escreve_LCD(buffer2); //Escreve a variável no display
            escreve_LCD(buffer3); //Escreve a variável no display
            escreve_LCD(buffer4); //Escreve a variável no display
            escreve_LCD(buffer5); //Escreve a variável no display
        }
    }
}

int main() {
    //aula01();
    aula02();
}