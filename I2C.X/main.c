#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include "UART.h"
#include "I2C.h"

int main(void) {
    //Variáveis para armazernar a leitura do i2c
    unsigned char i2c_reg1 = 0;
    unsigned char i2c_reg2 = 0;
    I2C_Init(); //Configura o periférico TWI
    UART_config(); //Configura a comunicação UART
    while (1) {
    //Configura o dispositivo
    //------------------------------------------------------------
    I2C_Start(); //Condição de START
    I2C_Write(0x40); //Endereço do escravo + RW = 0
    I2C_Write(0x06); //Endereço do registrador
    I2C_Write(0x01); //IO0.0 como entrada e o restante saída
    I2C_Write(0x00); //IO1 como saída
    I2C_Stop(); //Condição de STOP
    //------------------------------------------------------------
    //Escreve nos pinos de saída
    //------------------------------------------------------------
    I2C_Start(); //Condição de START
    I2C_Write(0x40); //Endereço do escravo + RW = 0
    I2C_Write(0x02); //Reistrador de saída
    I2C_Write(0xAA); //IO0 = AAh
    I2C_Write(0x55); //IO1 = 55h
    I2C_Stop(); //Condição de STOP
    //------------------------------------------------------------
    //Faz a leitura dos registradores
    //------------------------------------------------------------
    I2C_Start(); //Condição de START
    I2C_Write(0x40); //Endereço do escravo + RW = 0
    I2C_Write(0x00); //Registrador que faremos a leitura
    I2C_Start(); //Envia um Re-START
    I2C_Write(0x41); //Endereço do escravo + RW = 1
    i2c_reg1 = I2C_Read(1); //Faz a leitura e Envia um ACK para continuar
    i2c_reg2 = I2C_Read(0); //Faz a leitura e Envia um NACK para finalizar
    I2C_Stop(); //Condição de STOP
    //------------------------------------------------------------
    UART_enviaCaractere(i2c_reg1);
    UART_enviaCaractere(i2c_reg2);
    _delay_ms(1000);
    }
}