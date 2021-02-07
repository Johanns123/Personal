/*
 * File:   main.c
 * Author: Luis Alexandre
 *
 * Created on 6 de Fevereiro de 2021, 16:44
 */
#include "configbit.txt" //configurações dos bits
#include <xc.h>
#include <pic18F4550.h>
#include <stdio.h>
#define _XTAL_FREQ 20000000 //Frequencia em 20MHz


//código que simularia a leitura dos sensores frontais do seguidor


/*Informações do datasheet

TACQmin = 2.45us
TDAmin = 0.8us

Informações de montagem

FOSC = 20MHz
TOSC = 1/FOSC = 50ns

ADCS = 0.8u / 50n = 16s --> 16

TDA = 16 * TOSC = 0.800us

ACQT = 2.45u / 0.8u = 3.07us --> 4

TACQ = 4 * TDA = 3.2us

tempo de conv = 11*TDA + TACQ = 12us*/

//esse código faz a leitura de 6 entradas analógicas

#define led1 RD7
#define led2 RD6
#define led3 RD5
#define led4 RD4
#define led5 RD3
#define led6 RD2
#define led7 RD1
#define led8 RD0

int leitura1 = 0;
int leitura2 = 0;
int leitura3 = 0;
int leitura4 = 0;
int leitura5 = 0;
int leitura6 = 0;

int conversao_AD1(void){
    int conversao;
    ADCON0 = 0b00000001;// Seleciona canal AN0
    ADCON0bits.GO=1; // inicia a conversão
    while (ADCON0bits.GO); // Aguarda a o fim da conversão
    conversao = (ADRESL + (ADRESH << 8));
    return conversao;
    
}

int conversao_AD2(void){
    int conversao2;
    ADCON0 = 0b00000101;// Seleciona canal AN1
    ADCON0bits.GO=1; // inicia a conversão
    while (ADCON0bits.GO); // Aguarda a o fim da conversão
    conversao2 = ADRESL + (ADRESH << 8);
    return conversao2;
}

int conversao_AD3(void){
    int conversao3;
    ADCON0 = 0b00001001;// Seleciona canal AN2
    ADCON0bits.GO=1; // inicia a conversão
    while (ADCON0bits.GO); // Aguarda a o fim da conversão
    conversao3 = ADRESL + (ADRESH << 8);
    return conversao3;
    
}

int conversao_AD4(void){
    int conversao4;
    ADCON0 = 0b00001101;// Seleciona canal AN3
    ADCON0bits.GO=1; // inicia a conversão
    while (ADCON0bits.GO); // Aguarda a o fim da conversão
    conversao4 = ADRESL + (ADRESH << 8);
    return conversao4;

}

int conversao_AD5(){
    int conversao5;
    ADCON0 = 0b00010001;// Seleciona canal AN4
    ADCON0bits.GO=1; // inicia a conversão
    while (ADCON0bits.GO); // Aguarda a o fim da conversão
    conversao5 = ADRESL + (ADRESH << 8);
    return conversao5;

}

int conversao_AD6(void){
    int conversao6;
    ADCON0 = 0b00010101;// Seleciona canal AN5
    ADCON0bits.GO=1; // inicia a conversão
    while (ADCON0bits.GO); // Aguarda a o fim da conversão
    conversao6 = ADRESL + (ADRESH << 8);
    return conversao6;

}

void main(void) {
    //código de conversão AD usando trimpot
    TRISD = 0x00;
    TRISA = 0b00101111; //determina os pino RA0 até RA3 e RA5 como entrada
    TRISEbits.RE0 = 1;  //determina RE como entrada
    ADCON1 = 0b00001001; // --> 1001 determina somente AN0 até AN5 como analógico
    ADCON2 = 0b10010101; //para obter esses valores recorra ao datasheet e às informações acima
    CMCON = 7; //desabilita os comparadores
    PORTA = 0x00;
    PORTD = 0x00; // todos os leds desligados
    
    while(1){
        
        leitura1 = conversao_AD1(); //pega a função que faz a conversão AD do AN1
        leitura2 = conversao_AD2(); 
        leitura3 = conversao_AD3();
        leitura4 = conversao_AD4();
        leitura5 = conversao_AD5();
        leitura6 = conversao_AD6();
        
        
        if(leitura1 >= 0 && leitura1 <= 200){
            led1 = 1;
        }
        else if(leitura1 > 200){
            led1 = 0;
        }
        
        
        if(leitura2 >= 0 && leitura2 <= 200){
            led2 = 1;
        }
        else if(leitura2 > 200){
            led2 = 0;
        }

        if(leitura3 >= 0 && leitura3 <= 200){
            led3 = 1;
        }
        else if(leitura3 > 200){
            led3 = 0;
        }
        
        
        if(leitura4 >= 0 && leitura4 <= 200){
            led4 = 1;
        }
        else if(leitura4 > 200){
            led4 = 0;
        }
   
        if(leitura5 >= 0 && leitura5 <= 200){
            led5 = 1;
        }
        else if(leitura5 > 200){
            led5 = 0;
        }

        
        if(leitura6 >= 0 && leitura6 <= 200){
            led6 = 1;
        }
        else if(leitura6 > 200){
            led6 = 0;
        }
    }
        __delay_ms(500); //taxa de atualização do ADC*/
}
