//--------------------------------------------------------------------------------------------- //
//		AVR e Arduino: T�cnicas de Projeto, 2a ed. - 2012.										//	
//--------------------------------------------------------------------------------------------- //
//=============================================================================================	//
//		Sub-rotinas para o trabalho com um LCD 16x2 com via de dados de 4 bits					//	
//		Controlador HD44780	- Pino R/W aterrado													//
//		A via de dados do LCD deve ser ligado aos 4 bits mais significativos ou					//
//		aos 4 bits menos significativos do PORT do uC											// 																
//=============================================================================================	//

//Defini��es para facilitar a troca dos pinos do hardware e facilitar a re-programa��o
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>   //para o uso do PROGMEM, gravação de dados na memória flash

#define DADOS_LCD    	PORTD  	//4 bits de dados do LCD no PORTD 
#define nibble_dados	0		//0 para via de dados do LCD nos 4 LSBs do PORT empregado (Px0-D4, Px1-D5, Px2-D6, Px3-D7) 
								//1 para via de dados do LCD nos 4 MSBs do PORT empregado (Px4-D0, Px5-D1, Px6-D2, Px7-D3) 
#define CONTR_LCD 		PORTB  	//PORT com os pinos de controle do LCD (pino R/W em 0).
#define E    			PB3     //pino de habilita��o do LCD (enable)
#define RS   			PB5     //pino para informar se o dado � uma instru��o ou caractere

#define tam_vetor	5	//n�mero de digitos individuais para a convers�o por ident_num()	 
#define conv_ascii	48	//48 se ident_num() deve retornar um n�mero no formato ASCII (0 para formato normal)

//sinal de habilita��o para o LCD
#define pulso_enable() 	_delay_us(1); LCD_set_bit(CONTR_LCD,E); _delay_us(1); LCD_clr_bit(CONTR_LCD,E); _delay_us(45)

#define	LCD_set_bit(y,bit)	(y|=(1<<bit))	//coloca em 1 o bit x da vari�vel Y
#define	LCD_clr_bit(y,bit)	(y&=~(1<<bit))	//coloca em 0 o bit x da vari�vel Y
#define LCD_cpl_bit(y,bit) 	(y^=(1<<bit))	//troca o estado l�gico do bit x da vari�vel Y
#define LCD_tst_bit(y,bit) 	(y&(1<<bit))	//retorna 0 ou 1 conforme leitura do bit

//prot�tipo das fun��es
void cmd_LCD(unsigned char c, char cd);
void inic_LCD_4bits();		
void escreve_LCD(char *c);
void escreve_LCD_Flash(const char *c);
void ident_num(unsigned int valor, unsigned char *disp);