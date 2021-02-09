/*
 * File:   main.c
 * Author: Luis Alexandre
 *
 * Created on February 8, 2021, 2:54 PM
 */


#include "configbit.txt" //configurações dos bits
#include <xc.h>
#include <stdio.h>
#include "serial.h"
//#include <pic18F4550.h>

#define _XTAL_FREQ 20000000 //Frequencia em 20MHz


#define led1 RD5
#define led2 RD6
#define led3 RD7
#define botao RC0

char uart_rd;

void main(void) {
    TRISD = 0x00;
    CMCON = 0x07;
    TRISCbits.RC0 = 0x01;
    
    serial_init(); //inicializa o serial com 9600 de baud rate
    __delay_ms(100);
    
    serial_tx_str("Start");
    serial_tx(10);
    serial_tx(13);
    
    
    while(1){

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
