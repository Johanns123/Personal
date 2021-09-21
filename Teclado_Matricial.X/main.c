/*
 * File:   main.c
 * Author: johannsamorim
 *
 * Created on 20 de Setembro de 2021, 18:38
 */
/*
#include <xc.h>
#define _XTAL_FREQ 4000000
#include "configbits.txt"
#include <string.h>
#include "lcd.h"
#include "pwm.h"


//Mapeamento de Hardware
#define linha1  LATB3
#define linha2  LATB4
#define linha3  LATB5
#define linha4  LATA5

#define disp1 LATA2
#define disp2 LATA3
#define disp3 LATA4
#define disp4 LATA5
#define display LATD


//Variáveis globais
//================
char linha01[17]="LCD linha1      ", linha02[17]="LCD linha2      ";

unsigned int fan = 0;
unsigned int max_timer0 = 200, max_timer1 = 200, max_timer2 = 200;
int cont1 = 0, cont2 = 0, cont3 = 0, cont4 = 0, max_count1 = 0, max_count2 = 0;
unsigned char controle = 0x01;
unsigned char numero = 0x00;
unsigned char n1=1, n2=2, n3, n4;
unsigned char fLCDnew=0;
int valorADC;
unsigned char fADCnew=0;
unsigned char contador, pausa, reverso;


//Protótipo das funções
//================
void setup_hard(void);
void setup (void);
void loop (void);
void f_int0 (void);
void f_int1 (void);
void f_int2 (void);
void maqdisplay (void);
void keybdisplay(void);
void f_timers(void);
void f_timer0 (void);
void f_timer1 (void);
void f_timer2 (void);
void fint_ADC(void);
void mostraLCD (void);
void entrada();

unsigned char conv_7seg[17] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07
                      , 0x7F, 0x6F, 0x77, 0x7F, 0x39, 0x3F, 0x79, 0x71};

void __interrupt (high_priority) f_interrupt_high (void) {
   
    if (TMR0IE && TMR0IF) {
        TMR0L += 6;
        f_timers();
        TMR0IF=0;
    }
    

    if (ADIE && ADIF) {
        ADIE = 0;
        fint_ADC();
        ADIF=0;     
    }
    
    
}

void __interrupt (low_priority) f_interrupt_low (void) {
    
    
    
}

void f_timers(void) {
    
    static unsigned int cont_timer0=0, cont_timer1=0, cont_timer2=0, cont_timer3 = 0;
    static char cont_disp=0;
    
    cont_timer3++;
    
    TRISB6 = 0;
    if (LATB6) LATB6=0; else LATB6=1;
    
    if (cont_disp < 4) {
        cont_disp++;
    }
    
    else {
        cont_disp=0;
        
        if (fLCDnew==0) {
        maqdisplay();
        keybdisplay();
        }
    }
    
    
    if (cont_timer0 < max_timer0) {
        cont_timer0++;
    }
    
    else {
        cont_timer0=0;
        f_timer0();
    }
    
    if (cont_timer1 < max_timer1) {
        cont_timer1++;
    }
    else {
        cont_timer1=0;
        f_timer1();
    }
    
    if (cont_timer2 < max_timer2) {
        cont_timer2++;
    }
    
    else {
        cont_timer2=0;
        f_timer2();
    }
    
    if(cont_timer3 == 10)
    {
        entrada();
    }
    
}

void setup_hard (void) {
    
    IPEN = 0;
    GIE = 0;
    
    // PORTAS ENTRADA E SAIDA
    
    ADCON1 = 0b00001111;
    
    TRISA = 0b11000011;
    TRISB = 0b11111111;
    TRISD = 0b00000000;
    TRISE = 0b11111001;
    
    
    // INTERRUPCAO
    
    
    //timer0 fosc=4000000 tempo=1e-3
    T0CON = 0b11010001;
    TMR0L = 6;
    
    // AD  [TADmin = 0,7us  TACQmin = 2,4us  Fosc = 4MHz]
    
    ADCON0 = 0b00000001; //AN0
    ADCON1 = 0b00001110; 
    ADCON2 = 0b10010100;
    
    disp1 = 0; disp2 = 0; disp3 = 0; disp4 = 0;
    display = conv_7seg[16];
    
    lcd_init();
    
    PWM_Init();
    
    setup(); // SETUP LOGICO
    
    
    // HABILITANDO OU NAO INTERRUPCAO
    
    PEIE = 1;
    GIE = 1;
}



void main (void) {
    
    setup_hard();
    while(1) {
        
        if(fLCDnew) {
        mostraLCD();
        fLCDnew=0; 
        }
        
        loop();
    }
    
}

void maqdisplay (void) {
    
    static unsigned char estado = 1;
    
    
    switch (estado) {
        
        case 1:
            estado = 2;
            disp4 = 0;
            display = conv_7seg[n1];
            disp1 = 1;
            break;
            
        case 2:
            estado = 3;
            disp1 = 0;
            display = conv_7seg[n2];
            disp2 = 1;
            break;
            
        case 3:
            estado = 4;
            disp2 = 0;
            display = conv_7seg[n3];
            disp3 = 1;
            break;
            
        case 4:
            estado = 1;
            disp3 = 0;
            display = conv_7seg[n4];
            disp4 = 1;
            break;
            
        default:
            estado = 1;
            disp1 = 0; disp2 = 0; disp3 = 0; disp4 = 0;
            display = conv_7seg[16];
            
    }
    
}

void keybdisplay (void) {
    
    if(linha1 && controle == 0x01)
    {
        controle = 0x02;
        linha1 = 0x00;
        linha2 = 0x01;
        linha3 = 0x01;
        linha4 = 0x01;
    
        if(!RB0)
        {
            numero = 1;
        }
        else if(!RB1)
        {
            numero = 2;
        }
        
        else if(!RB2)
        {
            numero = 3;
        }
    }
    
    
    else if(linha2 && controle == 0x02)
    {
        controle = 0x03;
        linha1 = 0x01;
        linha2 = 0x00;
        linha3 = 0x01;
        linha4 = 0x01;
        
        if(!RB0)
        {
            numero = 4;
        }
        else if(!RB1)
        {
            numero = 5;
        }
        
        else if(!RB2)
        {
            numero = 6;
        }
    }
    
    else if(linha3 && controle == 0x03)
    {
        controle = 0x04;
        linha1 = 0x01;
        linha2 = 0x01;
        linha3 = 0x00;
        linha4 = 0x01;
        
        if(!RB0)
        {
            numero = 7;
        }
        else if(!RB1)
        {
            numero = 8;
        }
        
        else if(!RB2)
        {
            numero = 9;
        }
        
        
    }
    
    else if(linha4 && controle == 0x04)
    {
        controle = 0x01;
        linha1 = 0x01;
        linha2 = 0x01;
        linha3 = 0x01;
        linha4 = 0x00;
        
        if(!RB0)
        {
            numero = 0x2a;      //'*'   42Dec
        }
        else if(!RB1)
        {
            numero = 0;
        }
        
        else if(!RB2)
        {
            numero = 0x23;      //'#'   35Dec
        }
    }
    
}


// PARTE LOGICA


void setup (void) {
    
    max_timer0 = 50 - 1;
    max_timer1 = 200 - 1; // TEMPO DE AMOSTRAGEM DO ESTADO
    max_timer2 = 200 - 1;
    
    
    strcpy(linha01, "MARC_MATH_RODR");
    
}

void loop (void) {     
    valorADC = lerADC();
    if (fADCnew) {
    
        fADCnew=0;
   
    }
}

void startADC ( void) {
    
    ADIF = 0;
    ADIE = 1;
    GO = 1;
    
}

int lerADC (void) {
    
    int valor = 0;
    valor = ADRESH;
    valor = valor<<8;
    valor += ADRESL; 
                                            
    return valor;
    
}

void mostraLCD (void) {
    
    extern char linha01[17], linha02[17];
    
    disp1 = 0; disp2 = 0; disp3 = 0; disp4 = 0;
    
    lcd_cmd(L_CLR);
    
    lcd_cmd(L_L1);
    lcd_str(linha01);
    
    lcd_cmd(L_L2);
    lcd_str(linha02);
      
    display = conv_7seg[16];
    
}

void f_int0 (void) 
{     
    fan = 0;
    PWM1_Set_Duty(fan);
}


void f_int1 (void) {
    
    fan = 500;
    PWM1_Set_Duty(fan);
}


void f_int2 (void) {

}


void f_timer0 (void) {
    startADC();
     
}


void f_timer1 (void) {
    
    
}


void f_timer2 (void) {
    
    
    
}


void fint_ADC (void) {
    
    valorADC = lerADC();
    fADCnew=1;
    
}

void entrada()
{
    static unsigned char rb3_old=1, rb4_old=1, rb5_old=1;
    
    unsigned char rb3_new, rb4_new, rb5_new;
    
    // amostragem dos pinos de entrada
    rb3_new = RB3;
    rb4_new = RB4;
    rb5_new = RB5;
    
    //analise
    
    if (rb3_new<rb3_old) f_int2();
    if (rb4_new<rb4_old) f_int1();
    if (rb5_new<rb5_old) f_int0();
    
    //quardo o valor atual
    
    rb3_old = rb3_new;
    rb4_old = rb4_new;
    rb5_old = rb5_new;
}*/

