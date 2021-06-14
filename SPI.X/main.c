#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "UART.h"
unsigned char SPI(unsigned char dado);

int main(void) {
    char buffer[6]; //Buffer para conversão em string
    unsigned char adcHigh = 0, adcLow = 0; //Variáveis temporárias da conversão
    unsigned int adcTotal = 0; //Variável da conversão
    //--------------------------------------------------------
    //configuração dos pinos de entrada e saída da SPI
    DDRB = 0b00101100; //PB5(MOSI), PB4(MISO), PB3(SCK), PB2(/SS)
    /*
     * Habilita a SPI (SPE = 1)
     * Configura como Master (MSTR = 1)
     * Fosc/64 (SPR1 = 1)
     * Leitura do dado sem atraso (CPHA = 0)
     * Clock não invertido (CPOL = 0)
     */
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (0 << CPHA) | (0 << CPOL);
    UART_config(); //Configura a comunicação UART
    while (1) {
        PORTB &= 0b11111011; //Ativa o dispositivo (SS = 0)
        SPI(0b00000100); //START + Differential Conversion + D2 = 0
        adcHigh = SPI(0b00000000); //D1 e D0 = 0 (Leitura de CH0 - CH1)
        adcHigh &= 0x0F; //Nibble mais significativo da conversão
        adcLow = SPI(0b00000000); //Nibble menos significativo da conversão
        PORTB |= 0b00000100; //Desativa o dispositivo (SS = 1)
        //Junta os valores high e low para formar o valor total da conversão (12 bits)
        adcTotal = (adcHigh << 8) | adcLow;
        sprintf(buffer, "%4d", adcTotal); //Converte o valor para string
        UART_enviaString(buffer); //Envia a string pela UART
        UART_enviaCaractere(13); //Pula uma linha no terminal
        _delay_ms(500);
    }
}

unsigned char SPI(unsigned char dado) {
    SPDR = dado;
    while (!(SPSR & (1 << SPIF))); //espera envio
    return SPDR;
}