/*
 * File:   main.c
 * Author: johan
 *
 * Created on 30 de Maio de 2021, 16:52
 */


#define F_CPU 16000000      //define a frequencia do uC para 16MHz
#include <avr/io.h>         //Biblioteca geral dos AVR
#include <avr/interrupt.h>  //Biblioteca de interrupção
#include <stdio.h>          //Bilioteca do C
#include <util/delay.h>     //Biblioteca geradora de atraso
#include "UART.h"           //Biblioteca da comunicação UART
#include "ADC.h"            //Biblioteca do conversor AD
//#include "configbits.txt"   //configura os fusíveis - está dando problema no avrdude

//variáveis de comando para os registradores
#define set_bit(y,bit) (y|=(1<<bit)) //coloca em 1 o bit x da variável Y
#define clr_bit(y,bit) (y&=~(1<<bit)) //coloca em 0 o bit x da variável Y
#define cpl_bit(y,bit) (y^=(1<<bit)) //troca o estado lógico do bit x da variável Y
#define tst_bit(y,bit) (y&(1<<bit)) //retorna 0 ou qualquer valor acima conforme leitura do bit
//PORTB, PB6 = 6 bits

//Lado direito
#define AIN2 PD6// Quando em HIGH, roda direita anda para frente
#define AIN1 PD5 

//Lado Esquerdo
#define BIN1 PD4 // Quando em HIGH, roda esquerda anda para frente
#define BIN2 PD3

int Kp = 2; //prescale de 100 - prescale int
int Kd = 1; //prescale de 100 - prescale int
int Ki = 1; // Variáveis que são modificadas no PID - prescale de 100 ou mais
int PWMR = 400; // valor da força do motor em linha reta
int PWM_Curva = 350; //PWM ao entrar na curva
int erro, p, d, erroAnterior = 0, i, integral = 0, Turn = 0; //Área PID -erro também possui prescale = 100
int u = 0; //valor de retorno do PID
int u_curva = 0;
int prescale = 2000; //prescale das constantes * prescale do erro
int PWMA, PWMB; // Modulação de largura de pulso enviada pelo PID
int PWMA_C, PWMB_C; //PWM de curva com ajuste do PID
int entrou;
int contador = 0, acionador = 0; // Borda
int marcadores = 6;
int erroAnterior_curva = 0;
int Turn_curva, p_curva, d_curva, i_curva, integral_curva = 0;

unsigned int delta_T = 0;
unsigned int tempo_atual = 0, tempo_passado;
int peso [] = {-3, -2, -1, 1, 2, 3};
int soma_direito = 0, soma_esquerdo = 0;
int denominador_direito = 6;
int denominador_esquerdo = 6;
int soma_total = 0;

int valor_max [] = {1023, 1023, 1023, 1023, 1023, 1023};
int valor_min [] = {0, 0, 0, 0, 0, 0};
int valor_min_abs = 0, valor_max_abs = 1023;


char s [] = "Início da leitura";
char buffer[5]; //String que armazena valores de entrada para serem printadas
volatile char ch; //armazena o caractere lido
volatile char flag_com = 0; //flag que indica se houve recepção de dado

volatile unsigned int millis = 0;


//------------------------------------------------------

void setDuty_1(int duty); //Seleciona o duty cycle na saída digital  3
void setFreq(char option); //Seleciona a frequência de operação do PWM
void setDuty_2(int duty);
int PID(int error);
void frente();
void tras();
void esquerda();
void direita();
void motor_off();
void freio();
int entrouCurva(int sensor, int frontal, int valor_erro);
int PID_Curva(int error_curva);
void setup();
void loop();
int calibra_sensores();
int seta_calibracao();
int sensores();
void area_de_parada();
void sentido_de_giro();
void PWM_limit();
void correcao_do_PWM();


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
    TCNT0 = 240; //Recarrega o Timer 0 para que a contagem seja 1ms novamente
    millis++; //Incrementa a variável millis a cada 1ms
    contador++;
    if (contador == 50) {
        correcao_do_PWM();
        PWM_limit();
        contador = 0;
    }
    sensores();
}

