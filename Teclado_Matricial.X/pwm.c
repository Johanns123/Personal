#include <xc.h>

#include"pwm.h"


void PWM_Init () {
    //periodo do pwm = (PR2 + 1) * ciclo de máquina * prescaler do timer2
    //periodo = (240+1) * 0,2us * 16 = 0,8192ms
    //frequencia = 1/periodo = 1220,70Hz = 1,29kHz
    
    
    //2ms de período
    //1us de colck machine
    //500Hz a 4MHz de clock
    PR2 = 124;

    T2CONbits.T2CKPS1 = 1; 
    T2CONbits.T2CKPS0 = 1;   // Timer 2 Prescaler = 16 
    T2CONbits.TMR2ON = 1; 
    
    CCP1CONbits.CCP1M3 = 1;
    CCP1CONbits.CCP1M2 = 1;     // Configura o pino CCP1 para o modo PWM
    
    CCP2CONbits.CCP2M3= 1;
    CCP2CONbits.CCP2M2 = 1;     // Configura o pino CCP2 para o modo PWM
    
}

void PWM1_Set_Duty (unsigned short pwm1)  // duty_ratio vai de 0 a 962
{
    
    CCPR1L = pwm1 >> 2;
    CCP1CONbits.DC1B1 = pwm1>>8;
    CCP1CONbits.DC1B0 = pwm1>>9; 
    
}

void PWM2_Set_Duty (unsigned short pwm2)  // duty_ratio vai de 0 a 962
{
    
    CCPR2L = pwm2 >> 2;
    CCP2CONbits.DC2B1 = pwm2>>8;
    CCP2CONbits.DC2B0 = pwm2>>9; 
    
}