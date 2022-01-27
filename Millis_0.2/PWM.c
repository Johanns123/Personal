#include "PWM.h"


//PWM em fast mode de 10 bits do Timer 1 do atemga328p

void PWM_setDuty_2(int duty) //MotorB
{

    OCR1B = duty; //registrador de PWM do OC1B

} //end setDuty

void PWM_setDuty_1(int duty) //MotorA
{

    OCR1A = duty; //valores de 0 - 1023

} //end setDuty

void PWM_setFreq(char option) 
{
    /*
    TABLE:
  	//no fast Mode
        option  frequency (as frequ�ncias no timer 1 s�o menores do que as frequ�ncias nos timers 0 e 2)
        
          1      16    kHz
          2       2    kHz
          3     250     Hz
          4     62,5    Hz
          5     15,6    Hz
     */
    TCCR1B = option;
	
	
	
    /*
    TABLE:
  	//no phase corret PWM Mode
        option  frequency (as frequ�ncias no timer 1 s�o menores do que as frequ�ncias nos timers 0 e 2)
        
          1       8     kHz
          2       1     kHz
          3     125      Hz
          4     31,25    Hz
          5     7,8      Hz
     */
     
} //end setFrequency


void PWM_limit()
{
    /*Vari�veis externas*/
    extern unsigned int PWMA, PWMB;
    
    //------> Limitando PWM
    static int ExcessoB = 0, ExcessoA = 0;
    
    if (PWMA > 1023)
    {
      ExcessoB = (PWMA - 1023);
      PWMA = 1023;
      PWMB -= ExcessoB;
    }

    else if (PWMB > 1023)
    {
      ExcessoA = (PWMB - 1023);
      PWMB = 1023;
      PWMA -= ExcessoA;
    }

    if (PWMA < 0)
    {
      //ExcessoB = (PWMA*(-1) * 2);
      ExcessoB = (PWMA << 1);
      //PWMA += (PWMA*(-1)*2);
      PWMA -= ExcessoB;
      PWMB -= ExcessoB;
    }

    else if (PWMB < 0)
    {
      //ExcessoA = (PWMB*(-1) * 2);
      ExcessoA = (PWMB << 1);
      //PWMB += (PWMB*(-1)*2);
      PWMB -= ExcessoA;
      PWMA -= ExcessoA;
    }        

}

unsigned int PWM_calc_pwm(unsigned char duty_cycle)
{
    int pwm;
    
    extern unsigned char vect_duty_cycle[101];
    
	if(duty_cycle > 100) duty_cycle = 100;
    
    pwm = vect_duty_cycle[duty_cycle];
			
	return pwm;
	
} /* end calc_pwm */

unsigned char PWM_calc_duty(unsigned int pwm)
{	/*Desenvolvimento*/
    char duty_cycle;
   
    extern unsigned int vect_PWM[512];
    
    
	if(pwm > 511) pwm = 511;

    duty_cycle = vect_PWM[pwm];
		
    return duty_cycle;
    
} /* end calc_duty */
