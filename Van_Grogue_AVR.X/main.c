/*
 * File:   main.c
 * Author: johan
 *
 * Created on 30 de Maio de 2021, 16:52
 */


#define F_CPU 16000000      //define a frequencia do uC para 16MHz
#include <avr/io.h>         //Biblioteca geral dos AVR
#include <avr/interrupt.h>  //Biblioteca de interrup��o
#include <stdio.h>          //Bilioteca do C
#include <util/delay.h>     //Biblioteca geradora de atraso
#include "UART.h"           //Biblioteca da comunica��o UART
#include "ADC.h"            //Biblioteca do conversor AD
//#include "configbits.txt"   //configura os fus�veis

//vari�veis de comando para os registradores
#define set_bit(y,bit) (y|=(1<<bit)) //coloca em 1 o bit x da vari�vel Y
#define clr_bit(y,bit) (y&=~(1<<bit)) //coloca em 0 o bit x da vari�vel Y
#define cpl_bit(y,bit) (y^=(1<<bit)) //troca o estado l�gico do bit x da vari�vel Y
#define tst_bit(y,bit) (y&(1<<bit)) //retorna 0 ou 1 conforme leitura do bit


//Lado direito
#define AIN2 PD6
#define AIN1 PD5 // Quando em HIGH, roda direita anda para frente 

//Lado Esquerdo
#define BIN1 PD4 
#define BIN2 PD3 // Quando em HIGH, roda esquerda anda para frente


#define sensor_de_curva PB0
#define sensor_de_parada PD7


int Kp = 2; //prescale de 100 - prescale int
int Kd = 1; //prescale de 100 - prescale int
int Ki = 1; // Vari�veis que s�o modificadas no PID - prescale de 100 ou mais
int PWMR = 400; // valor da for�a do motor em linha reta
int PWM_Curva = 350; //PWM ao entrar na curva
int erro, p, d, erroAnterior = 0, i, integral = 0, Turn = 0; //�rea PID -erro tamb�m possui prescale = 100
int u = 0; //valor de retorno do PID
int u_curva = 0;
int prescale = 2000; //prescale das constantes * prescale do erro
int PWMA, PWMB; // Modula��o de largura de pulso enviada pelo PID
int PWMA_C, PWMB_C; //PWM de curva com ajuste do PID
int entrou;
int erroAnterior_curva = 0, erroAnterior_traseiro = 0;
int Turn_curva, p_curva, d_curva, i_curva, integral_curva = 0;
int position_line = 0; // M�todo com o Read Line - sem uso por ora
int contador = 0, numParada = 4; // Borda
int curvaValor = 0, entrou = 0;
unsigned int timer2, TempoEspera = 100;
int ejetor = 0;
int tempo_atual = 0;

int valor_min [] = {1023, 1023, 1023, 1023, 1023, 1023}; //vari�veis usadas na calibra��o do sensores
int valor_max [] = {0, 0, 0, 0, 0, 0};
int valor_min_abs = 0, valor_max_abs = 1023;

unsigned int delta_T = 0;
int peso [] = {-3, -2, -1, 1, 2, 3}; //utilizando um prescale de 100
int soma_direito = 0, soma_esquerdo = 0;
int denominador_direito = 6;
int denominador_esquerdo = 6;
int soma_total = 0;

char s [] = "In�cio da leitura";
char buffer[5]; //String que armazena valores de entrada para serem printadas
volatile char ch; //armazena o caractere lido
volatile char flag_com = 0; //flag que indica se houve recep��o de dado
// Interrup��o da UART
//======================================================


/*tempo =65536 ? Prescaler/Fosc = 65536 ? 1024/16000000 = 4, 19s
 tempo = X_bit_timer * Prescaler/Fosc
 Valor inicial de contagem = 256 - tempo_desejado*Fosc/Prescaler = 256 - 0,001*16000000/1024 = 255
 Valor inicial de contagem = X_bit_timer - tempo_desejado*Fosc/Prescaler*/


unsigned int millis = 0;

ISR(USART_RX_vect) {
    ch = UDR0; //Faz a leitura do buffer da serial

    UART_enviaCaractere(ch); //Envia o caractere lido para o computador
    flag_com = 1; //Aciona o flag de comunica��o
}