// aula do dia 15/09/2021
//pwm

#include "configbits.txt"

#include <xc.h>
#include <string.h>
#include "lcd.h"
#include "pwm.h"
#include <stdio.h>
#include <stdlib.h>

char linha1[17]="LCD linha1      ", linha2[17]="LCD linha2      ";

#define _XTAL_FREQ 4000000

void setup_hard(void);
void setup (void);
void loop (void);
void f_int0 (void);
void f_int1 (void);
void f_int2 (void);
void atraso (unsigned int tempo_ms);
void maqdisplay (void);
void f_timers(void);
void f_timer0 (void);
void f_timer1 (void);
void f_timer2 (void);
void f_timer3 (void);
void fint_ADC (void);
void escreve_DC (float valor_dc);

void mostraLCD (void);

void startADC (void);
int lerADC (void);


void mostra7seg (int valor);

unsigned char fLCDnew=0;

#define disp1 LATA2
#define disp2 LATA3
#define disp3 LATA4
#define disp4 LATA5

#define display LATD

unsigned char n1=1, n2=2, n3=3, n4=4;
unsigned int valorAD = 0;
unsigned int max_timer0, max_timer1, max_timer2, max_timer3;
unsigned int dado_teclado;
unsigned char conv_7seg[17] = {
    0b00111111,
    0b00000110,
    0b01011011,
    0b01001111,
    0b01100110,
    0b01101101,
    0b01111101,
    0b00000111,
    0b01111111,
    0b01101111,
    0b01110111,
    0b01111100,
    0b00111001,
    0b01011110,
    0b01111001,
    0b01110001,
    0b00000000
};


