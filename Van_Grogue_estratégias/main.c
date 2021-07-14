/*
 * File:   main.c
 * Author: Johann
 *
 * Created on 30 de Maio de 2021, 16:52
 * Last update on July 7th of 2021 at 15:43
 */

/*Bibliotecas e frequência do uc*/
#define F_CPU 16000000      //define a frequencia do uC para 16MHz
#include <avr/io.h>         //Biblioteca geral dos AVR
#include <avr/interrupt.h>  //Biblioteca de interrupção
#include <stdio.h>          //Bilioteca do C
#include <util/delay.h>     //Biblioteca geradora de atraso
#include "UART.h"           //Biblioteca da comunicação UART
#include "ADC.h"            //Biblioteca do conversor AD
#include "PWM_10_bits.h"    //Biblioteca de PWM fast mode de 10 bits
#include "Driver_motor.h"   //Biblioteca das funções de controle dos motores
#include "PID.h"            //Biblioteca do controle PID
//#include "configbits.txt"   //configura os fusíveis
/*============================================================*/


//variáveis de comando para os registradores
#define set_bit(y,bit) (y|=(1<<bit)) //coloca em 1 o bit x da variável Y
#define clr_bit(y,bit) (y&=~(1<<bit)) //coloca em 0 o bit x da variável Y
#define cpl_bit(y,bit) (y^=(1<<bit)) //troca o estado lógico do bit x da variável Y
#define tst_bit(y,bit) (y&(1<<bit)) //retorna 0 ou 1 conforme leitura do bit
/*==============================================================*/

/*Mapeamento de Hardware*/
#define sensor_de_curva   PB0
#define sensor_de_parada  PD7
#define led               PB5
#define leitura_curva    PINB
#define leitura_parada   PIND

/*==============================================================*/

/*Variáveis globais*/
int erro = 0; //Área PID
int PWMA = 0, PWMB = 0; // Modulação de largura de pulso enviada pelo PID

//Variáveis globais da calibração de sensores
unsigned int valor_max [] = {1023, 1023, 1023, 1023, 1023, 1023}; //variáveis usadas na calibração do sensores
unsigned int valor_min [] = {0, 0, 0, 0, 0, 0};
unsigned int valor_min_abs = 0, valor_max_abs = 1023;

//Variáveis globais do timer0
unsigned int millis = 0;
unsigned int counter1 = 0, counter2 = 0;

//Variáveis globais da UART
//char s [] = "Início da leitura";
char buffer[5]; //String que armazena valores de entrada para serem printadas
volatile char ch; //armazena o caractere lido
volatile char flag_com = 0; //flag que indica se houve recepção de dado
// Interrupção da UART


/*======================================================*/


/*tempo =65536 * Prescaler/Fosc = 65536 * 1024/16000000 = 4, 19s
 tempo = X_bit_timer * Prescaler/Fosc
 Valor inicial de contagem = 256 - tempo_desejado*Fosc/Prescaler = 256 - 0,001*16000000/1024 = 255
 Valor inicial de contagem = X_bit_timer - tempo_desejado*Fosc/Prescaler */
/*===========================================================================*/

/*Protótipo das funções*/
void tomada_de_tempo();
void estrategia();
void mapeamento();
void coleta_de_dados();
void tomada_de_tempo();
void entrou_na_curva(int valor_erro, int PWM_Curva);
void parada(int value_erro, int PWM_Curva);
void calibra_sensores();
void seta_calibracao();
void sensores();
void setup();
void setup_Hardware();
void setup_logica();
void loop();
void area_de_parada(int PWM_Curva);
void sentido_de_giro(int PWM_Curva);
void PWM_limit();
void correcao_do_PWM(int PWMR);
/*===========================================================================*/

/*Interrupções*/
ISR(USART_RX_vect) {
    ch = UDR0; //Faz a leitura do buffer da serial

    UART_enviaCaractere(ch); //Envia o caractere lido para o computador
    flag_com = 1; //Aciona o flag de comunicação
}

ISR(TIMER0_OVF_vect) {
    TCNT0 = 240; //Recarrega o Timer 0 para que a contagem seja 1ms novamente
    millis++; //Incrementa a variável millis a cada 1ms
    estrategia();
}//end TIMER_0

/*============================================================================*/

