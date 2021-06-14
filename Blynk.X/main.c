#define F_CPU 16000000      //define a frequencia do uC para 16MHz
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include "UART.h"
#include "configbits.txt"

#define set_bit(y,bit) (y|=(1<<bit)) //coloca em 1 o bit x da variável Y
#define clr_bit(y,bit) (y&=~(1<<bit)) //coloca em 0 o bit x da variável Y
#define cpl_bit(y,bit) (y^=(1<<bit)) //troca o estado lógico do bit x da variável Y
#define tst_bit(y,bit) (y&(1<<bit)) //retorna 0 ou 1 conforme leitura do bit

char s[] = "Comunicacao SERIAL\n";
char bt_press[] = "BOTAO PRESSIONADO\n";
char bt_solto[] = "BOTAO SOLTO\n";
char buffer[5];
volatile char ch; //armazena o caractere lido
volatile char flag_com = 0; //flag que indica se houve recepção de dado
// Interrupção da UART
//======================================================

ISR(USART_RX_vect) {
    ch = UDR0; //Faz a leitura do buffer da serial

    UART_enviaCaractere(ch); //Envia o caractere lido para o computador
    flag_com = 1; //Aciona o flag de comunicação
}
//------------------------------------------------------

int main() {

    DDRC = 0b00000000;
    PORTC = 0b00001100;
    DDRD = 0b11111111;
    PORTD = 0b00000000;
    UART_config(); //Inicializa a comunicação UART
    UART_enviaString(s); //Envia um texto para o computador

    sei(); //Habilita as interrupções

    while (1) {
        //Se houve comunicação, testa o caractere recebido

        if (flag_com == 1) {
            switch (ch) {
                case 'A':

                    set_bit(PORTD, PD7); //Liga o LED6
                    UART_enviaCaractere(0x0D); //pula linha

                    break;

                case 'B':

                    clr_bit(PORTD, PD7); //Desliga o LED6
                    UART_enviaCaractere(0x0D); //pula linha

                    break;
                case 'C':
                    for(int i = 0; i < 99; i++){
                        cpl_bit(PORTD, PD7);
                        _delay_ms(50);
                    }
                    break;

                default:
                    UART_enviaCaractere(0x0D); //pula linha

                    break;
            }
            //Apaga o flag de comunicação
            flag_com = 0;

        }
    }
}
