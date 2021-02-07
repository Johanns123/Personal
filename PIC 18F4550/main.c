/*
 * File:   main.c
 * Author: Luis Alexandre
 *
 * Created on 2 de Fevereiro de 2021, 19:28
 */
#include "configbit.txt" //configurações dos bits
#include <xc.h>
#include <pic18F4550.h>
#include <stdio.h>
#define _XTAL_FREQ 20000000 //Frequencia em 20MHz

void aula01(){
    //código que acende e apagua o led
    
    TRISB = 0b00000000; //declara todas as portas do PORTB como saída
    PORTB = 0b00000000; //Inicializa todas em 0
    
    while(1){
        RB1 = 1; //Porta RB1
        __delay_ms(100);
        
        RB1 = 0;
        __delay_ms(100);
    }
}

void aula02(){
    //código que vai acendendo os leds em sequência de bits
    
//         RB 76543210
    TRISB = 0b00000000;
    PORTB = 0b00000000;
    
    while(1){
        PORTB++; //incrementando no PORTB
        __delay_ms(500);
        
    }
}


void aula03(){
    //código que acende e apaga dois leds simultaneamentes
    
    TRISB = 0b00000000;
    PORTB = 0;
    while(1){
        RB0 = 1;
        RB1 = 0;
        __delay_ms(100);
        
        RB0 = 0;
        RB1 = 1;
        __delay_ms(100);
    }
}


void aula04(){ //Esse código 
    //faz com que a porta 0 leia bits e faça com que a porta 1 acenda um led
    //quando a outra está em sinal alto
    
    
    
    TRISB = 0b00000001; //somente RB0 decretado com entrada
    
    PORTB = 0;
    
    
    while(1){
        if(RB0 == 1){
            RB1 = 1;
            __delay_ms(2000);
        }
        
        else{
            RB1 = 0;
        }
    
    }

}


void aula05(){
    //código que analisa a função de delay e mostra que as funções são lidas em série
    
    TRISD = 0b00000000;
    PORTD = 0b00000000;
    
    while(1){
        RD0 = 1;
        __delay_ms(10);
        RD0 = 0;
        __delay_ms(10);
        
        RD1 = 1;
        __delay_ms(1);
        RD1 = 0;
        __delay_ms(1);
        
    }

}


#define botao1 RC0
#define botao2 RC1
#define D1 RD0
#define D2 RD1 //define a porta com um nome, LED no caso*/

void aula06(){
    //código que acende e apaga o led através de um botão
    
    CMCON = 7; //Serve para alterar a função da porta entre A e D 7=111 em binário
    // nessa configuração RA0, RA1, RA2 e RA3 como portas digitais.
    
    TRISC = 0b00000011; //RC0 e RC1 como entradas digitais
    TRISD = 0b00000000;
    
    botao1 = 1;
    botao2 = 1;
    D1 = 0;
    D2 = 0;
    
    
    while(1){
        if(botao1 == 0){
            D1 = ~D1; //~ significa o inverso de
            __delay_ms(300);
        }
        
        if(botao2 == 0){
            D2 = ~D2;
            __delay_ms(300);
        }
    }


}


void aula07(){
    //código que acende os leds em cadeia com shift left
    
    int control = 0xFF;
    
    TRISD = 0; //Todos os terminais estão como saída
    PORTD = 0x00; //Inicia todos em 0, notação em hexadecimal
    
    while(1){
        PORTD = ~control;  //identifica as portas com valores inversos ao do control
        __delay_ms(300);
        
        control = control << 1;  //shift left = deslocamento de bit para esquerda.
                                //Do menos significativo para o mais significativo
        
        if(control == 0){
            control = 0xFF;
        }
        
    }
    
    
}

#define botao1 RC0 
#define botao2 RC1
#define led1 RD6
#define led2 RD7

void aula08(){
    CMCON = 7;
    TRISC = 0x03;
    TRISD = 0x00;
    PORTC = 0x03;
    
    while(1){
        if(botao1 == 0){
            while(1){//programa 1
                led1 = 1;
                led2 = 0;
                __delay_ms(500);
                led1 = 0;
                led2 = 1;
                __delay_ms(500);
            
            }
        }
        
        if(botao2 == 0){
            while(1){//programa 2
                led1 = 1;
                led2 = 1;
                __delay_ms(500);
                led1 = 0;
                led2 = 0;
                __delay_ms(500);
            
            }
        }
    }
    
}

