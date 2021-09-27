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
void password();
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
unsigned int dado_teclado = 0;
unsigned int star_lcd = 0;
unsigned int dado = 0;
char flag = 1;
static int dado_teclado_old = 0;
static unsigned int count = 0;
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
                dado = dado / 10;
                sprintf(linha2, "%d", dado); 
                if(count == 0)  count = 3;
                else            count--;
            }
            
            else if(dado_teclado == 12)
            {
                strcpy(linha1, "0");
                strcpy(linha2, "0");
                dado = 0;
                count = 0;
            }
            else if(dado > 1023)
            {
                strcpy(linha2, "Valor incorreto!");
                dado = 0;
            }
            else
            {
                sprintf(linha1, "%.2f%%", (float)dado*100.0/1023); 
                sprintf(linha2, "%d", dado); 
            }
            mostraLCD();
            fLCDnew=0; 
        }
        atraso(10);
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
    strcpy(linha2, "PWM");
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
       


    PWM1_Set_Duty(dado);

}


void f_timer0 (void) {    
    startADC();
    entrada();
    
    dado_teclado=ler_teclado();
    if(flag && (dado_teclado!=dado_teclado_old))
    {
        dado_teclado_old = dado_teclado;
        if(dado_teclado != 11 || dado_teclado != 12)
        {
            password();
            fLCDnew = 1;
        }

    }
    
    /*else if(PORTD && !flag)
    {
        flag = 1;
        //dado_teclado_old = 0;
    }*/
    
    if(star_lcd && !flag)
    {

        star_lcd = 0;
        flag = 1;
        dado_teclado_old = -2;
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
    
    if ((entrada & 1) == 0)
    { 
        saida=11;
        star_lcd = 1; 
        flag = 0;
    }
    if ((entrada & 2) == 0)
    {
        saida=7;
        star_lcd = 1;
        flag = 0;
    }
    if ((entrada & 4) == 0)
    {
        saida=4;
        star_lcd = 1;
        flag = 0;
    }
    if ((entrada & 8) == 0)
    { 
        saida=1;
        star_lcd = 1;
        flag = 0;
    }
    
    LATB ^= 0b00000011;
    for (int i=0; i<10; i++) Nop();
    entrada = PORTD;
    
    if ((entrada & 1) == 0) 
    {
        saida=0;
        star_lcd = 1;
        flag = 0;
    }
    if ((entrada & 2) == 0) 
    {
        saida=8;
        star_lcd = 1;
        flag = 0;
    }
    if ((entrada & 4) == 0)
    {
        saida=5;
        star_lcd = 1;
        flag = 0;
    }
    if ((entrada & 8) == 0)
    { 
        saida=2;
        star_lcd = 1;
        flag = 0;
    }
    LATB ^= 0b00000110;
    for (int i=0; i<10; i++) Nop();
    entrada = PORTD;
    
    if ((entrada & 1) == 0) 
    {
        saida=12;
        star_lcd = 1;
        flag = 0;
    }
    if ((entrada & 2) == 0) 
    {
        saida=9;
        star_lcd = 1;
        flag = 0;
    }
    if ((entrada & 4) == 0) 
    {
        saida=6;
        star_lcd = 1;
        flag = 0;
    }
    if ((entrada & 8) == 0) 
    {
        saida=3;
        star_lcd = 1;
        flag = 0;
    }
    TRISB = 0b11111111;
    TRISD = 0b00000000;
       
    return saida;
}

void password()
{
    if(dado_teclado!=11 && dado_teclado!=12){
        switch (count)
        {
            case 0:
                count = 1;
                dado = dado_teclado;
                break;

            case 1:
                count = 2;
                dado = dado*10 + dado_teclado;      //dado passado * 10 + dado atual 
                break;

            case 2:
                count = 3;
                dado = dado*10 + dado_teclado;
                break;
               
            case 3:
                count = 0;
                dado = dado*10 + dado_teclado;
                break;
        }         
    }
    
}