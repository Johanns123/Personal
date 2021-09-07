/*Bibliotecas e frequência do uc*/
#define F_CPU 16000000        //define a frequencia do uC para 16MHz
#include <avr/io.h>           //Biblioteca geral dos AVR
#include <avr/interrupt.h>    //Biblioteca de interrupção
#include <util/delay.h>       //Biblioteca geradora de atraso
#include <avr/io.h>
#include "ADC.h"

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
int cont1 = 0, cont2 = 0, cont3 = 0, cont4 = 0, max_count1 = 0, max_count2 = 0;
int ADC_dados;
char maq_display [16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07
                      , 0x7F, 0x6F, 0x77, 0x7F, 0x39, 0x3F, 0x79, 0x71};
unsigned char VoltCen = 0, VoltDez = 0, VoltUni = 0;
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

    }

    
    else if (!tst_bit(PINC, botao2)) //botão2 pressionado?
    {

    }
    
    else if (!tst_bit(PINC, botao3)) //botão2 pressionado?
    {

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
    MCUCR &= 0xef;  //Pull up interno habilitado
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
    //max_count1 = 200;
    //max_count2 = 200;
    ADC_maq();
}

void loop()
{

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
    cont3++;// cont4++;
    
   /* if(cont1 < max_count1) cont1++;
    else
    {
        f_timer0();
        cont1 = 0;
    }*/
    
    /*if(cont2 < max_count2) cont2++;
    else
    {
        f_timer1();
        cont2 = 0;
    }*/
    
    if(cont3 == 10)
    {
        f_int_ADC();
        cont3 = 0;
    }
    
    /*if(cont4 == 20)
    {
        f_timer2();
        cont4 = 0;
    }*/


}
void f_timer0()
{   

    sequencia1();
        
        
}

void f_timer1()
{

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
            PORTB = maq_display[VoltUni];
            set_bit(PORTD, D1);
            estado = 1;
            break;
        
        case 1:
            clr_bit(PORTD, D1);
            PORTB = maq_display[VoltDez];
            set_bit(PORTD, D2);
            estado = 2;
            break;
        
        case 2:
            clr_bit(PORTD, D2);
            PORTB = maq_display[VoltCen] | (1<<PB7);    //número e ponto decimal
            set_bit(PORTC, D3);
            estado = 3;
            break;
            
        case 3:
            clr_bit(PORTC, D3);
            PORTB = maq_display[0];
            set_bit(PORTC, D4);
            estado = 0;
            break;
        
    }
}

/*void sequencia1()
{   
    
}

void sequencia2()
{   
    
}*/

void f_int_ADC()
{
    static unsigned int volt = 0;
    
    volt = ADC_dados * 500.0 / 1023.0;
    VoltCen = volt/100;      //429/100 = 4,29 = 4
    VoltDez = (volt%100)/10; //429%100 = 4,29 -> 29/10 = 2,9 = 2
    VoltUni =  volt%10;       //429%10 = 42,9 = 9
    
}

void f_timer2()
{

}