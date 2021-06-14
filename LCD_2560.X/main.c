#define F_CPU 16000000      //define a frequencia do uC para 16MHz
#include <avr/io.h>         //biblioteca de mapeamento dos registradores
#include <util/delay.h>     //biblioteca que gera atraso
#include <avr/pgmspace.h>   //para o uso do PROGMEM, gravação de dados na memória flash
#include "Display.h"           //biblioteca do LCD
#include <stdio.h> //biblioteca para o uso da função sprintf

//Definições de macros para trabalho com bits
#define	set_bit(y,bit)	(y|=(1<<bit))	//coloca em 1 o bit x da variável Y
#define	clr_bit(y,bit)	(y&=~(1<<bit))	//coloca em 0 o bit x da variável Y
#define cpl_bit(y,bit) 	(y^=(1<<bit))	//troca o estado lógico do bit x da variável Y
#define tst_bit(y,bit) 	(y&(1<<bit))	//retorna 0 ou 1 conforme leitura do bit

//definição para acessar a memória flash
PROGMEM const char flash_msg[] = "FLASH - IFSP"; //mensagem armazenada na memória flash
char ram_msg[] = "RAM - Ola mundo";
//----------------------------------------------------------------------------//

int aula01() {
    DDRD = 0b11111111; //PORTD como saída
    DDRB = 0b00101000; //PB5 e PB3 como saída

    inic_LCD_4bits(); //iniciliza o LCD
    escreve_LCD(ram_msg); //string armazenada na RAM
    cmd_LCD(0xc0, 0); //desloca cursor para a segunda linha
    escreve_LCD_Flash(flash_msg); //string armazenada no flash
    while (1) {
    }
}

//definição para acessar a memória RAM
char msg1[] = "E vai           ";
char msg2[] = "         e volta";

int aula02() {
    DDRD = 0b11111111; //PORTD como saída
    DDRB = 0b00101000; //PB5 e PB3 como saída

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
char buffer[4] = " "; //Buffer para armazenamento da variável convertida
//--------------------------------------------------------------------------------------

int aula03() {
    DDRD = 0b11111111; //PORTD inteiro como saída
    DDRB = 0b00101000; //PB3 e PB5 como saída

    inic_LCD_4bits(); //inicializa o LCD
    escreve_LCD(msg3); //string armazenada na RAM

    while (1) {
        for (int i = 0; i < 999; i++) {
            _delay_ms(500);
            sprintf(buffer, "%.3d", i); //Converte a variável em uma string
            cmd_LCD(0x8B, 0); //Retorna o cursor para o início do número
            escreve_LCD(buffer); //Escreve a variável no display
        }
    }
}

int aula04() {
    
    DDRD = 0b11111111; //PORTD inteiro como saída
    DDRB = 0b00101000; //PB3 e PB5 como saída

    float i = 0;
    
    i = 3.15;
            
    inic_LCD_4bits(); //inicializa o LCD
    escreve_LCD(msg3); //string armazenada na RAM

    sprintf(buffer, "%.3f", i); //Converte a variável em uma string
    cmd_LCD(0x8b, 0);        //Retorna o curso para o início do caracter
    escreve_LCD(buffer); //Escreve a variável no display

    while (1) {

    }
}

void main() {
    //aula01();
    //aula02();
    aula03();
    //aula04();
}