int numbers;

void aula09(){
    CMCON = 7; //desbilita os comparadores
    TRISD = 0x00; 
    TRISC = 0x03; //indica os bits menos significativos como entrada
    
    PORTC = 0x03;
    PORTD = 0x00;
    
    while(1){
        numbers = rand();
        
        led1 = numbers;
        
        led2 = ~led1;
        
        
        while(led1) //enquanto led1 for verdadeiro
        {
            if(botao1 == 0){
                led1 = 0;
                __delay_ms(70);
            }
        }
        
        while(led2){
            
            if(botao2 == 0){
                led2 =0;
                __delay_ms(70);
            }
        
        }
        __delay_ms(500);
        
    
    }
    

}


#define botao1 RC0 
#define botao2 RC1
#define led1 RD6
#define led2 RD7
#define servo RB0

void servo_horario();
void servo_centro();
void servo_antihorario();


void aula10(){
    TRISC = 0x03;
    TRISD = 0x00;
    TRISB = 0x00;
    
    PORTC = 0x03;
    PORTB = 0x00;
    
    while(1){
        //se manter botao1 pressionado
        //... servo move-se no sentido horário
        if(botao1 == 0x00 && botao2 == 0x01){
            servo_horario();
        }
        
        //se manter botao2 pressionado
        //... servo move-se no sentido anti-horario
        else if(botao2 == 0x00 && botao1 == 0x01){
            servo_antihorario();
        }
        
        //senão centraliza o servo
        else servo_centro();
    }


}

void servo_horario(){
    servo = 0x00;
    __delay_us(18000); //puslo baixo de 18ms pois o período é de 20ms
    servo = 0x01;
    __delay_us(2000); //pulso de 2ms --> duty cycle de 10%
}
    
void servo_antihorario(){
    servo = 0x00;
    __delay_us(19000);
    servo = 0x01;
    __delay_us(1000); //pulso de 1ms
}
    
    
void servo_centro(){
    servo = 0x00;
    __delay_us(18500);
    servo = 0x01;
    __delay_us(1500); //pulso de 1,5ms
}





/*Informações do datasheet

TACQmin = 2.45us
TDAmin = 0.8us

Informações de montagem

FOSC = 20MHz
TOSC = 1/FOSC = 50ns

ADCS = 0.8u / 50n = 16s --> 16

TDA = 16 * TOSC = 0.800us

ACQT = 2.45u / 0.8u = 3.07us --> 4

TACQ = 4 * TDA = 3.2us

tempo de conv = 11*TDA + TACQ = 12us*/

#define led1 RD7
#define led2 RD6
#define led3 RD5
#define led4 RD4
#define led5 RD3
#define led6 RD2
#define led7 RD1
#define led8 RD0

int leitura1 = 0;
int leitura2 = 0;