void __interrupt (high_priority) f_interrupt_high (void) {
    
    if (TMR0IE && TMR0IF) {
        TMR0L += 6;
        f_timers();
        TMR0IF=0;
    }
    
    if (INT0IE && INT0IF) {
        INT0IF = 0;
    }
    
    if (INT1IE && INT1IF) {
        INT1IF = 0;
    }
    
    if (INT2IE && INT2IF) {
        INT2IF = 0;
    }
    
    if (ADIE && ADIF) {
        ADIE = 0;
        fint_ADC();
        ADIF=0;     
    }
    
    
}

void __interrupt (low_priority) f_interrupt_low (void) {
    
    
    
}

void f_timers(void) {
    
    static unsigned int cont_timer0=0, cont_timer1=0, cont_timer2=0, 
                        cont_timer3=0;
    static char cont_disp=0;
    
    TRISB6 = 0;
    if (LATB6) LATB6=0; else LATB6=1;
    
    if (cont_disp < 4) {
        cont_disp++;
    }
    else {
        cont_disp=0;
        
        if (fLCDnew==0) {
        maqdisplay();
        }
    }
    
    
    if (cont_timer0 < max_timer0) {
        cont_timer0++;
    }
    else {
        cont_timer0=0;
        f_timer0();
    }
    
    if (cont_timer1 < max_timer1) {
        cont_timer1++;
    }
    else {
        cont_timer1=0;
        f_timer1();
    }
    
    if (cont_timer2 < max_timer2) {
        cont_timer2++;
    }
    else {
        cont_timer2=0;
        f_timer2();
    }
    
    if (cont_timer3 < max_timer3) {
        cont_timer3++;
    }
    else {
        cont_timer3=0;
        f_timer3();
    }
    
}