ISR(TIMER1_OVF_vect) {
    TCNT1 = 64755; //Inicializa em 64755 com 50ms
}

ISR(TIMER2_OVF_vect) {
    TCNT2 = 178; //Inicializa em 178 para um tempo de 5ms
    area_de_parada();
    sentido_de_giro();
}


int main(void) {


    setup();

    while (1) loop();
}


//===Funções não visíveis ao usuário======//


void setup() {

    DDRD = 0b01111000; //PD6 - PD3 definidos como saída
    PORTD = 0b00000000; //inicializados em nível baixo
    DDRB = 0b00100110; //Habilita PB1 e PB2 e PB5 como saída   PB1 e PB2 são portas PWM
    PORTB = 0b00000000; //PORTB inicializa desligado e saídas sem pull up
    DDRC = 0b00000000;  //PORTC como entrada
    PORTC = 0b10101111; //Algumas entradas com pull up
    
    UART_config(); //Inicializa a comunicação UART
    inicializa_ADC(); //Configura o ADC
    UART_enviaString(s); //Envia um texto para o computador


    //=============Configuração dos timers=========//
    TCCR0B = 0b00000101; //TC0 com prescaler de 1024
    TCNT0 = 240; //Inicia a contagem em 100 para, no final, gerar 1ms
    TIMSK0 = 0b00000001; //habilita a interrupção do TC0

    TCCR1B = 0b00000101; //TC1 com prescaler de 1024 de 50ms
    TCNT1 = 64755; //Inicializa em 64755
    TIMSK1 = 0b00000001; //habilita a interrupção do TC1

    TCCR2B = 0b00000111; //TC2 com prescaler de 1024 com 5ms
    TCNT2 = 178; //Inicializa em 178 para um tempo de 5ms
    TIMSK2 = 0b00000001; //Habilita a interrupção do TC2
    //=================================================//

    //==========configuração das interrupções externas==========//
    PCICR = 0b00000001; //Ativa os PCINT0 - interrupção externa
    PCMSK0 = 0b01111111; //Habilita o PC0 - PC6 como PCINT (PCINT específico)


    //====Configuração do PWM========================//
    TCCR1A = 0xA2; //Configura operação em fast PWM, utilizando registradores OCR1x para comparação



    setFreq(4); //Seleciona opção para frequência

    //============================//

    sei(); //Habilita as interrupções


    set_bit(PORTB, PB5); //subrotina de acender e apagar o LED 13
    calibra_sensores(); //calibração dos sensores
    sensores(); //determina o limiar dos sensores e printa seus valores na tela
    //========================//

    clr_bit(PORTB, PB5);
    _delay_ms(500);;
    set_bit(PORTB, PB5);
    _delay_ms(500);
    clr_bit(PORTB, PB5);
    _delay_ms(500);
    set_bit(PORTB, PB5);
    _delay_ms(1000);
    clr_bit(PORTB, PB5);
    _delay_ms(1000);
}

void loop() {


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
  
        option  frequency (as frequências no timer 1 são menores do que as frequências nos timers 0 e 2)
        
          1      16    kHz
          2       2    kHz
          3     250     Hz
          4     62,5    Hz
          5     15,6    Hz
     */
    TCCR1B = option;


} //end setFrequency

int PID(int error) {

    p = (error * Kp) / prescale; // Proporcao

    integral += error; // Integral
    i = ((Ki * integral) / prescale);

    d = ((Kd * (error - erroAnterior)) / prescale); // Derivada
    erroAnterior = error;

    Turn = p + i + d;
    return Turn; //retorna os valores após o PID
}

void frente() {

    set_bit(PORTD, AIN1); //frente direita
    clr_bit(PORTD, AIN2);
    set_bit(PORTD, BIN2); //frente esquerda
    clr_bit(PORTD, BIN1);
}