ISR(TIMER0_OVF_vect) {
    TCNT0 = 240; //Recarrega o Timer 0 para que a contagem seja 1ms novamente
    millis++; //Incrementa a vari�vel millis a cada 1ms
    sensores(); //faz a leitura dos sensores e se estiverem com valores fora do limiar, a corre��o ser� feita.
}

ISR(TIMER1_OVF_vect) {
    TCNT1 = 64755; //Inicializa em 64755 com 50ms
    correcao_do_PWM(); //controle PID
    PWM_limit(); //Muda o valor do PWM caso o PID gere um valor acima de 8 bits no final
}

ISR(TIMER2_OVF_vect) {
    TCNT2 = 178; //Inicializa em 178 para um tempo de 5ms
    area_de_parada(); //Verfica se � uma parada ou um cruzamento
    sentido_de_giro(); //Verifica qual o sentido da curva
}
//------------------------------------------------------

void setDuty_1(int duty); //Seleciona o duty cycle na sa�da digital  3
void setFreq(char option); //Seleciona a frequ�ncia de opera��o do PWM
void setDuty_2(int duty);
int PID(int error);
void frente();
void tras();
void esquerda();
void direita();
void motor_off();
void freio();
void entrou_na_curva(int valor_erro);
int PID_Curva(int error_curva);
int parada(int value_erro);
int calibra_sensores();
int seta_calibracao();
int sensores();
void setup();
void loop();
void area_de_parada();
void sentido_de_giro();
void PWM_limit();
void correcao_do_PWM();

int main(void) {
    setup();

    while (1) loop();
}

//===Fun��es n�o vis�veis ao usu�rio======//

void setup() {


    DDRD = 0b01111000; //PD3 - PD6 definidos como sa�da, PD7 como entrada
    PORTD = 0b00000000; //inicializados em n�vel baixo
    DDRB = 0b00100110; //Habilita PB0 como entrada e PB5, PB1 e PB2 como sa�da
    PORTB = 0b00000000; //PORTB inicializa desligado e sa�das sem pull up


    //esquerdo pino 4 - PD2
    UART_config(); //Inicializa a comunica��o UART
    inicializa_ADC(); //Configura o ADC
    UART_enviaString(s); //Envia um texto para o computador

    TCCR0B = 0b00000101; //TC0 com prescaler de 1024
    TCNT0 = 240; //Inicia a contagem em 100 para, no final, gerar 1ms
    TIMSK0 = 0b00000001; //habilita a interrup��o do TC0

    TCCR1B = 0b00000101; //TC1 com prescaler de 1024 de 50ms
    TCNT1 = 64755; //Inicializa em 64755
    TIMSK1 = 0b00000001; //habilita a interrup��o do TC1

    TCCR2B = 0b00000111; //TC2 com prescaler de 1024 com 5ms
    TCNT2 = 178; //Inicializa em 178 para um tempo de 5ms
    TIMSK2 = 0b00000001; //Habilita a interrup��o do TC2

    TCCR1A = 0xA2; //Configura opera��o em fast PWM, utilizando registradores OCR1x para compara��o

    setFreq(4); //Seleciona op��o para frequ�ncia

    sei(); //Habilita as interrup��es


    //----> Calibra��o dos Sensores frontais <----//
    set_bit(PORTB, PB5); //subrotina de acender e apagar o LED 13
    calibra_sensores(); //calibra��o dos sensores
    sensores(); //determina o limiar dos sensores e printa seus valores na tela
    //========================//

    set_bit(PORTB, PB5); //subrotina de acender e apagar o LED 13
    _delay_ms(1000);
    clr_bit(PORTB, PB5);
    _delay_ms(500);
    set_bit(PORTB, PB5);
    _delay_ms(500);
    clr_bit(PORTB, PB5);


}

void loop() {




    //regi�o que seta os valores nos sensores frontais ap�s a calibra��o
    sensores();

    //----------------------------------------------------------------//

}

void setDuty_2(int duty) //MotorB
{

    OCR1B = duty;

} //end setDuty

void setDuty_1(int duty) //MotorA
{

    OCR1A = duty; //valores de 0 - 1023

} //end setDuty

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

