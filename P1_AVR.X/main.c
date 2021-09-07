/*Bibliotecas e frequência do uc*/
#define F_CPU 16000000        //define a frequencia do uC para 16MHz
#include <avr/io.h>           //Biblioteca geral dos AVR
#include <avr/interrupt.h>    //Biblioteca de interrupção
#include <stdio.h>            //Bilioteca do C
#include <util/delay.h>       //Biblioteca geradora de atraso
#include <avr/io.h>
#include <avr/pgmspace.h>   //para o uso do PROGMEM, gravação de dados na memória flash
#include "LCD.h"
#include "ADC.h"
#include <string.h>


//variáveis de comando para os registradores
#define set_bit(y,bit) (y|=(1<<bit)) //coloca em 1 o bit x da variável Y
#define clr_bit(y,bit) (y&=~(1<<bit)) //coloca em 0 o bit x da variável Y
#define cpl_bit(y,bit) (y^=(1<<bit)) //troca o estado lógico do bit x da variável Y
#define tst_bit(y,bit) (y&(1<<bit)) //retorna 0 ou 1 conforme leitura do bit
//==============================================================

/*Mapeamento de Hardware*/
#define botao1  PC3
#define botao2  PC2
#define botao3  PC0
#define display PORTB
#define D1      PD2
#define D2      PD3
#define D3      PC4
#define D4      PC5
/*Estruturas*/


//Variáveis globais
//================
int cont1 = 0, cont2 = 0, cont3 = 0, cont4 = 0, max_count1 = 0, max_count2 = 0, value_AD;
int ADC_dados;
char maq_display [16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07
                      , 0x7F, 0x6F, 0x77, 0x7F, 0x39, 0x3F, 0x79, 0x71};

char linha1[17] = "Linha1          ";
char linha2[17] = "Linha2          ";
char flag1 = 0, flag2 = 0;
char valor1 = 0, valor2 = 0, seq = 0;
char pausa = 0;
int N1 = 0, N2 = 0, N3 = 0, N4 = 0;
char reverso1 = 0, reverso2 = 0;
//Protótipo das funções
//================
void setup_hardware(void);
void setup(void);
void INT_init(void);
void f_timer0();
void f_timer1();
void f_timer2();
void ADC_maq();
void tempos (char valor, char seq);
void loop();
void display_maq();
void f_timers();
void sequencia1();
void sequencia2();
void f_int_ADC();

ISR(TIMER0_OVF_vect)
{   TCNT0 = 255;
    f_timers();

}

ISR(PCINT1_vect) {
    if (!tst_bit(PINC, botao1)) //botão1 pressionado?
    {
        valor1 ^= 0x01;
        seq = 1;
        tempos(valor1, seq);
    }

    
    else if (!tst_bit(PINC, botao2)) //botão2 pressionado?
    {
        valor2 ^= 0x01;
        seq = 2;
        tempos(valor2, seq);
    }
    
    else if (!tst_bit(PINC, botao3)) //botão2 pressionado?
    {
        pausa ^= 0x01;
    }


}

ISR(ADC_vect)
{
    ADC_maq();
}


int main() {
    
    setup_hardware();
    INT_init();
    ADC_init();
    //inic_LCD_4bits();
    sei();
    setup();

    while (1) {
        loop();

    }
    return 0;
}
void setup_hardware(void)
{
    MCUCR &= 0xef;
    DDRB = 0xff;    //Todo PORTB como saída
    PORTB = 0x00;   //inicia todos do PORTD em LOW
    DDRC = 0xf0;    //PC0-PC3 como entrada
    PORTC = 0x0d;   //entradas com pull up e PC1 sem
    DDRD = 0xff;    //Todo PORTD como saída
    PORTD = 0x00;   //iniciado em low
    DIDR0 = 0x02;   //desabilita entrada digital de PC1
}

void setup(void)
{
    max_count1 = 200;
    max_count2 = 200;
    ADC_maq();
    strcpy(linha1, "Johann");
    strcpy(linha2, "S1 C S2 C");
}

void loop()
{
    /*escreve_LCD(linha1);
    cmd_LCD(0xc0, 0); //vai pra linha de baixo
    escreve_LCD(linha2);*/
}
void INT_init(void)
{
    TCCR0B = 0b00000011; //TC0 com prescaler de 8
    TCNT0 = 255; //Inicia a contagem em 56 para, no final, gerar 1ms
    TIMSK0 = 0b00000001; //habilita a interrupção do TC0
    
    
    PCICR = 0x02; //Habilita interrupção do PCINT0
    PCMSK1 = 0x0d; //Habilita PCINT8 e PCINT10 e PCINT11
}