/*Função principal*/
int main(void) {
    setup();

    while (1) loop();
}//end main

//===Funções não visíveis ao usuário======//

void setup() {

    setup_Hardware();
    setup_logica();
    sei(); //Habilita as interrupções

}

void setup_Hardware() {
    DDRD = 0b01111010; //PD3 - PD6 definidos como saída, PD7 como entrada
    PORTD = 0b10000000; //inicializados em nível baixo e PD7 com pull up
    DDRB = 0b00100110; //Habilita PB0 como entrada e PB5, PB1 e PB2 como saída
    PORTB = 0b00000001; //PORTB inicializa desligado e pull up no PB0
    DDRC = 0b00000000; //PORTC como entrada
    PORTC = 0b00001111; //PC3 - PC0 com pull up (colocar resistor de pull up nos pinos A6 e A7)


    TCCR0B = 0b00000101; //TC0 com prescaler de 1024
    TCNT0 = 240; //Inicia a contagem em 100 para, no final, gerar 1ms
    TIMSK0 = 0b00000001; //habilita a interrupção do TC0

    EICRA = 0x01;
    EIMSK = 0x01; //habilita INT0
    
    TCCR1A = 0xA2; //Configura operação em fast PWM, utilizando registradores OCR1x para comparação

    setFreq(4); //Seleciona opção para frequência

}

void setup_logica() {
    
    inicializa_ADC(); //Configura o ADC
    
    //----> Calibração dos Sensores frontais <----//
    set_bit(PORTB, led); //subrotina de acender e apagar o LED 13
    calibra_sensores(); //calibração dos sensores
    seta_calibracao(); //estabelece o limiar dos sensores através dos valores da função de cima
    sensores(); //determina o limiar dos sensores e printa seus valores na tela

    clr_bit(PORTB, led);
    _delay_ms(500);
    set_bit(PORTB, led); //subrotina de acender e apagar o LED 13
    _delay_ms(1000);
    clr_bit(PORTB, led);
    _delay_ms(500);
    set_bit(PORTB, led);
    _delay_ms(500);
    clr_bit(PORTB, led);
    _delay_ms(2000);
    
    
    estrategia();


}

void loop()//loop vazio
{

}

void estrategia()
{
    /*Configuração das estratégias*/
    /*Utilização do PB3 e PB4 como entrada para o dip switch de 2 vias*/
    
    
    char sw; //variável do switch
    sw = 0b00000011; //determinação de 8 bits para uma lógica AND com o PINx

    sw &= PINB >> 3; //lógica AND com a leitura dos pinos deslocados três bits à direta

    switch (sw) {
        case 0:
            //esquerdo pino 4 - PD2
            UART_config(); //Inicializa a comunicação UART
            //mapeamento(); //estrategia de mapeamento
            break;

        case 1:
            //esquerdo pino 4 - PD2
            UART_config(); //Inicializa a comunicação UART
            sei(); //Habilita as interrupções
            coleta_de_dados(); //estratégia de coleta de dados
            break;

        case 2:
            //esquerdo pino 4 - PD2
            tomada_de_tempo(); //estratégia de tomada de tempo
            break;

    }
    
    
}

void mapeamento()
{
    static unsigned int counter1 = 0, counter2 = 0;
    static unsigned int PWMR = 400; // valor da força do motor em linha reta
    static unsigned int PWM_Curva = 350; //PWM ao entrar na curva
    counter1++;
    counter2++;
    
    //mapping();
    sensores(); //faz a leitura dos sensores e se estiverem com valores fora do limiar, a correção será feita.

    if(counter1 == 5)   //chamado a cada 5ms
    {
        correcao_do_PWM(PWMR); //controle PID
        sprintf(buffer, "%5d\n", erro); //Converte para string
        UART_enviaString(buffer); //Envia para o computador
        UART_enviaCaractere(0x0D); //pula linha

        counter1 = 0;
    }
    
    if(counter2 == 50)  //chamado a cada 50ms
    {
         
        area_de_parada(PWM_Curva); //Verfica se é uma parada ou um cruzamento
        sentido_de_giro(PWM_Curva); //Verifica qual o sentido da curva
        counter2 = 0;
    }
    
    
    
}


