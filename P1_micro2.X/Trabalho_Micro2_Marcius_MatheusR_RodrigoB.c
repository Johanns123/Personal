// 31/08

#include "configbits.txt"

#include <xc.h>
#include <string.h>
#include "lcd.h"

char linha1[17]="LCD linha1      ", linha2[17]="LCD linha2      ";

#define _XTAL_FREQ 4000000
int var;
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
void fint_ADC (void);

void mostraLCD (void);

void startADC (void);
int lerADC (void);

void seq_0 (void);
void seq_1 (void);



void amostragem (int valor);

unsigned char fLCDnew=0;
unsigned char estado = 0;
unsigned char seq;
unsigned char contador, pausa, reverso1, reverso2;

#define disp1 LATA2
#define disp2 LATA3
#define disp3 LATA4
#define disp4 LATA5

#define display LATD

unsigned char n1=1, n2=2, n3, n4;

unsigned int max_timer0, max_timer1, max_timer2;

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
        f_int0();
        INT0IF = 0;
    }
    
    if (INT1IE && INT1IF) {
        f_int1();
        INT1IF = 0;
    }
    
    if (INT2IE && INT2IF) {
        f_int2();
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
    
    static unsigned int cont_timer0=0, cont_timer1=0, cont_timer2=0;
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
    
    if (cont_timer1 < max_timer1/2 ) {
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
    
    
}



void setup_hard (void) {
    
    IPEN = 0;
    GIE = 0;
    PEIE = 0;
    
    // PORTAS ENTRADA E SAIDA
    
    ADCON1 = 0b00001111;
    
    TRISA = 0b11000011;
    TRISB = 0b11111111;
    TRISD = 0b00000000;
    TRISE = 0b11111001;
    
    
    // INTERRUPCAO
    
    INTCON  = 0b00110000;
    INTCON2 = 0b10000101;
    INTCON3 = 0b11011000;
    
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

void seq_0 (void) {
//S1: 0 25 65 56 43 69 68 41 74 35 100 57 50 0
    static unsigned char estado = 0;
    switch (estado) {
        
        case 0:
            //LATD = conv_7seg[estado];
            n1 = 0;
            n2 = 0;
            if (reverso1 == 0)
                estado = 25;
            else
                estado = 50;
            break;
            
        case 25:
            //LATD = conv_7seg[estado];
            n1 = 2;
            n2 = 5;
            if (reverso1 == 0)
                estado = 65;
            else
                estado = 0;
            break;
            
        case 65:
            //LATD = conv_7seg[estado];
            n1 = 6;
            n2 = 5;
            if (reverso1 == 0)
                estado = 56;
            else
                estado = 25;
            break;
            
        case 56:
            //LATD = conv_7seg[estado];
            n1 = 6;
            n2 = 5;
            if (reverso1 == 0)
                estado = 43;
            else
                estado = 65;
            break;
        
        case 43:
            //LATD = conv_7seg[estado];
            n1 = 4;
            n2 = 3;
            if (reverso1 == 0)
                estado = 69;
            else
                estado = 56;
            break;
            
        case 69:
            //LATD = conv_7seg[estado];
            n1 = 6;
            n2 = 9;
            if (reverso1 == 0)
                estado = 68;
            else
                estado = 43;
            break;
            
        case 68:
            //LATD = conv_7seg[estado];
            n1 = 6;
            n2 = 8;
            if (reverso1 == 0)
                estado = 41;
            else
                estado = 69;
            break;
            
        case 41:
            //LATD = conv_7seg[estado];
            n1 = 4;
            n2 = 1;
            if (reverso1 == 0)
                estado = 74;
            else
                estado = 68;
            break;
            
        case 74:
            //LATD = conv_7seg[estado];
            n1 = 7;
            n2 = 4;
            if (reverso1 == 0)
                estado = 35;
            else
                estado = 41;
            break;  
            
        case 35:
            //LATD = conv_7seg[estado];
            n1 = 3;
            n2 = 5;
            if (reverso1 == 0)
                estado = 100;
            else
                estado = 74;
            break;
            
        case 100:
            //LATD = conv_7seg[estado];
            n1 = 0;
            n2 = 0;
            if (reverso1 == 0)
                estado = 57;
            else
                estado = 35;
            break;
        
        case 57:
            //LATD = conv_7seg[estado];
            n1 = 5;
            n2 = 7;
            if (reverso1 == 0)
                estado = 50;
            else
                estado = 100;
            break;
            
        case 50:
           //LATD = conv_7seg[estado];
            n1 = 5;
            n2 = 0;
            if (reverso1 == 0)
                estado = 0;
            else
                estado = 57;
            break;
            
            
        default:
            //LATD = conv_7seg[estado];
            estado = 0;
            n3 = 0;
            n4 = 0;
            
            break;
        
         
    }
}

void seq_1 (void) {
//S2: 0 67 28 71 65 4 50 97 56 55 76 90 16 0    
    static unsigned char estado = 0;
    switch (estado) {
        
        case 0:
            //LATD = conv_7seg[estado];
            n3 = 0;
            n4 = 0;
            if (reverso2 == 0)
                estado = 67;
            else
                estado = 16;
            break;
        
        case 67:
            //LATD = conv_7seg[estado];
            n3 = 6;
            n4 = 7;
            if (reverso2 == 0)
                estado = 28;
            else
                estado = 0;
            break;
        
        case 28:
            //LATD = conv_7seg[estado];
            n3 = 2;
            n4 = 8;
            if (reverso2 == 0)
                estado = 71;
            else
                estado = 67;
            break;
        
        case 71:
            //LATD = conv_7seg[estado];
            n3 = 7;
            n4 = 1;
            if (reverso2 == 0)
                estado = 65;
            else
                estado = 28;
            break;   
        
        case 65:
            //LATD = conv_7seg[estado];
            n3 = 6;
            n4 = 5;
            if (reverso2 == 0)
                estado = 4;
            else
                estado = 71;
            break;
            
        case 4:
            //LATD = conv_7seg[estado];
            n3 = 0;
            n4 = 4;
            if (reverso2 == 0)
                estado = 50;
            else
                estado = 65;
            break;
            
        case 50:
            //LATD = conv_7seg[estado];
            n3 = 5;
            n4 = 0;
            if (reverso2 == 0)
                estado = 97;
            else
                estado = 4;
            break;
            
        case 97:
            //LATD = conv_7seg[estado];
            n3 = 9;
            n4 = 7;
            if (reverso2 == 0)
                estado = 56;
            else
                estado = 50;
            break;    
             
        case 56:
            //LATD = conv_7seg[estado];
            n3 = 5;
            n4 = 6;
            if (reverso2 == 0)
                estado = 55;
            else
                estado = 97;
            break;
            
        case 55:
            //LATD = conv_7seg[estado];
            n3 = 5;
            n4 = 5;
            if (reverso2 == 0)
                estado = 76;
            else
                estado = 56;
            break;
            
        case 76:
            //LATD = conv_7seg[estado];
            n3 = 7;
            n4 = 6;
            if (reverso2 == 0)
                estado = 90;
            else
                estado = 55;
            break;    
        case 90:
            //LATD = conv_7seg[estado];
            n3 = 9;
            n4 = 0;
            if (reverso2 == 0)
                estado = 16;
            else
                estado = 76;
            break;
            
        case 16:
            //LATD = conv_7seg[estado];
            n3 = 1;
            n4 = 6;
            if (reverso2 == 0)
                estado = 0;
            else
                estado = 90;
            break;     
            
        default:
            //LATD = conv_7seg[estado];
            estado = 0;
            n3 = 0;
            n4 = 0;
            
            break;
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
    if (valor < 255){
        var = 1;
    }
    else if ((valor > 255) && (valor < 511)){
        var = 2;
    }
    else if ((valor > 512) && (valor < 767)){
        var = 3;
    }
    else{
        var = 4;
    }
                                            
    //atraso(10);
    return valor;
    
}

/*void amostragem (int valor) {
    
    valor = valor / 10;
 
    valor = valor / 10;
    
    n2 = valor % 10;
 
    n1 = valor/10;
    
     
}*/

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


// PARTE LOGICA


int valorADC;
unsigned char fADCnew=0;
unsigned char contador, pausa, reverso;


void setup (void) {
    
    max_timer0 = 50 - 1;
    max_timer1 = 200 - 1; // TEMPO DE AMOSTRAGEM DO ESTADO
    max_timer2 = 200 - 1;
    
    reverso1 = 0;
    reverso2 = 0;
    pausa = 0;
    seq_0(); // seq 1, 2, 3 ou 4
    seq_1();
    //n1 = estado; 
    
    
    strcpy(linha1, "MARC_MATH_RODR");
//    strcpy(linha2, "terca e quartas");
//    
//    fLCDnew=1;
    
}


void loop (void) {     
    valorADC = lerADC();
    if (fADCnew) {
    
        fADCnew=0;
   
    }
    
    if (pausa == 0) {
        //amostragem(max_timer1 + 1);
        if ((reverso1==0) && (reverso2==0)) // 0 crescente, 1 decrescente
        {
            strcpy(linha2, "S1-C S2-C");
            
        }
        else if ((reverso1==1) && (reverso2==0))
        {
            strcpy(linha2, "S1-D S2-C");
        }
            
        else if ((reverso1==0) && (reverso2==1))
        {
            strcpy(linha2, "S1-C S2-D");
        } 
        else if ((reverso1==1) && (reverso2==1))
        {   
            strcpy(linha2, "S1-D S2-D");
        }
    }
          
        //LATD = contador;
        atraso(10);
}


void f_int0 (void) {     
    
    if (pausa) pausa=0; else pausa=1;
    
}


void f_int1 (void) {
    
    if (max_timer1 == 199) max_timer1 = 799; else max_timer1 = 199;
        f_timers();
}


void f_int2 (void) {
      
    if (max_timer2 == 199) max_timer2 = 799; else max_timer2 = 199;
        f_timers();
}


void f_timer0 (void) {
    
    startADC();
     
}


void f_timer1 (void) {
    
    static unsigned char troca=0;
    
    if (troca) 
    {
        troca=0; 
        seq = 0;
        if(pausa == 0){
            if (seq == 0){
                if (var == 1){
                    reverso1 = 0;
                    seq_0();
                }
                if (var == 2){
                    reverso1 = 0;
                    seq_0();
                }
                if (var == 3){
                    reverso1 = 1;
                    seq_0();
                }
                if (var == 4){
                    reverso1 = 1;
                    seq_0();
                }
            }
        }
    }
                      
    else 
    {
        troca=1;
    }
    
    fLCDnew=1;
    
}


void f_timer2 (void) {
    
    static unsigned char troca=0;
    
    if (troca) 
    {
        troca=0; 
        seq = 0;
        if(pausa == 0){
            if (seq == 0){
                if (var == 1){
                    reverso2 = 0;
                    seq_1();
                }
                if (var == 2){
                    reverso2 = 1;
                    seq_1();
                }
                if (var == 3){
                    reverso2 = 0;
                    seq_1();
                }
                if (var == 4){
                    reverso2 = 1;
                    seq_1();
                }
            }
        }
    }    
    else 
    {
        troca=1;
    }
    
    fLCDnew=1;
    
}
void fint_ADC (void) {
    
    valorADC = lerADC();
    fADCnew=1;
    
}