void tras() {
    clr_bit(PORTD, AIN1);
    set_bit(PORTD, AIN2); //tras direita
    clr_bit(PORTD, BIN2);
    set_bit(PORTD, BIN1); //tras esquerda

}

void motor_off() {
    clr_bit(PORTD, AIN1);
    clr_bit(PORTD, AIN2);
    clr_bit(PORTD, BIN2);
    clr_bit(PORTD, BIN1);
}

void freio() {
    frente();

    setDuty_1(50);
    setDuty_2(50);

    _delay_ms(500);

    tras();

    setDuty_1(10);
    setDuty_2(10);

    _delay_ms(5);

    frente();

    setDuty_1(0);
    setDuty_2(0);

    _delay_ms(2000);

    motor_off();

    _delay_ms(60000);
}

/*void direita() {
    set_bit(PORTD, AIN1); //frente direita
    clr_bit(PORTD, AIN2);
    set_bit(PORTD, BIN2); //frente esquerda
    clr_bit(PORTD, BIN1); 

    setDuty_1(PWMA_C);
    setDuty_2(PWMB_C);

    //calibração dos sensores frontais - seta o valor médio
}

void esquerda() {
    set_bit(PORTD, AIN1); //frente direita
    clr_bit(PORTD, AIN2);
    set_bit(PORTD, BIN2); //frente esquerda
    clr_bit(PORTD, BIN1); 

    setDuty_1(PWMA_C);
    setDuty_2(PWMB_C);


    //calibração dos sensores frontais - seta o valor médio
}*/

int entrouCurva(int sensor, int frontal, int valor_erro) {
    if (sensor < 550 && frontal > 100) {
        switch (entrou) {
            case 0: //entrou na curva
                u_curva = PID_Curva(valor_erro);
                PWMA_C = PWM_Curva - u_curva;
                PWMB_C = PWM_Curva + u_curva;
                frente();
                setDuty_1(PWMA_C);
                setDuty_2(PWMB_C);
                entrou = 1;
                break;

            case 1: //término da curva
                entrou = 0;
                frente();
                setDuty_1(PWMA);
                setDuty_2(PWMB);
                clr_bit(PORTB, PB5);
                break;
        }
    } else if (sensor > 550 && frontal < 100) { //região de cruzamento
        frente();
        setDuty_1(PWMA);
        setDuty_2(PWMB);
    }
}

int PID_Curva(int error_curva) {
    p_curva = (error_curva * Kp) / prescale; // Proporcao

    integral_curva += error_curva; // Integral
    i_curva = ((Ki * integral_curva) / prescale);

    d_curva = ((Kd * (error_curva - erroAnterior_curva)) / prescale); // Derivada
    erroAnterior_curva = error_curva;

    Turn_curva = p_curva + i_curva + d_curva;
    return Turn_curva; //retorna os valores após o PID
}
//=========================================//


//=========Funções visíveis ao usuário===========//

int calibra_sensores() {
    int calibrado = 0;
    //=====Função que inicializa a calibração====//
    for (int i = 0; i < 120; i++) {
        int sensores_frontais[] = {le_ADC(3), le_ADC(2), le_ADC(1), le_ADC(0), le_ADC(7), le_ADC(6)};
        for (int i = 0; i < 6; i++) {
            if (valor_min [i] < sensores_frontais [i]) {
                valor_min[i] = sensores_frontais[i];
            } 
            else if (valor_max [i] > sensores_frontais[i]) {
                valor_max[i] = sensores_frontais [i];
            }
        }

        /*
        Após isso determinar o limiar de todos os sensores para que eles tenham os mesmos valores do AD. 
        Para que todos tenham um limite inferior e superior igual.
         */
    }

    calibrado = seta_calibracao();
    return calibrado;
}

int seta_calibracao() {
    //----> Calibração dos Sensores frontais <----\\

    //função que seta o limiar dos sensores
    for (int i = 0; i < 6; i++) {
        if (valor_min_abs < valor_min [i]) {
            valor_min_abs = valor_min [i];
        }
        else if (valor_max_abs > valor_max [i]) {
            valor_max_abs = valor_max [i];
        }
    }
    //valores que os sensores não poderiam ultrapassar
    //return (valor_min_abs, valor_max_abs);
}