void coleta_de_dados() {
    static unsigned int counter1 = 0, counter2 = 0;
    static unsigned int PWMR = 600; // valor da força do motor em linha reta
    static unsigned int PWM_Curva = 500; //PWM ao entrar na curva
    counter1++;
    counter2++;
    
    sensores(); //faz a leitura dos sensores e se estiverem com valores fora do limiar, a correção será feita.

    if(counter1 == 5)   //chamado a cada 5ms
    {
        correcao_do_PWM(PWMR); //controle PID
        PWM_limit(); //Muda o valor do PWM caso o PID gere um valor acima de 8 bits no final
        sprintf(buffer, "%5d\n", erro); //Converte para string
        UART_enviaString(buffer); //Envia para o computador
        UART_enviaCaractere(0x0D); //pula linha

        counter1 = 0;
    }
    
    if(counter2 == 50)  //chamado a cada 50ms
    {
         
        area_de_parada(PWM_Curva); //Verfica se é uma parada ou um cruzamento
        sentido_de_giro(PWM_Curva); //Verifica qual o sentido da curva
        counter2 = 0;
    }
    
    
}

void tomada_de_tempo()
{
    
    static unsigned int counter1 = 0, counter2 = 0;
    static unsigned int PWMR = 800; // valor da força do motor em linha reta
    static unsigned int PWM_Curva = 700; //PWM ao entrar na curva
    counter1++;
    counter2++;
    sensores(); //faz a leitura dos sensores e se estiverem com valores fora do limiar, a correção será feita.

    if(counter1 == 5)   //chamado a cada 5ms
    {
        correcao_do_PWM(PWMR); //controle PID
        PWM_limit(); //Muda o valor do PWM caso o PID gere um valor acima de 8 bits no final
        counter1 = 0;
    }
    
    if(counter2 == 50)  //chamado a cada 50ms
    {
         
        area_de_parada(PWM_Curva); //Verfica se é uma parada ou um cruzamento
        sentido_de_giro(PWM_Curva); //Verifica qual o sentido da curva
        counter2 = 0;
    }
    
    
}

//=========Funções visíveis ao usuário===========//

void entrou_na_curva(int valor_erro, int PWM_Curva) {
    int u_curva = 0;
    static unsigned int PWMA_C = 0, PWMB_C = 0, entrou = 0; //PWM de curva com ajuste do PID;

    if ((!tst_bit(leitura_curva, sensor_de_curva)) && tst_bit(leitura_parada, sensor_de_parada))
        //li branco no sensor de curva e li preto no sensor de parada
    {
        switch (entrou) {
            case 0: //entrou na curva
                u_curva = PID(valor_erro);
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
                setDuty_1(PWMA); //témino da curva
                setDuty_2(PWMB);
                clr_bit(PORTB, led);
                break;
        }
    }
}

void parada(int value_erro, int PWM_Curva) {

    static char contador = 0, numParada = 4; // Borda   //contador - número de marcadores de curva;

    if ((!tst_bit(leitura_curva, sensor_de_curva)) && tst_bit(leitura_parada, sensor_de_parada)) {
        contador++;
        entrou_na_curva(value_erro, PWM_Curva); // Verifica se é uma curva
    }
    else if ((!tst_bit(leitura_curva, sensor_de_curva)) && (!tst_bit(leitura_parada, sensor_de_parada))) //verifica se é crizamento
    {
        frente();
        setDuty_1(PWMA);
        setDuty_2(PWMB);
    }

    while (contador == numParada) {
        freio();
    }
}

void calibra_sensores() {
    //=====Função que inicializa a calibração====//
    for (int i = 0; i < 120; i++) {
        int sensores_frontais[] = {le_ADC(3), le_ADC(2), le_ADC(1), le_ADC(0), le_ADC(7), le_ADC(6)};
        for (int i = 0; i < 6; i++) {
            if (valor_min [i] < sensores_frontais [i]) {
                valor_min[i] = sensores_frontais[i];
            } else if (valor_max [i] > sensores_frontais[i]) {
                valor_max[i] = sensores_frontais [i];
            }
        }

        _delay_ms(10); //tempo o suficiente para o pessoa calibrar os sensores mecanicamente

        /*
        Após isso determinar o limiar de todos os sensores para que eles tenham os mesmos valores do AD. 
        Para que todos tenham um limite inferior e superior igual.
         */
    }

}

