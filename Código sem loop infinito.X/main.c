/*
 * File:   main.c
 * Author: johan
 *
 * Created on 9 de Junho de 2021, 17:13
 */


#define F_CPU 16000000      //define a frequencia do uC para 16MHz
#include <avr/io.h>         //Biblioteca geral dos AVR
#include <avr/interrupt.h>  //Biblioteca de interrupção
#include <stdio.h>          //Bilioteca do C
#include <util/delay.h>     //Biblioteca geradora de atraso
#include "UART.h"           //Biblioteca da comunicação UART
#include "ADC.h"            //Biblioteca do conversor AD

//Lado direito
#define AIN2 PD6
#define AIN1 PD5 // Quando em HIGH, roda direita anda para frente

//Lado Esquerdo
#define BIN1 PD4 // Quando em HIGH, roda esquerda anda para frente
#define BIN2 PD3

//variáveis de comando para os registradores
#define set_bit(y,bit) (y|=(1<<bit)) //coloca em 1 o bit x da variável Y
#define clr_bit(y,bit) (y&=~(1<<bit)) //coloca em 0 o bit x da variável Y
#define cpl_bit(y,bit) (y^=(1<<bit)) //troca o estado lógico do bit x da variável Y
#define tst_bit(y,bit) (y&(1<<bit)) //retorna 0 ou qualquer valor acima conforme leitura do bit
//PORTB, PB6 = 6 bits

volatile int contador = 0;
char s [] = "Início da leitura";
char buffer[]; //String que armazena valores de entrada para serem printadas
volatile char ch; //armazena o caractere lido
volatile char flag_com = 0; //flag que indica se houve recepção de dado

int error;

int soma_total = 0, soma_esquerdo = 0, soma_direito = 0, denominador_esquerdo = 6, denominador_direito = 6;
int peso [] = {-3, -2, -1, 1, 2, 3};
int erro;

int kp = 2, kd = 1, ki = 1;
int p, d, i, erroAnterior = 0, integral = 0, Turn = 0;
int prescale = 2000;
int u = 0;
unsigned int millis = 0;
char buffer [5];

unsigned int delta_T = 0;
unsigned int tempo_atual = 0, tempo_passado;

void setup();
void loop();
int err();
int PID();
void adress();
void setDuty_1(int duty);
void setDuty_2(int duty);
void setFreq(char option);
void sentido_de_giro();
// Interrupção da UART
//======================================================//

ISR(USART_RX_vect) {
    ch = UDR0; //Faz a leitura do buffer da serial

    UART_enviaCaractere(ch); //Envia o caractere lido para o computador
    flag_com = 1; //Aciona o flag de comunicação
}

/*tempo =65536 ? Prescaler/Fosc = 65536 ? 1024/16000000 = 4, 19s
 tempo = X_bit_timer * Prescaler/Fosc
 Valor inicial de contagem = 256 ? tempo_desejado?Fosc/Prescaler = 256 ? 0,01?16000000/1024 = 98,75 ? 99
 Valor inicial de contagem = X_bit_timer - tempo_desejado*Fosc/Prescaler*/
ISR(TIMER0_OVF_vect) {
    TCNT0 = 255; //Recarrega o Timer 0 para que a contagem seja 1ms novamente
    millis++; //Incrementa a variável millis a cada 1ms

    //cpl_bit(PORTD, PD2);
}

ISR(TIMER1_OVF_vect) {
    TCNT1 = 65224; //Inicializa em 65224 com 20ms
    //PORTD = (le_ADC(0) >> 2);
}

ISR(TIMER2_OVF_vect) {
    TCNT2 = 100; //Inicializa em 100 para um tempo de 10ms
    contador++;
    if (contador == 50) {
        //adress();
        //sentido_de_giro();
        contador = 0;
    }
    /*contador++;
    if (contador == 5) {
        u = PID();
        sprintf(buffer, "%5d\n", u); //Converte para string
        UART_enviaString(buffer); //Envia para o computador
        UART_enviaCaractere(0x0D); //pula linha*/ //leitura do erro
    //contador = 0;
    //}
}

ISR(PCINT0_vect) {

}

int main(void) {
    setup();
    while (1) loop();
}