int sensores() {
    seta_calibracao(); //Estabelece os limites dos sensores

    int sensores_frontais[6] = {le_ADC(3), le_ADC(2), le_ADC(1), le_ADC(0), le_ADC(7), le_ADC(6)};
    //======Estabelece o limiar da leitura dos sensores====//
    //função de correção da calibração
    for (int i = 0; i < 6; i++) {
        if (valor_min_abs < sensores_frontais[i]) {
            sensores_frontais[i] = valor_min_abs;
        } 
        else if (valor_max_abs > sensores_frontais[i]) {
            sensores_frontais [i] = valor_max_abs;
        }

        sprintf(buffer, "%4d", sensores_frontais[i]); //Converte para string
        UART_enviaString(buffer); //Envia para o computador
        UART_enviaCaractere(0x20); //espaço
    }
    UART_enviaCaractere(0x0A); //pula linha
}

void area_de_parada() {
    //--------------->AREA DO SENSOR DE PARADA<---------------
    int sensor_borda = le_ADC(5);
    if ((sensor_borda < 300) && (acionador == 0) && soma_total > 100) {
        contador++;
        acionador = 1;
    } else if ((sensor_borda > 500) && (acionador == 1)) {
        acionador = 0;
    } else if ((sensor_borda > 500) && soma_total < 100) //situação de cruzamento
    {
        acionador = 0;
    }//função freio
    else if (contador >= marcadores) {
        freio();
    }


}

void sentido_de_giro() {
    int sensor_borda = le_ADC(5);
    if (erro < 0) //virar para a esquerda
    {
        entrouCurva(sensor_borda, soma_total, erro);
        set_bit(PORTB, PB5); //liga o LED
        /*while (erro < 0) {
            frente();
            setDuty_1(PWMA_C);
            setDuty_2(PWMB_C);
        }*/

    } else if (erro > 0) {
        entrouCurva(sensor_borda, soma_total, erro);
        set_bit(PORTB, PB5); //liga o LED
        /*while (erro > 0) {
            frente();
            setDuty_1(PWMA_C);
            setDuty_2(PWMB_C);
        }*/
    }
    //A função que fazia o robô rodar em seu próprio eixo foi removida
}

void PWM_limit() {
    //------> Limitando PWM

    if (PWMA > 255) {
        PWMA = 250;
    }
    else if (PWMB > 255) {
        PWMB = 250;
    }
}

void correcao_do_PWM() {
    //==============================================//

    tempo_atual = millis;
    delta_T = tempo_atual - tempo_passado;


    //int sensores_frontais[6] = {le_ADC(0), le_ADC(1), le_ADC(2), le_ADC(3), le_ADC(4), le_ADC(6)};
    int sensores_frontais[6] = {le_ADC(3), le_ADC(2), le_ADC(1), le_ADC(0), le_ADC(7), le_ADC(6)}; //Grogue antigo 
    for (int j = 0; j < 3; j++) {
        soma_esquerdo += (sensores_frontais[j] * peso[j]);
        soma_direito += (sensores_frontais[5 - j] * peso[5 - j]);
    }

    soma_total = (soma_esquerdo + soma_direito) / (denominador_esquerdo + denominador_direito);

    erro = 0 - soma_total; //valor esperado(estar sempre em cima da linha) - valor medido

    /*sprintf(buffer, "%5d\n", erro); //Converte para string
    UART_enviaString(buffer); //Envia para o computador
    UART_enviaCaractere(0x0A); //pula linha*/ //leitura do erro

    soma_esquerdo = 0;
    soma_direito = 0;
    soma_total = 0;

    u = PID(erro);
    PWMA = PWMR - u;
    PWMB = PWMR + u;

    frente();
    setDuty_1(PWMA);
    setDuty_2(PWMB);
    
    /*if(delta_T >= 50){
        tempo_passado = tempo_atual;

    }*///sem uso no momento


}