void seta_calibracao() {
    //----> Calibração dos Sensores frontais <----//

    //função que seta o limiar dos sensores
    for (int i = 0; i < 6; i++) {
        if (valor_min_abs < valor_min [i]) {
            valor_min_abs = valor_min [i];
        } else if (valor_max_abs > valor_max [i]) {
            valor_max_abs = valor_max [i];
        }
    }
}

void sensores() {

    int sensores_frontais[6] = {le_ADC(3), le_ADC(2), le_ADC(1), le_ADC(0), le_ADC(7), le_ADC(6)};
    //======Estabelece o limiar da leitura dos sensores====//
    //função de correção da calibração
    for (int i = 0; i < 6; i++) {
        if (valor_min_abs < sensores_frontais[i]) {
            sensores_frontais[i] = valor_min_abs;
        } else if (valor_max_abs > sensores_frontais[i]) {
            sensores_frontais [i] = valor_max_abs;
        }

        sprintf(buffer, "%4d", sensores_frontais[i]); //Converte para string
        UART_enviaString(buffer); //Envia para o computador
        UART_enviaCaractere(0x20); //espaço
    }
    UART_enviaCaractere(0x0A); //pula linha
}

void area_de_parada(int PWM_Curva) {
    //--------------->AREA DOS SENSORES<---------------
    static int ejetor = 0;
    static unsigned int delta_T = 0;
    static unsigned int tempo_atual = 0;
    static unsigned int timer2, TempoEspera = 100;

    tempo_atual = millis;
    delta_T = tempo_atual - timer2;
    switch (ejetor) {
        case 0:
            if ((tst_bit(leitura_curva, sensor_de_curva)) || (tst_bit(leitura_parada, sensor_de_parada)))//verifica se sos sensores estão em nível 0
            {
                timer2 = tempo_atual;
                ejetor = 1;
            }
            break;

        case 1:
            if ((delta_T) > TempoEspera) {
                parada(erro, PWM_Curva); // Verifica se é um marcador de parada
                timer2 = 0;
                ejetor = 0;
                //ejetor = 2;
            }
            break;

            /*case 2:
                if ((!(tst_bit(leitura_curva, sensor_de_curva))) && (!(tst_bit(leitura_parada, sensor_de_parada)))) {
                    timer2 = 0;
                    ejetor = 0;
                }
                break;*/
    }
}

void sentido_de_giro(int PWM_Curva) {
    //-----> Área do senstido de giro


    if (erro < 0) //virar para a esquerda
    {
        entrou_na_curva(erro, PWM_Curva);
        set_bit(PORTB, led); //liga o LED
        /*while (erro < 0) {
            frente();
            setDuty_1(PWMA_C);
            setDuty_2(PWMB_C);
        }*/

    }
    else if (erro > 0) { //cirar para a direita
        entrou_na_curva(erro, PWM_Curva);
        set_bit(PORTB, led); //liga o LED
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

    if (PWMA > 1023) {
        PWMA = 1000;
    } else if (PWMB > 1023) {
        PWMB = 1000;
    }
}

void correcao_do_PWM(int PWMR) {

    int soma_direito = 0, soma_esquerdo = 0, denominador_direito = 6, denominador_esquerdo = 6, soma_total = 0;
    int u = 0; //valor de retorno do PID
    
    static int peso [] = {-3, -2, -1, 1, 2, 3}; //utilizando um prescale de 2000

    int sensores_frontais[6] = {le_ADC(3), le_ADC(2), le_ADC(1), le_ADC(0), le_ADC(7), le_ADC(6)};
    for (int j = 0; j < 3; j++) {
        soma_esquerdo += (sensores_frontais[j] * peso[j]);
        soma_direito += (sensores_frontais[5 - j] * peso[5 - j]);
    }

    soma_total = (soma_esquerdo + soma_direito) / (denominador_esquerdo + denominador_direito);

    erro = 0 - soma_total; //valor esperado(estar sempre em cima da linha) - valor medido

    //--------------->AREA DO PID<---------------

    u = PID(erro);

    PWMA = PWMR - u;
    PWMB = PWMR + u;

    frente();
    setDuty_1(PWMA);
    setDuty_2(PWMB);
}//fim do programa