int PID(int error) {
    p = (error * Kp) / prescale; // Proporcao

    integral += error; // Integral
    i = ((Ki * integral) / prescale);

    d = ((Kd * (error - erroAnterior)) / prescale); // Derivada
    erroAnterior = error;

    Turn = p + i + d;
    return Turn; //retorna os valores ap�s o PID
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

    setDuty_1(200);
    setDuty_2(200);

    _delay_ms(500);

    tras();

    setDuty_1(150);
    setDuty_2(150);

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
    clr_bit(PORTD, AIN2); //tras direita 
    set_bit(PORTD, BIN2);
    clr_bit(PORTD, BIN1); //frente esquerda

    setDuty_1(PWMA_C);
    setDuty_2(PWMB_C);

    //calibra��o dos sensores frontais - seta o valor m�dio
}*/

/*void esquerda() {
    set_bit(PORTD, AIN1); //direita frente
    clr_bit(PORTD, AIN2); //direita tras
    set_bit(PORTD, BIN2);
    clr_bit(PORTD, BIN1); //esquerda frente

    setDuty_1(PWMA_C);
    setDuty_2(PWMB_C);


    //calibra��o dos sensores frontais - seta o valor m�dio
}*/

//=========Fun��es vis�veis ao usu�rio===========//

void entrou_na_curva(int valor_erro) {
    if ((!tst_bit(PORTD, sensor_de_curva) >> sensor_de_curva) && tst_bit(PORTD, sensor_de_parada) >> sensor_de_parada) {
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

            case 1:
                entrou = 0;
                frente();
                setDuty_1(PWMA); //t�mino da curva
                setDuty_2(PWMB);
                clr_bit(PORTB, PB5);
                break;
        }
    }
}

int PID_Curva(int error_curva) {
    p_curva = (error_curva * Kp) / prescale; // Proporcao

    integral_curva += error_curva; // Integral
    i_curva = ((Ki * integral_curva) / prescale);

    d_curva = ((Kd * (error_curva - erroAnterior_curva)) / prescale); // Derivada
    erroAnterior_curva = error_curva;

    Turn_curva = p_curva + i_curva + d_curva;
    return Turn_curva; //retorna os valores ap�s o PID
}

int parada(int value_erro) {
    if ((!tst_bit(PORTD, sensor_de_curva) >> sensor_de_curva) && tst_bit(PORTD, sensor_de_parada) >> sensor_de_parada) {
        contador++;
        entrou_na_curva(value_erro); // Verifica se � uma curva
    } else if ((!tst_bit(PORTD, sensor_de_curva) >> sensor_de_curva) && (!tst_bit(PORTD, sensor_de_parada) >> sensor_de_parada)) //verifica se � crizamento
    {
        frente();
        setDuty_1(PWMA);
        setDuty_2(PWMB);
    }

    while (contador == numParada) {
        freio();
    }
}

int calibra_sensores() {
    int calibrado = 0;
    inicializa_ADC(); //Configura o ADC
    //=====Fun��o que inicializa a calibra��o====//
    for (int i = 0; i < 120; i++) {
        int sensores_frontais[] = {le_ADC(3), le_ADC(2), le_ADC(1), le_ADC(0), le_ADC(7), le_ADC(6)};
        for (int i = 0; i < 6; i++) {
            if (valor_min [i] > sensores_frontais [i]) {
                valor_min[i] = sensores_frontais[i];
            } 
            else if (valor_max [i] < sensores_frontais[i]) {
                valor_max[i] = sensores_frontais [i];
            }
        }

        /*
        Ap�s isso determinar o limiar de todos os sensores para que eles tenham os mesmos valores do AD. 
        Para que todos tenham um limite inferior e superior igual.
         */
    }

    calibrado = seta_calibracao();
    return calibrado;
}

int seta_calibracao() {
    //----> Calibra��o dos Sensores frontais <----\\

    //fun��o que seta o limiar dos sensores
    for (int i = 0; i < 6; i++) {
        if (valor_min_abs < valor_min [i]) {
            valor_min_abs = valor_min [i];
        }
        else if (valor_max_abs > valor_max [i]) {
            valor_max_abs = valor_max [i];
        }
    }
    //valores que os sensores n�o poderiam ultrapassar
    //return (valor_min_abs, valor_max_abs);
}

