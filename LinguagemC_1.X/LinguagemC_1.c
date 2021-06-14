/*
 * File:   LinguagemC_1.c
 * Author: Johann
 *
 * Created on 24 de Maio de 2021, 18:00
 */
#define F_CPU 16000000  //define a frequencia do uC para 16MHz
#include <avr/io.h>     //biblioteca de mapeamento dos registradores
#include <util/delay.h> //biblioteca que gera atraso
#include <avr/interrupt.h>

int aula1(){
    DDRD = 0b11111111;      //habilita todos os pinos do PORTD como sa�da
    PORTD = 0b00000000;     //declara todo o PORTD de como LOW
    
    while(1)
    {
        PORTD = 0xAA;       //0b 1010 1010
        _delay_ms(500);
        PORTD = 0x55;       //0b 0101 0101
        _delay_ms(500);
    }
}

int aula02(){
    /*Nesta aula e na pr�xima s�o focados os operadores l�gicos
     |  ou l�gico - aciona bits
     &  e l�gico - apaga bits
     ^  OU Exclusivo - inverte o estado de um bit(XOR)
     ~  inversor - inverte uma l�gica
     
     Acionamento de um bit:
     PORTD |= (1<<5)        Aciona o bit 5 do PORTD
     
     Para desligar um bit
     PORTD &= ~(1<<5)   utiliza-se um inversor para n�o desligar todos os bits mais somente o bit 5
     
     Para inverter um bit:
     PORTD ^= (1<<5)    inverte o valor do bit 5, podendo ser 0 ou 1
                        dependendendo do valor inicial
     
    Testar se um bit � diferente de 0:
    if(PIND &(1<<5))       se for 1 � verdadeiro
     
    Testar se um bit � zero (Devido aos pull-ups internos os bot�es tem
                                        n�vel zero quando pressionados:
    if(!(PIND &(1<<5)))*/
    
    
    DDRD = 0b11111111;      //habilita todos os pinos do PORTD como sa�da
    PORTD = 0b00000000;     //declara todo o PORTD de como LOW
    
    while(1)
    {
        PORTD ^= 0b00001000;    //l�gica XOR - s�ida 0 com entradas de mesmo n�vel l�gico
                                //este c�digo vai fazer somente o led 3 piscar
        _delay_ms(500);
        
    } 
}

int aula03(){
    
    DDRC = 0b00000000;
    PORTC = 000001100;
    
    DDRD = 0xFF;
    
    while(1){
        if(!(PINC & (1<<2))){       //bot�o pressionado?
            PORTD |= (1<<7);        //sim? acende led 7. Este comando ativa o bit 7
        }
        
        else if(!(PINC & (1<<3))){
            PORTD &= ~(1<<7);       //apaga led 7
                                    //este comando apaga o bit 7, por�m o
                                    //operador l�gico ~ � usado 
                                    //para n�o ser executado em outros bits
                                    //Na d�vida utilize os endere�os em bin�rio
                                    //e fa�a a tabela verdade e compare
                                    //os valores de origem e destino por l�gica digital 
        }
    }
}

int aula04(){
    char flag = 0;
    
    DDRC = 0b00000000;
    PORTC = 000001100;
    
    DDRD = 0xFF;
    
    while(1){
        if(!(PINC & (1<<2))){       //bot�o pressionado?
            _delay_ms(10);          //tempo de debouncing
            flag = 1;
        }
        if((PINC &(1<<2)) && flag == 1){    //bot�o solto e flag = 1
            _delay_ms(10);
            flag = 0;
            PORTD ^= (1<<4);        //ao soltar o bot�o o led 4 troca de estado
        }
    }
}

int aula05(){
    /* Ordem dos segmentos dos displays:
    PD0- 'e' PD1- 'd' PD2- 'c' PD3- 'dp' PD4- 'g' PD5- 'f' PD6- 'a' PD7- 'b'*/
    char flag = 0;
    int contador = 0;
    
    DDRC = 0b00000000;
    PORTC = 000001100;
    
    DDRD = 0xFF;
    
    DDRB |= 0b00000001;
    PORTB |= 0b00000001;    //ativa somente...
    
    while(1){
        if(!(PINC & (1<<2))){       //bot�o pressionado?
            _delay_ms(10);          //tempo de debouncing
            flag = 1;
        }
        if((PINC &(1<<2)) && flag == 1){    //bot�o solto e flag = 1
            _delay_ms(10);
            contador++;
            if(contador > 4){
                contador = 0;
            }
            flag = 0;
        }
        
        switch(contador){
            case 0: PORTD = 0b11100111;
                break;
            case 1: PORTD = 0b10000100;
                break;
            case 2: PORTD = 0b11010011;
                break;
            case 3: PORTD = 0b11010110;
                break;
            default: PORTD = 0xff;
                break;
        }
        
    }
}

