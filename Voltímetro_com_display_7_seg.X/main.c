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
#define display PORTD
#define D1      PB0
#define D2      PB1
#define D3      PB4
#define D4      PB5
/*Estruturas*/


//Variáveis globais
//================
int cont1 = 0, cont2 = 0, cont3 = 0, cont4 = 0, max_count1 = 0, max_count2 = 0;
int ADC_dados[6];
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
{   TCNT0 = 240;
    f_timers();

}

ISR(PCINT1_vect) {



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
    //DDRC = 0xf0;    //PC0-PC3 como entrada
    //PORTC = 0x0d;   //entradas com pull up e PC1 sem
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
    TCCR0B = 0b00000101; //TC0 com prescaler de 1024
    TCNT0 = 240; //Inicia a contagem em 56 para, no final, gerar 1ms
    TIMSK0 = 0b00000001; //habilita a interrupção do TC0
    
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

   
        
        
}

void f_timer1()
{

    
}

void ADC_maq () 
{

    static unsigned char estado = 10;
    
    switch (estado) {
        
        case 0:
            estado = 1;
            ADC_dados[0] = ADC_ler();
            ADC_conv_ch(1);
            break;
            
        case 1:
            estado = 2;
            ADC_dados[1] = ADC_ler();
            ADC_conv_ch(2);
            break;
            
        case 2:
            estado = 3;
            ADC_dados[2] = ADC_ler();
            ADC_conv_ch(3);
            break;
            
        case 3:
            estado = 4;
            ADC_dados[3] = ADC_ler();
            ADC_conv_ch(4);
            break;
            
        case 4:
            estado = 5;
            ADC_dados[4] = ADC_ler();
            ADC_conv_ch(5);
            break;
            
        case 5:
            estado = 0;
            ADC_dados[5] = ADC_ler();
            ADC_conv_ch(0);
            break;
            
        default:
            estado = 0;
            ADC_conv_ch(0);
            ADC_dados[0] = ADC_ler();
            break; 
    }
    
}

void tempos (char valor, char seq)
{   
    
}

void display_maq()
{
    static unsigned char estado = 0;
    switch(estado)
    {
        case 0:
            set_bit(PORTB, D4);;
            PORTD = 0x3e;  //Printar 'V'
            clr_bit(PORTB, D1);
            estado = 1;
            break;
        
        case 1:
            set_bit(PORTB, D1);
            PORTD = maq_display[VoltUni];
            clr_bit(PORTB, D2);
            estado = 2;
            break;
        
        case 2:
            set_bit(PORTB, D2);
            PORTD = maq_display[VoltDez];
            clr_bit(PORTB, D3);
            estado = 3;
            break;
            
        case 3:
            set_bit(PORTB, D3);
            PORTD = maq_display[VoltCen] | (1<<PD7);    //número e ponto decimal
            clr_bit(PORTB, D4);
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
    
    volt = ADC_dados[5] * 500.0 / 1023.0;
    VoltCen = volt/100;      //429/100 = 4,29 = 4
    VoltDez = (volt%100)/10; //429%100 = 4,29 -> 29/10 = 2,9 = 2
    VoltUni =  volt%10;       //429%10 = 42,9 = 9
    
}

void f_timer2()
{

}