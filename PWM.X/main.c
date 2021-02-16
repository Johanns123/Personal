/*
 * File:   main.c
 * Author: Luis Alexandre
 *
 * Created on 15 de Fevereiro de 2021, 19:36
 */


#include "configbit.txt" //configurações dos bits
#include <xc.h>          //compilador da IDE
#include"PWM.h"
#define _XTAL_FREQ 20000000 //Frequencia em 20MHz


unsigned short pwm;
unsigned short pwm2;
 //a cada 0,200271us * 128 * 256 = 6,5536ms --tempo
/*void __interrupt() isr(void)      //vetor de interrupção
{
    if(INTCONbits.TMR0IF){          //Testando se houve estouro no Timer0
        TMR0 = 0x00;                //Reinicia o registrador Timer0

        for(pwm2 = 0; pwm2 < 256; pwm2++){
            CCPR2L = pwm;
            __delay_ms(50);
            if(pwm2 == 256){
                CCPR2L = pwm2;
                __delay_ms(2000);
            }
        }
        
        
        INTCONbits.TMR0IF = 0x00;
    }

}*/

void main(void) {
    
    //PWM configurado a partir do TIMER2    
    CMCON = 0x07;
    /*INTCON2bits.RBPU = 1;
    INTCON2bits.INTEDG0 = 0;
    T0CONbits.T0CS = 0;
    T0CONbits.T0SE = 0;
    T0CONbits.PSA = 0;
    T0CONbits.T0PS2 = 0x06;  //prescaler 1:128
    INTCONbits.GIE = 1;     //habilita interrupção global
    INTCONbits.PEIE = 1;    //habilita interrupção por periféricos
    INTCONbits.T0IE = 1;    //habilita interrupção do timer0
    
    TMR0 = 0x00;*/
    
    //periodo do pwm = (PR2 + 1) * ciclo de máquina * prescaler do timer2
    //periodo = (224+1) * 0,2us * 4 = 0,8192ms
    //frequencia = 1/periodo = 1220,70Hz = 1,22kHz

    /*PR2 = 224;             //Inicializa o registrador de controle do timer 2 em 224
    T2CON = 0x06;           //Liga o timer2 e prescaler 1:16
    CCPR2L = 0x00;          //led iniciará desligado
    CCP2CON = 0x0C;        //habilita o PWM
    */
    
    TRISB = 0x00;
    TRISC = 0x00;       //somente RC0 e RC1 como entrada
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