int aula06(){
    /* Ordem dos segmentos dos displays:
    PD0- 'e' PD1- 'd' PD2- 'c' PD3- 'dp' PD4- 'g' PD5- 'f' PD6- 'a' PD7- 'b'*/
    char flag = 0;
    int contador = 0;
    char DISPLAY [4] = {0b11100111,0b10000100,0b11010011,0b11010110,0x00};
    
    DDRC = 0b00000000;
    PORTC = 000001100;
    
    DDRD = 0xFF;
    
    DDRB |= 0b00000001;
    PORTB |= 0b00000001;    //ativa somente...
    
    PORTD = DISPLAY[0];     //inicializa em zero
    
    while(1){
        if(!(PINC & (1<<2))){       //bot�o pressionado?
            _delay_ms(10);          //tempo de debouncing
            flag = 1;
        }
        if((PINC &(1<<2)) && flag == 1){    //bot�o solto e flag = 1
            _delay_ms(10);
            contador++;
            if(contador > 4){
                contador = 0;
            }
            PORTD = DISPLAY[contador];
            flag = 0;
        }
        

    }
}

/*ISR(PCINT1_vect);

int aula07(){
    //Interrup��es externas
    
    DDRD =  0xff;    //Defino o PORTD como sa�da
    PORTD = 0x00;   //Inicializo em 0
    
    DDRC =  0b00000000; //defino o PORTC como entrada 
    PORTC = 0b00001100; //ativo os pull-ups do PC2 e PC3
    
    //configura��o da interrup��o externa
    PCICR = 0b00000010;  //Ativa o PCINT1 Geral - interrup��o externa
    PCMSK1 = 0b00001100; //Habilita o PC2 e PC3 como PCINT (PCINT espec�fico)
    
    sei();               //Faz a habilita��o geral das interrup��es - SREG
    //--------------------------------//
    
    
    while(1){
        PORTD ^= (1<<3);    //alterna o estado do led PD3
        _delay_ms(200);
        
    }
}

ISR(PCINT1_vect){   //fun��o de interrup��o da biblioteca interrup.h
    
    if(!(PINC &(1<<2))){ //bot�o PC2 pressionado?
        PORTD |= (1<<5);    //acende led PD5
    }
    
    else if (!(PINC &(1<<3))){  //botao PC3 pressionado?
        PORTD &= ~(1<<5);
    }
}*/

ISR(PCINT1_vect);
ISR(PCINT0_vect);

int aula07(){
    //Interrup��es externas
    
    DDRD =  0xff;    //Defino o PORTD como sa�da
    PORTD = 0x00;   //Inicializo em 0
    
    DDRC =  0b00000000; //defino o PORTC como entrada 
    PORTC = 0b00001100; //ativo os pull-ups do PC2 e PC3
    
    DDRB = 0x00;
    //configura��o da interrup��o externa
    PCICR = 0b00000011;  //Ativa o PCINT1 Geral + PCINT0 Geral - interrup��o externa
    PCMSK0 = 0b00010100; //Habilita o PB2 e PB4 como PCINT (PCINT espec�fico)
                         //n�o precisa habilitar os resistores de pull-up do DDRB
                         //pois est�o conectados a switchs de duas fases (VCC OR GND)
    PCMSK1 = 0b00001100; //Habilita o PC2 e PC3 como PCINT (PCINT espec�fico)
    
    
    
    
    sei();               //Faz a habilita��o geral das interrup��es - SREG
    //--------------------------------//
    
    
    while(1){
        PORTD ^= (1<<3);    //alterna o estado do led PD3
        _delay_ms(200);
        
    }
}

ISR(PCINT1_vect){   //fun��o de interrup��o da biblioteca interrup.h
    
    if(!(PINC &(1<<2))){ //bot�o PC2 pressionado?
        PORTD |= (1<<5);    //acende led PD5
    }
    
    else if (!(PINC &(1<<3))){  //botao PC3 pressionado?
        PORTD &= ~(1<<5);
    }
}

ISR(PCINT0_vect){   //fun��o de interrup��o da biblioteca interrup.h
    
    if(!(PINB &(1<<2))){ //bot�o PC2 pressionado?
        PORTD |= (1<<7);    //acende led PD5
    }
    
    else if (!(PINB &(1<<4))){  //botao PC3 pressionado?
        PORTD &= ~(1<<7);
    }
}


//Defini��es de macros para o trabalho com bits
#define set_bit(y,bit) (y|=(1<<bit)) //coloca em 1 o bit x da vari�vel Y
#define clr_bit(y,bit) (y&=~(1<<bit)) //coloca em 0 o bit x da vari�vel Y
#define cpl_bit(y,bit) (y^=(1<<bit)) //troca o estado l�gico do bit x da vari�vel Y
#define tst_bit(y,bit) (y&(1<<bit)) //retorna 0 ou 1 conforme leitura do bit

int aula08(){
    DDRC = 0b00000000; //PORTC como entrada
    PORTC = 0b00001100; //Ativa os pull-ups de PC2 e PC3
    DDRD = 0b11111111; //PORTD como sa�da
    PORTD = 0b00000000; //Apaga os leds do PORTD
    while(1){
    //Testa o bit PC2
        if(tst_bit(PINC,PINC2) == 0){
            set_bit(PORTD, PORTD0); //Liga PD0
        }
        else{
            clr_bit(PORTD, PORTD0); //Desliga PD0
        }
        //Testa o bit PC3
        if(tst_bit(PINC,PINC3) == 0){
            //Enquanto PC3 = 0, n�o faz nada
            while(tst_bit(PINC,PINC3) == 0);
            _delay_ms(10);
            cpl_bit(PORTD, PORTD2); //Inverte o pino PD2
        }
    }

}

void main()
{
    //aula01();
    //aula02();
    //aula03();
    //aula04();
    //aula05();
    //aula06();
    //aula07();
    aula08();
}