void f_timers(){

    display_maq();
    cont3++; cont4++;
    
    if(cont1 < max_count1) cont1++;
    else
    {
        f_timer0();
        cont1 = 0;
    }
    
    if(cont2 < max_count2) cont2++;
    else
    {
        f_timer1();
        cont2 = 0;
    }
    
    if(cont3 == 10)
    {
        f_int_ADC();
        cont3 = 0;
    }
    
    if(cont4 == 20)
    {
        f_timer2();
        cont4 = 0;
    }


}
void f_timer0()
{   
    if(!value_AD)
    {
        reverso1 = 0;
    }
    else if(value_AD == 1)
    {
        reverso1 = 1;
    }
    
    else if(value_AD == 2)
    {
        reverso1 = 0;
    }
    
    else
    {
        reverso1 = 1;
    }
    sequencia1();
        
        
}

void f_timer1()
{
    if(!value_AD)
    {
        reverso2 = 0;
        //strcpy(linha2, "S1 C S2 C");
    }
    else if(value_AD == 1)
    {
        reverso2 = 0;
        //strcpy(linha2, "S1 D S2 C");
    }
    
    else if(value_AD == 2)
    {
        reverso2 = 1;
        //strcpy(linha2, "S1 C S2 D");
    }
    
    else
    {
        reverso2 = 1;
        //strcpy(linha2, "S1 D S2 D");
    }
    sequencia2();
}

void ADC_maq () 
{

    ADC_conv_ch(1);
    ADC_dados = ADC_ler();
    
    
}

void tempos (char valor, char seq)
{   
    if(seq == 1 && valor)
    {
        max_count1 = 800;
    }
    
    else if(seq == 1 && !valor)
    {
        max_count1 = 200;
    }
    
    if(seq == 2 && valor)
    {
        max_count2 = 800;
    }
    
    else if(seq == 2 && !valor)
    {
        max_count2 = 200;
    }
    
}

void display_maq()
{
    static unsigned char estado = 0;
    switch(estado)
    {
        case 0:
            clr_bit(PORTC, D4);
            PORTB = maq_display[N2];
            set_bit(PORTD, D1);
            estado = 1;
            break;
        
        case 1:
            clr_bit(PORTD, D1);
            PORTB = maq_display[N1];
            set_bit(PORTD, D2);
            estado = 2;
            break;
        
        case 2:
            clr_bit(PORTD, D2);
            PORTB = maq_display[N4];
            set_bit(PORTC, D3);
            estado = 3;
            break;
            
        case 3:
            clr_bit(PORTC, D3);
            PORTB = maq_display[N3];
            set_bit(PORTC, D4);
            estado = 0;
            break;
        
    }
}

void sequencia1()
{   
    //S1 - 0 25 65 56 43 69 68 41 74 35 100 57 50 0
    static unsigned char numeros = 0;
    if(!pausa)
    {
        switch(numeros)
        {
            case 0:
                if(reverso1)
                {
                    numeros = 50;
                }
                else
                {
                    numeros = 25;
                }
                N1 = 0;
                N2 = 0;
                break;
            case 25:
                if(reverso1)
                {
                    numeros = 0;  
                }
                else
                {
                   numeros = 65; 
                }
                N1 = 2;
                N2 = 5;
                break;
                
            case 65:
                if(reverso1)
                {
                    numeros = 25;  
                }
                else
                {
                   numeros = 56; 
                }
                N1 = 6;
                N2 = 5;
                break;
            
            case 56:
                if(reverso1)
                {
                    numeros = 65;  
                }
                else
                {
                   numeros = 43; 
                }
                N1 = 5;
                N2 = 6;
                break;
                
            case 43:
                if(reverso1)
                {
                    numeros = 56;  
                }
                else
                {
                   numeros = 69; 
                }
                N1 = 4;
                N2 = 3;
                break;
                
            case 69:
                if(reverso1)
                {
                    numeros = 43;  
                }
                else
                {
                   numeros = 68; 
                }
                N1 = 6;
                N2 = 9;
                break;
             
            case 68:
                if(reverso1)
                {
                    numeros = 69;  
                }
                else
                {
                   numeros = 41; 
                }
                N1 = 6;
                N2 = 8;
                break;
                
            case 41:
                if(reverso1)
                {
                    numeros = 68;  
                }
                else
                {
                   numeros = 74; 
                }
                N1 = 4;
                N2 = 1;
                break;
             
            case 74:
                if(reverso1)
                {
                    numeros = 41;  
                }
                else
                {
                   numeros = 35; 
                }
                N1 = 7;
                N2 = 4;
                break;
                    
            case 35:
                if(reverso1)
                {
                    numeros = 74;  
                }
                else
                {
                   numeros = 100; 
                }
                N1 = 3;
                N2 = 5;
                break;
            
            case 100:
                if(reverso1)
                {
                    numeros = 35;  
                }
                else
                {
                   numeros = 57; 
                }
                N1 = 10;
                N2 = 0;
                break;
            
            case 57:
                if(reverso1)
                {
                    numeros = 100;  
                }
                else
                {
                   numeros = 50; 
                }
                N1 = 5;
                N2 = 7;
                break;
            
            case 50:
                if(reverso1)
                {
                    numeros = 57;  
                }
                else
                {
                   numeros = 0; 
                }
                N1 = 5;
                N2 = 0;
                break;
                
                
            default:
                N1 = 0;
                N2 = 0;
                break;
        }
    }
    
    else;
}