void setup_hard (void) {
    
    IPEN = 0;
    GIE = 0;
    PEIE = 0;
    
    // PORTAS ENTRADA E SAIDA
    
    ADCON1 = 0b00001111;
    
    TRISA = 0b11000011;
    TRISB = 0b11111111;
    TRISC = 0b11111011;
    TRISD = 0b00000000;
    TRISE = 0b11111001;
    
    
    // INTERRUPCAO
    
    INTCON  = 0b00100000;
    INTCON2 = 0b10000101;
    INTCON3 = 0b11000000;
    
    //timer0 fosc=4000000 tempo=1e-3
    T0CON = 0b11010001;
    TMR0L = 6;
    
    // AD  [TADmin = 0,7us  TACQmin = 2,4us  Fosc = 4MHz]
    
    ADCON0 = 0b00000001;
    ADCON1 = 0b00001110;
    ADCON2 = 0b10010100;
    
    /*// PWM [Tpwm = 2ms; Tdc = 0.5ms]
    
    T2CON = 0b00000110;
    PR2 = 124;
    
    CCP1CON = 0b00011100;
    CCPR1L = 0b00011111;*/
    PWM_Init();
    
    disp1 = 0; disp2 = 0; disp3 = 0; disp4 = 0;
    display = conv_7seg[16];
    
    lcd_init();
    
    setup(); // SETUP LOGICO
    
    mostraLCD();
    // HABILITANDO OU NAO INTERRUPCAO
    
    PEIE = 1;
    GIE = 1;
    
}



void main (void) {
    
    setup_hard();
    while(1) {
        
        if(fLCDnew) 
        {
            if(dado_teclado == 11)
            {
                strcpy(linha2, "");
            }
            
            else if(dado_teclado == 12)
            {
                strcpy(linha1, "");
                strcpy(linha2, "");
            }
            
            else
            {
                strcpy(linha1, "MARC_MATH_RODR");
            }
            mostraLCD();
            fLCDnew=0; 
        }
        
        loop();
    }
    
}

void atraso (unsigned int tempo_ms) {
    
    while(tempo_ms) {
        __delay_us(1000);
        tempo_ms--;
    }
    
}

void maqdisplay (void) {
    
    static unsigned char estado = 1;
    
//    TRISB7 = 0;
//    if (LATB7) LATB7=0; else LATB7=1;
    
    switch (estado) {
        
        case 1:
            estado = 2;
            disp4 = 0;
            display = conv_7seg[n1];
            disp1 = 1;
            break;
            
        case 2:
            estado = 3;
            disp1 = 0;
            display = conv_7seg[n2];
            disp2 = 1;
            break;
            
        case 3:
            estado = 4;
            disp2 = 0;
            display = conv_7seg[n3];
            disp3 = 1;
            break;
            
        case 4:
            estado = 1;
            disp3 = 0;
            display = conv_7seg[n4];
            disp4 = 1;
            break;
            
        default:
            estado = 1;
            disp1 = 0; disp2 = 0; disp3 = 0; disp4 = 0;
            display = conv_7seg[16];
            
    }
    
}


void startADC ( void) {
    
    ADIF = 0;
    ADIE = 1;
    GO = 1;
    
}



int lerADC (void) {
    
    int valor = 0;
    
    valor = ADRESH;
    valor = valor<<8;
    valor += ADRESL;
    
    return valor;
    
}

void mostra7seg (int valor) {
    
    n4 = valor % 10;
    valor = valor / 10;
    
    n3 = valor % 10;
    valor = valor / 10;
    
    n2 = valor % 10;
    valor = valor / 10;
    
    n1 = valor % 10;
     
}

void mostraLCD (void) {
    
    extern char linha1[17], linha2[17];
    
    disp1 = 0; disp2 = 0; disp3 = 0; disp4 = 0;
    
    lcd_cmd(L_CLR);
    
    lcd_cmd(L_L1);
    lcd_str(linha1);
    
    lcd_cmd(L_L2);
    lcd_str(linha2);
      
    display = conv_7seg[16];
    
}


//não usei
void escreve_DC (float dc) {
    
    unsigned int valor_dc;
    
    valor_dc = 4*(PR2+1)*dc;
    
    CCPR1L = (valor_dc & 0x03ff) >> 2;
    CCP1CON &= 0b11001111;
    CCP1CON |= (valor_dc & 0x0003) << 4;
      
}