int sensores() {
    seta_calibracao(); //Estabelece os limites dos sensores

    int sensores_frontais[6] = {le_ADC(3), le_ADC(2), le_ADC(1), le_ADC(0), le_ADC(7), le_ADC(6)};
    //======Estabelece o limiar da leitura dos sensores====//
    //fun��o de corre��o da calibra��o
    for (int i = 0; i < 6; i++) {
        if (valor_min_abs > sensores_frontais[i]) {
            sensores_frontais[i] = valor_min_abs;
        } 
        else if (valor_max_abs < sensores_frontais[i]) {
            sensores_frontais [i] = valor_max_abs;
        }

        sprintf(buffer, "%4d", sensores_frontais[i]); //Converte para string
        UART_enviaString(buffer); //Envia para o computador
        UART_enviaCaractere(0x20); //espa�o
    }
    UART_enviaCaractere(0x0A); //pula linha
}

void area_de_parada() {
    //--------------->AREA DOS SENSORES<---------------
    tempo_atual = millis;
    delta_T = tempo_atual - timer2;
    switch (ejetor) {
        case 0:
            if ((!(tst_bit(PORTB, sensor_de_curva) >> sensor_de_curva))
                    || (!(tst_bit(PORTD, sensor_de_parada) >> sensor_de_parada)))//verifica se sos sensores est�o em n�vel 0
            {
                timer2 = tempo_atual;
                ejetor = 1;
            }
            break;

        case 1:
            if ((delta_T) > TempoEspera) {
                parada(erro); // Verifica se � um marcador de parada
                ejetor = 2;
            }
            break;

        case 2:
            if ((tst_bit(PORTB, sensor_de_curva) >> sensor_de_curva)
                    && (tst_bit(PORTD, sensor_de_parada) >> sensor_de_parada)) {
                timer2 = 0;
                ejetor = 0;
            }
            break;
    }
}

void sentido_de_giro() {
    //-----> �rea do senstido de giro

    if (erro < 0) //virar para a esquerda
    {
        entrou_na_curva(erro);
        set_bit(PORTB, PB5); //liga o LED
        /*while (erro < 0) {
            frente();
            setDuty_1(PWMA_C);
            setDuty_2(PWMB_C);
        }*/

    } else if (erro > 0) {
        entrou_na_curva(erro);
        set_bit(PORTB, PB5); //liga o LED
        /*while (erro > 0) {
            frente();
            setDuty_1(PWMA_C);
            setDuty_2(PWMB_C);
        }*/
    }

    //A fun��o que fazia o rob� rodar em seu pr�prio eixo foi removida

}

void PWM_limit() {
    //------> Limitando PWM

    if (PWMA > 255) {
        PWMA = 250;
    } else if (PWMB > 255) {
        PWMB = 250;
    }
}

void correcao_do_PWM() {
    int sensores_frontais[6] = {le_ADC(3), le_ADC(2), le_ADC(1), le_ADC(0), le_ADC(7), le_ADC(6)};
    for (int j = 0; j < 3; j++) {
        soma_esquerdo += (sensores_frontais[j] * peso[j]);
        soma_direito += (sensores_frontais[5 - j] * peso[5 - j]);
    }

    soma_total = (soma_esquerdo + soma_direito) / (denominador_esquerdo + denominador_direito);

    erro = 0 - soma_total; //valor esperado(estar sempre em cima da linha) - valor medido

    /*sprintf(buffer, "%5d\n", erro); //Converte para string
    UART_enviaString(buffer); //Envia para o computador
    UART_enviaCaractere(0x0D); //pula linha*/

    //--------------->AREA DO PID<---------------

    soma_esquerdo = 0;
    soma_direito = 0;
    soma_total = 0;

    u = PID(erro);

    PWMA = PWMR - u;
    PWMB = PWMR + u;

    /*if (delta_T >= TempoEspera) {   //Sem uso no momento, reservado para o encoder

        timer2 = 0;
    }*/

    frente();
    setDuty_1(PWMA);
    setDuty_2(PWMB);
}