void aula11(){ // esse código está funcionando parcialmente
    
    //código de conversão AD usando trimpot
    TRISD = 0x00;
    ADCON1 = 0b00001110; // --> 1110 determina somente RA1 como analógico
    ADCON2 = 0b10010101; //para obter esses valores recorra ao datasheet e às informações acima
    CMCON = 7;
    TRISA = 0b00000011;
    PORTD = 0x00; // todos os leds desligados
    
    while(1){//é necessário sempre chamar o registrador ADCON
             //toda vez que for ler outra porta analógica
             //pois o 18F4550 possui um multiplexador nas portas analógicas
        
        ADCON0 = 0b00000001;// Seleciona canal AN0
        ADCON0bits.GO=1; // inicia a conversão
        while (ADCON0bits.GO); // Aguarda a o fim da conversão
        leitura1 = ADRESL + (ADRESH << 8);
        
        ADCON0 = 0b00000101;// Seleciona canal AN1
        ADCON0bits.GO=1; // inicia a conversão
        while (ADCON0bits.GO); // Aguarda a o fim da conversão
        leitura2 = ADRESL + (ADRESH << 8);
        
        
        //int leitura_total[] = {leitura1, leitura2};
        //int difrenca_erro = leitura_total[0] - leitura_total[1];
        
        if(leitura1 > 0 && leitura1 <= 204){
            led1 = 0;
            led2 = 0;
            led3 = 0;
            led4 = 0;
        }
        if(leitura1 > 204 && leitura1 <= 408){
            led1 = 1;
            led2 = 0;
            led3 = 0;
            led4 = 0;
        }
        if(leitura1 > 408 && leitura1 <= 612){
            led1 = 1;
            led2 = 1;
            led3 = 0;
            led4 = 0;
        }
        if(leitura1 > 612 && leitura1 <= 860){
            led1 = 1;
            led2 = 1;
            led3 = 1;
            led4 = 0;
        }
        if(leitura1 > 860 && leitura1 <= 1023){
            led1 = 1;
            led2 = 1;
            led3 = 1;
            led4 = 1;
        }
        if(leitura2 > 0 && leitura2 <= 204){
            led5 = 0;
            led6 = 0;
            led7 = 0;
            led8 = 0;
        }
        if(leitura2 > 204 && leitura2 <= 408){
            led5 = 1;
            led6 = 0;
            led7 = 0;
            led8 = 0;
        }
        if(leitura2 > 408 && leitura2 <= 612){
            led5 = 1;
            led6 = 1;
            led7 = 0;
            led8 = 0;
        }
        if(leitura2 > 612 && leitura2 <= 860){
            led5 = 1;
            led6 = 1;
            led7 = 1;
            led8 = 0;
        }
        if(leitura2 > 860 && leitura2 <= 1023){
            led5 = 1;
            led6 = 1;
            led7 = 1;
            led8 = 1;
        }
    }
        __delay_ms(500); //taxa de atualização do ADC*/
}
        
void aula12(){ //código para exibir no display de 7 segmentos
    unsigned char catodo, cont = 0x00;
    unsigned char segmento [] ={0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67}; //determina o que será mostrado no display convertido em hexadecimal
                                //valores de 0 a 9 em hexadecimal.
    CMCON = 0x07;
    TRISD = 0x00;
    PORTD = 0x00; //inicializa todos os leds em zero
    
    
    while(1){
        catodo = segmento [cont]; //número para ser enviado no PORTB ex.: segmento[3] amostra o valor 3
        PORTD = catodo; //envia o padrão no PORTB
        cont++; //incrementa no cont paar amostragem no display ir se alterando
        
        if(cont == 10){
            cont = 0;
        }
        __delay_ms(1000);
    
    
    }
}


void aula13(){
    //este programa faz a contagem de dois displays de 7 segmentos de 1 a 99 usando o CI 4511
    unsigned char catodo1, catodo2, cont1, cont2 = 0x00;
    unsigned char segmento1 [] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    unsigned char segmento2 [] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    CMCON = 0x07;
    TRISB = 0x00;
    TRISD = 0x00;
    
    PORTB = 0x00;
    PORTD = 0x00;
    
    while(1){
        catodo1 = segmento1 [cont1]; //número para ser enviado no PORTB ex.: segmento[3] amostra o valor 3
        PORTD = catodo1; //envia o padrão no PORTB
        cont1++; //incrementa no cont paar amostragem no display ir se alterando
        
        catodo2 = segmento2 [cont2]; //número para ser enviado no PORTB ex.: segmento[3] amostra o valor 3
        PORTB = catodo2; //envia o padrão no PORTB
        
        if(cont1 == 10){
        cont1 =0;
        cont2++;
        }
        if(cont2 == 10){
            cont1 = 0;
            cont2 = 0;
        }
        __delay_ms(100);
    }
    



}


#define led1 RD7
#define led2 RD6
#define botao1 RC0
#define botao2 RC1

void aula14(){
    CMCON = 0x07; //desabilita os comparadores
    TRISD = 0x00;
    TRISC = 0x03;
    PORTC = 0x03;
    PORTD = 0x00;
    
    
    while(1){
        led1 = 0x01;
        led2 = 0x00;
        __delay_ms(10);
        led1 = 0x00;
        led2 = 0x01;
        __delay_ms(10);
    
    
    }

}

void aula15(){


}

void main(void) {
    //aula01();
    //aula02();
    //aula03();
    //aula04();
    //aula05();
    //aula06();
    //aula07();
    //aula08();
    //aula09();
    //aula10();
    //aula11();
    //aula12();
    //aula13();
    //aula14();
    aula15();
    
    
    
    

}

