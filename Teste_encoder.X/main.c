#define F_CPU 16000000      //define a frequencia do uC para 16MHz
#include <avr/io.h>         //Biblioteca geral dos AVR
#include <avr/interrupt.h>  //Biblioteca de interrup��o
#include <stdio.h>          //Bilioteca do C
#include <util/delay.h>     //Biblioteca geradora de atraso
#include "UART.h"           //Biblioteca da comunica��o UART
#include "ADC.h"            //Biblioteca do conversor AD


#define    encoder_C1   PD2                     //Conex�o C1 do encoder
#define    encoder_C2   PD4                     //Conex�o C2 do encoder
#define    motor1       PD6
#define    motor2       PD7                     // Quando em HIGH, roda direita anda para frente
#define    pot          PC0                    //Entrada para leitura de potenci�metro

//vari�veis de comando para os registradores
#define set_bit(y,bit) (y|=(1<<bit)) //coloca em 1 o bit x da vari�vel Y
#define clr_bit(y,bit) (y&=~(1<<bit)) //coloca em 0 o bit x da vari�vel Y
#define cpl_bit(y,bit) (y^=(1<<bit)) //troca o estado l�gico do bit x da vari�vel Y
#define tst_bit(y,bit) (y&(1<<bit)) //retorna 0 ou qualquer valor acima conforme leitura do bit
//PORTB, PB6 = 6 bits

volatile int contador = 0;
char s [] = "In�cio da leitura";
char buffer[]; //String que armazena valores de entrada para serem printadas
volatile char ch; //armazena o caractere lido
volatile char flag_com = 0; //flag que indica se houve recep��o de dado
char buffer [5];
char teste [] = "Palavra";

// ========================================================================================================
// --- Vari�veis Globais ---
void motor_control(); //Fun��o para controle do motor


// ========================================================================================================
// --- Vari�veis Globais ---
int Encoder_C1Last;
int pulse_number = 0, adc, pwm_value = 128;

int direction_m;


// ========================================================================================================
// --- Vari�veis Globais ---



// ========================================================================================================


void setup();
void loop();
void setDuty_1(int duty);
void setDuty_2(int duty);
void setFreq(char option);
void count_pulses();
// Interrup��o da UART
//======================================================//

ISR(USART_RX_vect) {
    ch = UDR0; //Faz a leitura do buffer da serial

    UART_enviaCaractere(ch); //Envia o caractere lido para o computador
    flag_com = 1; //Aciona o flag de comunica��o
}

/*tempo =65536 ? Prescaler/Fosc = 65536 ? 1024/16000000 = 4, 19s
 tempo = X_bit_timer * Prescaler/Fosc
 Valor inicial de contagem = 256 ? tempo_desejado?Fosc/Prescaler = 256 ? 0,01?16000000/1024 = 98,75 ? 99
 Valor inicial de contagem = X_bit_timer - tempo_desejado*Fosc/Prescaler*/
ISR(TIMER0_OVF_vect) {
    TCNT0 = 255; //Recarrega o Timer 0 para que a contagem seja 1ms novamente
}

ISR(TIMER1_OVF_vect) {
    TCNT1 = 65224; //Inicializa em 65224 com 20ms
}

ISR(TIMER2_OVF_vect) {
    TCNT2 = 100; //Inicializa em 100 para um tempo de 10ms
    contador++;

}

ISR(INT0_vect) {
    count_pulses();    
}

int main(void) {
    setup();
    while (1) loop();
}

void setup() {
    DDRD = 0b11000010;
    PORTD = 0b00100000;

    UART_config(); //Inicializa a comunica��o UART
    inicializa_ADC(); //Configura o ADC
    UART_enviaString(s); //Envia um texto para o computador


    //=============Configura��o dos timers=========//
    TCCR0B = 0b00000101; //TC0 com prescaler de 1024
    TCNT0 = 255; //Inicia a contagem em 100 para, no final, gerar 1ms
    TIMSK0 = 0b00000001; //habilita a interrup��o do TC0

    TCCR1B = 0b00000101; //TC1 com prescaler de 1024 de 200ms
    TCNT1 = 65224; //Inicializa em 65224
    TIMSK1 = 0b00000001; //habilita a interrup��o do TC1

    TCCR2B = 0b00000111; //TC2 com prescaler de 1024 com 10ms
    TCNT2 = 100; //Inicializa em 100 para um tempo de 10ms
    TIMSK2 = 0b00000001; //Habilita a interrup��o do TC2
    //=================================================//

    //==========configura��o das interrup��es externas==========//
    //PCICR = 0b00000001; //Ativa os PCINT0 - interrup��o externa
    //PCMSK0 = 0b01111111; //Habilita o PC0 - PC6 como PCINT (PCINT espec�fico)

    EICRA = 0x03;
    EIMSK = 0x01; //habilita INT0
    //====Configura��o do PWM========================//
    TCCR1A = 0xA2; //Configura opera��o em fast PWM, utilizando registradores OCR1x para compara��o



    setFreq(4); //Seleciona op��o para frequ�ncia

    //============================//

    sei();
}

void loop() {

    motor_control();
    _delay_ms(100);
}

void motor_control() {
    sprintf(buffer, "%4d", pulse_number);
    UART_enviaString(buffer);
    UART_enviaCaractere(0x0D); //pula linha

    adc = le_ADC(0);

    if (adc >= 512) {
        clr_bit(PORTD, motor1);
        set_bit(PORTD, motor2);
        pwm_value = (adc >> 2);
        setDuty_2(pwm_value);
    }
    else {
        set_bit(PORTD, motor1);
        clr_bit(PORTD, motor2);
        pwm_value = 511 - (adc >> 2);
        setDuty_2(pwm_value);
    }

    pulse_number = 0x00;
}

void count_pulses() {
    int Lstate = tst_bit(PIND,encoder_C1); //l� estado do encoder_C1 e armazena em Lstate

    //if (Encoder_C1Last != Lstate) //Encoder_C1Last igual a zero e Lstate diferente de zero
    //{
        int val = tst_bit(PIND,encoder_C2); //L� estado de encoder_C2 e armazena na vari�vel val

        if((Lstate != val) && direction_m)      direction_m = 1;    //sentido reverso
        
        else if((Lstate == val) && !direction_m);  direction_m = 0;    //sentido direto
    //}
    
    Encoder_C1Last = Lstate;    //Encoder_C1Last recebe o valor antigo
    
    if(!direction_m)    pulse_number++;
    else                pulse_number--;
    
}

void setDuty_1(int duty) //MotorA
{

    OCR1B = duty;

} //end setDuty_pin3

void setDuty_2(int duty) //MotorB
{

    OCR1A = duty; //valores de 0 - 1023

} //end setDuty_pin3

void setFreq(char option) {
    /*
    TABLE:
  
        option  frequency (as frequ�ncias no timer 1 s�o menores do que as frequ�ncias nos timers 0 e 2)
        
          1      16    kHz
          2       2    kHz
          3     250     Hz
          4     62,5    Hz
          5     15,6    Hz
     */
    TCCR1B = option;


} //end setFrequency