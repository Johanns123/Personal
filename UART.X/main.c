/*
 * File:   main.c
 * Author: Luis Alexandre
 *
 * Created on February 8, 2021, 2:54 PM
 */

//código de leitura dos sensores frontais da forma mais rudementar possível

#include "configbit.txt" //configurações dos bits
#include <xc.h>
#include <stdio.h>
#include "serial.h"
#include <stdlib.h>
#include <string.h>
//#include <pic18F4550.h>

#define _XTAL_FREQ 20000000 //Frequencia em 20MHz


#define led1 RD5
#define led2 RD6
#define led3 RD7
#define botao RC0

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

char uart_rd;
int leitura1 = 0;
int leitura2 = 0;
int leitura3 = 0;
int leitura4 = 0;
int leitura5 = 0;
int leitura6 = 0;
char txt[7];
char txt2[7]; //variáveis que enviam valores de leitura

int contador = 0;

void main(void) {
    TRISAbits.RA0 = 0x01;
    PORTA = 0x00;
    ADCON1 = 0b00001001;
    ADCON2 = 0b10010101;
    TRISD = 0x00;
    CMCON = 0x07;
    TRISCbits.RC0 = 0x01;
    
    serial_init(); //inicializa o serial com 9600 de baud rate
    __delay_ms(100);
    
    serial_tx_str("Start");
    serial_tx(10);
    serial_tx(13);      //quebra de linha
    
    
    while(1){
        
        //-----leitura utilizando vetores-------//
        leitura1 = conversao_AD1();
        leitura2 = conversao_AD2();
        leitura3 = conversao_AD3();
        leitura4 = conversao_AD4();
        leitura5 = conversao_AD5();
        leitura6 = conversao_AD6();
        
        int vetor_leituras [] = {leitura1, leitura2, leitura3, leitura4, leitura5, leitura6};
        
        for(int i = 0; i < 6; i++){
            serial_tx_str("sensor");
            sprintf(txt2, "%i", (contador + 1));
            serial_tx_str(txt2);
            serial_tx_str(":");
            serial_tx(32);       //gera espaço
            sprintf(txt, "%i", vetor_leituras[i]); //converter de inteiro para string a leitura do sensor na posição i
            serial_tx_str(txt);
            serial_tx(32);       //gera espaço
            contador++;
        }
        serial_tx(13); //quebra de linha
        contador = 0;
        __delay_ms(100);
        
        
        //----------Outra forma de leitura--------//
        /*sprintf(txt,"%i", leitura1);  //converte inteiro para string
        sprintf(txt2,"%i", leitura2);
        sprintf(txt3,"%i", leitura3);
        sprintf(txt4,"%i", leitura4);  //converte inteiro para string
        sprintf(txt5,"%i", leitura5);  //converte inteiro para string
        sprintf(txt6,"%i", leitura6);  //converte inteiro para string

        serial_tx_str(txt);  //imprime na tela a string do valor de 0 1023
        serial_tx(32);       //gera espaço
        serial_tx_str(txt2);
        serial_tx(32);       //gera espaço entre as leituras para que as leituras sejam expressas como se fossem em paralelas
        serial_tx_str(txt3);
        serial_tx(32); 
        serial_tx_str(txt4);
        serial_tx(32); 
        serial_tx_str(txt5);
        serial_tx(32); 
        serial_tx_str(txt6);
        serial_tx(13);
        __delay_ms(300);*/
        
        
        
        
        //Área em que o terminal envia um comando para o PIC,
        //sugestão de utilizar os códigos em situações diferentes
        uart_rd = serial_rx(100);
        if (uart_rd != 0xA5) serial_tx(uart_rd);
        switch(uart_rd){
            case 'a':
                serial_tx(13);
                serial_tx_str("Led1 piscando");
                serial_tx(13); //quebra de linha
                led1 = 1;
                __delay_ms(500);
                led1 = 0;
                __delay_ms(500);
                break;
            
            case 'b':
                serial_tx(13);
                serial_tx_str("Led2 piscando");
                serial_tx(13);
                led2 = 1;
                __delay_ms(500);
                led2 = 0;
                __delay_ms(500);
                break;
                
            case 'c':
                serial_tx(13);
                serial_tx_str("Led3 piscando");
                serial_tx(13);
                led3 = 1;
                __delay_ms(500);
                led3 = 0;
                __delay_ms(500);
                break;
            
            default:
                if (uart_rd != 0xA5){
                    serial_tx(13);
                    serial_tx_str("Comando não implementado");
                    serial_tx(10);
                    serial_tx(13);
                }
                break;
        }
        
        if(botao == 0){
            serial_tx_str("Botão pressionado");
            serial_tx(10);
            serial_tx(13);
        }
        
    }
    
    
    
}