// PARTE LOGICA


void entrada (void);
unsigned char ler_teclado(void);

void setup (void) {
    
    max_timer0 = 10 - 1;
    max_timer1 = 20 - 1;
    max_timer2 = 1000 - 1;
    max_timer3 = 1000 -1 ;
    
    n1 = 0; n2 = 0; n3 = 0; n4 = 0;
    
    strcpy(linha1, "MARC_MATH_RODR");
}


void loop (void) {   
    
    
}


void f_int0 (void) {     
     
    //escreve_DC(0);
    PWM1_Set_Duty(0);
}


void f_int1 (void) {
    
    //escreve_DC(valorAD);
    PWM1_Set_Duty(valorAD);    
}


void f_int2 (void) {
       
    //escreve_DC(dado_teclado);
    switch(dado_teclado)
    {
        case 1:
            PWM1_Set_Duty(0);
            break;
        
        case 2:
            PWM1_Set_Duty(113);
            break;
        
        case 3:
            PWM1_Set_Duty(226);
            break;
            
        case 4:
            PWM1_Set_Duty(339);
            break;
            
        case 5:
            PWM1_Set_Duty(452);
            break;
            
        case 6:
            PWM1_Set_Duty(565);
            break;
            
        case 7:
            PWM1_Set_Duty(678);
            break;
            
        case 8:
            PWM1_Set_Duty(791);
            break;
            
        case 9:
            PWM1_Set_Duty(904);
            break;
        
        case 0:
            PWM1_Set_Duty(1023);
            break;
    }
    
}


void f_timer0 (void) {
    static unsigned int dado_teclado_old = 0;
    
    startADC();
    
    entrada();
    
    dado_teclado=ler_teclado();
    if(dado_teclado != dado_teclado_old)
    {
        fLCDnew = 1;
        dado_teclado_old = dado_teclado;
        sprintf(linha2, "%2d", dado_teclado);
    }
    //mostra7seg(dado_teclado);
}


void f_timer1 (void) {
    
}


void f_timer2 (void) {
       
}


void f_timer3 (void) {
       
}


void fint_ADC (void) {
    
    valorAD = lerADC();
    mostra7seg(valorAD);
    
}


void entrada (void) {
    
    static unsigned char rb3_old=1, rb4_old=1, rb5_old=1;
    
    unsigned char rb3_new, rb4_new, rb5_new;
    
    // amostragem dos pinos de entrada
    rb3_new = RB3;
    rb4_new = RB4;
    rb5_new = RB5;
    
    //analise
    
    if (rb3_new<rb3_old) f_int2();
    if (rb4_new<rb4_old) f_int1();
    if (rb5_new<rb5_old) f_int0();
    
    //quardo o valor atual
    
    rb3_old = rb3_new;
    rb4_old = rb4_new;
    rb5_old = rb5_new;
    
}

unsigned char ler_teclado(void) {
    
    static unsigned char entrada, saida=255;
    
    TRISB = 0b11111000;
    TRISD = 0b00001111;
    
    LATB |= 0b00000111; 
    
    LATB ^= 0b00000001;
    for (int i=0; i<10; i++) Nop();
    entrada = PORTD;
    
    if ((entrada & 1) == 0) saida=11;
    if ((entrada & 2) == 0) saida=7;
    if ((entrada & 4) == 0) saida=4;
    if ((entrada & 8) == 0) saida=1;
    
    LATB ^= 0b00000011;
    for (int i=0; i<10; i++) Nop();
    entrada = PORTD;
    
    if ((entrada & 1) == 0) saida=0;
    if ((entrada & 2) == 0) saida=8;
    if ((entrada & 4) == 0) saida=5;
    if ((entrada & 8) == 0) saida=2;
    
    LATB ^= 0b00000110;
    for (int i=0; i<10; i++) Nop();
    entrada = PORTD;
    
    if ((entrada & 1) == 0) saida=12;
    if ((entrada & 2) == 0) saida=9;
    if ((entrada & 4) == 0) saida=6;
    if ((entrada & 8) == 0) saida=3;
    
    TRISB = 0b11111111;
    TRISD = 0b00000000;
       
    return saida;
}







