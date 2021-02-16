//#include "configbit.txt" //configurações dos bits
#include <xc.h>

#include "PWM.h"

//Funções:

void PWM_Init () {
    //periodo do pwm = (PR2 + 1) * ciclo de máquina * prescaler do timer2
    //periodo = (240+1) * 0,2us * 16 = 0,8192ms
    //frequencia = 1/periodo = 1220,70Hz = 1,29kHz
    
    PR2 = 240;

    T2CKPS1 = 1; 
    T2CKPS0 = 1;   // Timer 2 Prescaler = 16 
    TMR2ON = 1; 
    
    CCP1M3 = 1;
    CCP1M2 = 1;     // Configura o pino CCP1 para o modo PWM
    
    CCP2M3 = 1;
    CCP2M2 = 1;     // Configura o pino CCP2 para o modo PWM
    
}

void PWM1_Set_Duty (unsigned short duty1)  // duty_ratio vai de 0 a 962
{
    
    CCPR1L = duty1 >> 2;
    DC1B1 = duty1 & 2;
    DC1B0 = duty1 & 1; 
    
}

void PWM2_Set_Duty (unsigned short duty2)  // duty_ratio vai de 0 a 962
{
    
    CCPR2L = duty2 >> 2;
    DC2B1 = duty2 & 2;
    DC2B0 = duty2 & 1; 
    
}


