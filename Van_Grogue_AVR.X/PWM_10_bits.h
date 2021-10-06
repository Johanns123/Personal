//PWM em fast mode de 10 bits do Timer 1 do atemga328p

void setDuty_2(int duty) //MotorB
{

    OCR1B = duty; //registrador de PWM do OC1B

} //end setDuty

void setDuty_1(int duty) //MotorA
{

    OCR1A = duty; //valores de 0 - 1023

} //end setDuty

void setFreq(char option) {
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
