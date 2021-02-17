/*
 * File:   main.c
 * Author: Johann Amorim
 *
 * Created on 15 de Fevereiro de 2021, 19:36
 */


#include "configbit.txt" //configurações dos bits
#include <xc.h>          //compilador da IDE
#include"PWM.h"
#define _XTAL_FREQ 20000000 //Frequencia em 20MHz


unsigned short pwm;
unsigned short pwm2;

void main(void) {
    
    //PWM configurado a partir do TIMER2    
    CMCON = 0x07;

    TRISB = 0x00;
    TRISC = 0x00;       
    PORTC = 0x00;
    PORTB = 0x00;

    
    PWM_Init();        //configura pwm em 1,388KHz
    
    pwm = 481;              //valor de duty cycle positivo 50%
    pwm2 = 481;
    PWM1_Set_Duty(pwm);
    PWM2_Set_Duty(pwm2);

    while(1){
        
        PWM1_Set_Duty(pwm);
        PWM2_Set_Duty(pwm2);
        
        __delay_ms(100);
        
        PWM1_Set_Duty(900);
        PWM2_Set_Duty(900);

    }
}