void sequencia2()
{   
    //S2 - 0 67 28 71 65 4 50 97 56 55 76 90 16 0
    static unsigned char numeros = 0;
    if(!pausa)
    {
        switch(numeros)
        {
            case 0:
                if(reverso2)
                {
                    numeros = 16;
                }
                else
                {
                    numeros = 67;
                }
                N3 = 0;
                N4 = 0;
                break;
            case 67:
                if(reverso2)
                {
                    numeros = 0;  
                }
                else
                {
                   numeros = 28; 
                }
                N3 = 6;
                N4 = 7;
                break;
                
            case 28:
                if(reverso2)
                {
                    numeros = 67;  
                }
                else
                {
                   numeros = 71; 
                }
                N3 = 2;
                N4 = 8;
                break;
            
            case 71:
                if(reverso2)
                {
                    numeros = 28;  
                }
                else
                {
                   numeros = 65; 
                }
                N3 = 7;
                N4 = 1;
                break;
                
            case 65:
                if(reverso2)
                {
                    numeros = 71;  
                }
                else
                {
                   numeros = 4; 
                }
                N3 = 6;
                N4 = 5;
                break;
                
            case 4:
                if(reverso2)
                {
                    numeros = 65;  
                }
                else
                {
                   numeros = 50; 
                }
                N3 = 0;
                N4 = 4;
                break;
             
            case 50:
                if(reverso2)
                {
                    numeros = 4;  
                }
                else
                {
                   numeros = 97; 
                }
                N3 = 5;
                N4 = 0;
                break;
                
            case 97:
                if(reverso2)
                {
                    numeros = 50;  
                }
                else
                {
                   numeros = 56; 
                }
                N3 = 9;
                N4 = 7;
                break;
             
            case 56:
                if(reverso2)
                {
                    numeros = 97;  
                }
                else
                {
                   numeros = 55; 
                }
                N3 = 5;
                N4 = 6;
                break;
                    
            case 55:
                if(reverso2)
                {
                    numeros = 56;  
                }
                else
                {
                   numeros = 76; 
                }
                N3 = 5;
                N4 = 5;
                break;
            
            case 76:
                if(reverso2)
                {
                    numeros = 55;  
                }
                else
                {
                   numeros = 90; 
                }
                N3 = 7;
                N4 = 6;
                break;
            
            case 90:
                if(reverso2)
                {
                    numeros = 76;  
                }
                else
                {
                   numeros = 16; 
                }
                N3 = 9;
                N4 = 0;
                break;
            
            case 16:
                if(reverso2)
                {
                    numeros = 90;  
                }
                else
                {
                   numeros = 0; 
                }
                N3 = 1;
                N4 = 6;
                break;
                
                
            default:
                N3 = 0;
                N4 = 0;
                break;
        }
    }
    
    else {};
}

void f_int_ADC()
{
    if(ADC_dados < 255)                             value_AD = 0;
    else if(ADC_dados > 255 && ADC_dados < 511)     value_AD = 1;
    else if(ADC_dados > 511 && ADC_dados < 767)     value_AD = 2;
    else                                            value_AD = 3;
    
}

void f_timer2()
{
    PORTD = (reverso1 << PD5) | (reverso2 << PD6);
}