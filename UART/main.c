/*
 * File:   main.c
 * Author: Luis Alexandre
 *
 * Created on February 8, 2021, 2:54 PM
 */


#include "configbit.txt" //configurações dos bits
#include <xc.h>
#include <pic18F4550.h>
#include <stdio.h>
//#include "serial.h"

#define _XTAL_FREQ 20000000 //Frequencia em 20MHz

char uart_rd;

void main(void) {
    
    //serial_init(void);; //inicializa o serial com 9600 de baud rate
    __delay_ms(100);
    
    /*serial_tx_str('Start');
    serial_tx(10);
    serial_tx(13);
    
    
    while(1){
        if(serial_rx()){
            uart_rd = serial_rx();
            serial_tx(uart_rd);
        
        }
    
    }*/
    
    
    
}
