#include <avr/io.h>

void PWM_setDuty_2(int  duty);
void PWM_setDuty_1(int  duty);
void PWM_setFreq  (char option);

void PWM_limit();

unsigned char PWM_calc_duty(unsigned int  pwm);
unsigned int  PWM_calc_pwm (unsigned char duty_cycle);