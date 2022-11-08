/* ===========================================================================

    Projeto da Aula 3 do M�dulo 2 Curso AVR Projetos Profissionais
	
	LCD com microcontrolador AVR

    ARQUIVO lcd.h
    
    Projeto Principal

    MCU:   Atmega328p
    Clock: 16MHz Cristal Externo

    Compilador: Microchip Studio 7.0.2542
    Autor: Dr. Eng. Wagner Rambo
    Data de cria��o: Mar�o de 2022
    �ltima atualiza��o: 19/03/2022

============================================================================ */


/* ========================================================================= */
/* --- Frequ�ncia de Clock --- */
#define    F_CPU    16000000									          /* configurado para 16MHz */


/* ========================================================================= */
/* --- Bibliotecas Auxiliares --- */
#include <avr/io.h>										                  /* biblioteca de entradas e sa�das padr�o */
#include <util/delay.h>													  /* biblioteca com as fun��es de delay */


/* ========================================================================= */
/* --- Mapeamento de Hardware --- */
#define      reg1       PORTB                  //registrador 1 utilizado nas IOs
#define		 reg2       PORTD                  //registrador 2 utilizado nas IOs
#define      RS         PB0                    //pino register select do LCD
#define      EN         PB1                    //pino enable do LCD
#define      D4         PD4                    //pino de dados DB4 do LCD
#define      D5			PD5                    //pino de dados DB5 do LCD
#define      D6			PD6                    //pino de dados DB6 do LCD
#define      D7			PD7                    //pino de dados DB7 do LCD
#define      CONTR      PD3                    //sa�da para gerar PWM para o ajuste de contraste


/* ========================================================================= */
/* --- Macros --- */
#define   set_bit(reg,bit)		(reg |= (1<<bit))		   /* seta um bit de determinado registrador */
#define   clr_bit(reg,bit)		(reg &= ~(1<<bit))		   /* limpa um bit de determinado registrador */
#define   test_bit(reg,bit)      (reg &   (1<<bit))         /* l� um bit de determinado registrador */


/* ========================================================================= */
/* --- Prot�tipo das Fun��es --- */
void disp_number(unsigned num, char row, char col);        /* converte um inteiro de at� 5 d�gitos para exibir no display, remove zeros � esquerda */
void disp_wr_po(unsigned char chr, char row, char col);    /* fun��o para escrever caracteres no LCD na posi��o indicada */
void disp_text(char *str, char row, char col);             /* fun��o para escrever uma string no LCD */
void disp_write(unsigned char chr);                        /* fun��o para escrever caracteres no LCD */
void disp_cmd(unsigned char cmd);                          /* fun��o para enviar comandos para o LCD*/
void disp_init(void);                                      /* fun��o para inicializar o LCD */
void disp_clear(void);                                     /* fun��o para limpar o LCD */
void send_nibble(unsigned char nib, char rsel);            /* envia cada nibble separadamente e gera pulso em enable */


/* ============================================================================

                                       _
                                      / \
                                     |oo >      <-- (Hello, liquid crystal display!)
                                     _\=/_
                    ___         #   /  _  \   #
                   /<> \         \\//|/.\|\\//
                 _|_____|_        \/  \_/  \/
                | | === | |          |\ /|
                |_|  0  |_|          \_ _/
                 ||  0  ||           | | |
                 ||__*__||           | | |
                |* \___/ *|          []|[]
                /=\ /=\ /=\          | | |
________________[_]_[_]_[_]_________/_]_[_\_______________________________


============================================================================ */
/* --- Final do Arquivo lcd.h --- */