void setup() {
    DDRD = 0xff;
    PORTD = 0b00101000;


    UART_config(); //Inicializa a comunicação UART
    inicializa_ADC(); //Configura o ADC
    UART_enviaString(s); //Envia um texto para o computador


    //=============Configuração dos timers=========//
    TCCR0B = 0b00000101; //TC0 com prescaler de 1024
    TCNT0 = 255; //Inicia a contagem em 100 para, no final, gerar 1ms
    TIMSK0 = 0b00000001; //habilita a interrupção do TC0

    TCCR1B = 0b00000101; //TC1 com prescaler de 1024 de 200ms
    TCNT1 = 65224; //Inicializa em 65224
    TIMSK1 = 0b00000001; //habilita a interrupção do TC1

    TCCR2B = 0b00000111; //TC2 com prescaler de 1024 com 10ms
    TCNT2 = 100; //Inicializa em 100 para um tempo de 10ms
    TIMSK2 = 0b00000001; //Habilita a interrupção do TC2
    //=================================================//

    //==========configuração das interrupções externas==========//
    PCICR = 0b00000001; //Ativa os PCINT0 - interrupção externa
    PCMSK0 = 0b01111111; //Habilita o PC0 - PC6 como PCINT (PCINT específico)

    //====Configuração do PWM========================//
    TCCR1A = 0xA2; //Configura operação em fast PWM, utilizando registradores OCR1x para comparação



    setFreq(4); //Seleciona opção para frequência

    //============================//

    sei();
}

void loop() {

}

int err() {
    int sensores[] = {le_ADC(0), le_ADC(1), le_ADC(2), le_ADC(3), le_ADC(4), le_ADC(5)};
    for (int j = 0; j < 3; j++) {
        soma_esquerdo += (sensores[j] * peso[j]);
        soma_direito += (sensores[5 - j] * peso[5 - j]);


    }


    soma_total = (soma_esquerdo + soma_direito) / (denominador_esquerdo + denominador_direito);

    erro = 0 - soma_total; //valor esperado(estar sempre em cima da linha) - valor medido


    soma_esquerdo = 0;
    soma_direito = 0;
    soma_total = 0;

    return erro;

}

int PID() {
    tempo_atual = millis;
    delta_T = tempo_atual - tempo_passado;

    if (delta_T >= 50) //a cada 50ms
    {
        error = err();
        p = (error * kp) / prescale; // Proporcao

        integral += error; // Integral
        i = ((ki * integral) / prescale) * delta_T;

        d = ((kd * (error - erroAnterior)) / prescale) / delta_T; // Derivada
        erroAnterior = error;
        tempo_passado = tempo_atual;

        Turn = p + i + d;
    }

    return Turn; //retorna os valores após o PID

}

void adress() {
    clr_bit(PORTD, PD2);
    sprintf(buffer, "%5d\n", le_ADC(0)); //Converte para string
    UART_enviaString(buffer); //Envia para o computador
    UART_enviaCaractere(0x20); //espaço
    set_bit(PORTD, PD2);
    sprintf(buffer, "%5d\n", le_ADC(0)); //Converte para string
    UART_enviaString(buffer); //Envia para o computador
    UART_enviaCaractere(0x20); //espaço
    clr_bit(PORTD, PD2);
    set_bit(PORTD, PD3);
    sprintf(buffer, "%5d\n", le_ADC(0)); //Converte para string
    UART_enviaString(buffer); //Envia para o computador
    UART_enviaCaractere(0x20); //espaço
    clr_bit(PORTD, PD3);
    set_bit(PORTD, PD2);
    set_bit(PORTD, PD3);
    sprintf(buffer, "%5d\n", le_ADC(0)); //Converte para string
    UART_enviaString(buffer); //Envia para o computador
    UART_enviaCaractere(0x20); //espaço
    clr_bit(PORTD, PD2);
    clr_bit(PORTD, PD3);
    set_bit(PORTD, PD4);
    sprintf(buffer, "%5d\n", le_ADC(0)); //Converte para string
    UART_enviaString(buffer); //Envia para o computador
    UART_enviaCaractere(0x0D); //pula linha*/ //leitura do erro
    clr_bit(PORTD, PD4);
}

//==========funções secundárias=======//

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
  
        option  frequency (as frequências no timer 1 são menores do que as frequências nos timers 0 e 2)
        
          1      16    kHz
          2       2    kHz
          3     250     Hz
          4     62,5    Hz
          5     15,6    Hz
     */
    TCCR1B = option;


} //end setFrequency

void sentido_de_giro() {
        PORTD ^= (1<<AIN1) | (1<<AIN2) | (1<<BIN1) | (1<<BIN2);
        setDuty_1(500);
        setDuty_2(500);
}