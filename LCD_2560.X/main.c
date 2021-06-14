#define F_CPU 16000000      //define a frequencia do uC para 16MHz
#include <avr/io.h>         //biblioteca de mapeamento dos registradores
#include <util/delay.h>     //biblioteca que gera atraso
#include <avr/pgmspace.h>   //para o uso do PROGMEM, grava��o de dados na mem�ria flash
#include "Display.h"           //biblioteca do LCD
#include <stdio.h> //biblioteca para o uso da fun��o sprintf

//Defini��es de macros para trabalho com bits
#define	set_bit(y,bit)	(y|=(1<<bit))	//coloca em 1 o bit x da vari�vel Y
#define	clr_bit(y,bit)	(y&=~(1<<bit))	//coloca em 0 o bit x da vari�vel Y
#define cpl_bit(y,bit) 	(y^=(1<<bit))	//troca o estado l�gico do bit x da vari�vel Y
#define tst_bit(y,bit) 	(y&(1<<bit))	//retorna 0 ou 1 conforme leitura do bit

//defini��o para acessar a mem�ria flash
PROGMEM const char flash_msg[] = "FLASH - IFSP"; //mensagem armazenada na mem�ria flash
char ram_msg[] = "RAM - Ola mundo";
//----------------------------------------------------------------------------//

int aula01() {
    DDRD = 0b11111111; //PORTD como sa�da
    DDRB = 0b00101000; //PB5 e PB3 como sa�da

    inic_LCD_4bits(); //iniciliza o LCD
    escreve_LCD(ram_msg); //string armazenada na RAM
    cmd_LCD(0xc0, 0); //desloca cursor para a segunda linha
    escreve_LCD_Flash(flash_msg); //string armazenada no flash
    while (1) {
    }
}

//defini��o para acessar a mem�ria RAM
char msg1[] = "E vai           ";
char msg2[] = "         e volta";

int aula02() {
    DDRD = 0b11111111; //PORTD como sa�da
    DDRB = 0b00101000; //PB5 e PB3 como sa�da

    inic_LCD_4bits(); //iniciliza o LCD

    while (1) {
        cmd_LCD(0x01, 0); //apaga o display
        escreve_LCD(msg1); //escreve a msg1
        for (int i = 0; i < 12; i++) {
            _delay_ms(150);
            cmd_LCD(0x18, 0); //desloca a msg para a direita
        }
        cmd_LCD(0x01, 0); //apaga o display
        escreve_LCD(msg2); //escreve msg2
        for (int i = 0; i < 10; i++) {
            _delay_ms(150);
            cmd_LCD(0x1C, 0); //Desloca a msg para a esquerda - olhar datasheet
        }
    }
}


char msg3[] = "CONTAGEM = ";
char buffer[4] = " "; //Buffer para armazenamento da vari�vel convertida
//--------------------------------------------------------------------------------------

int aula03() {
    DDRD = 0b11111111; //PORTD inteiro como sa�da
    DDRB = 0b00101000; //PB3 e PB5 como sa�da

    inic_LCD_4bits(); //inicializa o LCD
    escreve_LCD(msg3); //string armazenada na RAM

    while (1) {
        for (int i = 0; i < 999; i++) {
            _delay_ms(500);
            sprintf(buffer, "%.3d", i); //Converte a vari�vel em uma string
            cmd_LCD(0x8B, 0); //Retorna o cursor para o in�cio do n�mero
            escreve_LCD(buffer); //Escreve a vari�vel no display
        }
    }
}

int aula04() {
    
    DDRD = 0b11111111; //PORTD inteiro como sa�da
    DDRB = 0b00101000; //PB3 e PB5 como sa�da

    float i = 0;
    
    i = 3.15;
            
    inic_LCD_4bits(); //inicializa o LCD
    escreve_LCD(msg3); //string armazenada na RAM

    sprintf(buffer, "%.3f", i); //Converte a vari�vel em uma string
    cmd_LCD(0x8b, 0);        //Retorna o curso para o in�cio do caracter
    escreve_LCD(buffer); //Escreve a vari�vel no display

    while (1) {

    }
}

void main() {
    //aula01();
    //aula02();
    aula03();
    //aula04();
}
