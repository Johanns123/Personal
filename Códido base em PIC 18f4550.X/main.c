/*Pinos de conex�o pickit3
MCLR
VDD
VSS
PGD - RB7
PGC - RB6*/



//-------Conex�o PWM-----//

/*CCP1 - RC2
CCP2 - RC1*/


//-----Pinos de leitura anal�gica--//

//RA0-RA3, RA5, RE0-RE1; -->Sensores frontais


//---Pinos digitais de entrada de dados----//

//sensores laterias

//RB3 e RB4

//---Pinos de leitura UART/Serial---//

//RC6/TX e RC7/RX

//--Pinos digitais de sa�da de dados---//

//RD4 - RD7


//Observa��es:
/*RB0 - RB2 s�o pinos que possuem interrup��o externa
RB7 e RB6 s�o pinos de compila��o, logo se for utilizar, rob� deve estar desligado
Verificar a necessidade de utilizar as interrup��es externas*/





//------------------------------------------------------------------------------------
//C�digo de base para qualquer outro rob�, aqui est�o todas as fun��es e ideias usadas
//at� o momento, sendo estas comentadas para utiliza��o futura.
//------------------------------------------------------------------------------------

// PWM       
// Kp:     
// Ki:       
// Kd:   
// Tens�o: 

// PWM           
// Kp:          
// Kd:         
// Ki:          
// Tens�o:      

#include "configbit.txt" //configura��es dos bits
#include "PWM.h"         //biblioteca que gerencia o PWM
#include <xc.h>
#include <stdio.h>
#include "serial.h"
#include <stdlib.h>
#include <string.h>
#define _XTAL_FREQ 20000000 //Frequencia em 20MHz


//o PWM DESSE C�DIGO VAI DE 0 A 962


//#define SaidaA RC1          //pino PWM do motorA  --> n�o precisa ser decretado
//#define SaidaB RC2          //pino PWM do motorB

//-motores-//
//Lado direito
#define AIN1 RD4            //sentido hor�rio
#define AIN2 RD5            //sentido anti-hor�rio

//Lado esquerdo
#define BIN1 RD6            //sentido hor�rio
#define BIN2 RD7            //sentido anti-hor�rio

//sensores laterais
#define sensor_de_parada RB3
#define sensor_de_curva  RB4

#define led RD0         //led indicador de calibra��o

int LeituraBranco = 0;         // Numero de vezes que o sensor de borda lera branco
double Kp = 0, Kd = 0, Ki = 0; // Vari�veis que s�o modificadas no PID
int PWM = 0, PWMR = 0;         // valor da for�a do motor em linha reta
double erro, p, d, erroAnterior = 0, i, integral = 0, Turn = 0; //�rea PID
double MotorA, MotorB; // PWM que ser� alterado pelo PID


int position_line = 0; // M�todo com o Read Line;

int ExcessoA, ExcessoB;

int contador = 0, acionador = 0; // Borda

//�rea de defini��o das portas anal�gicas

int conversao_AD1(void){
    int conversao;
    ADCON0 = 0b00000001;// Seleciona canal AN0
    ADCON0bits.GO=1; // inicia a convers�o
    while (ADCON0bits.GO); // Aguarda a o fim da convers�o
    conversao = (ADRESL + (ADRESH << 8));
    return conversao;   //retorna valor de leitura em 10 bits
    
}


int conversao_AD2(void){
    int conversao2;
    ADCON0 = 0b00000101;// Seleciona canal AN1
    ADCON0bits.GO=1; // inicia a convers�o
    while (ADCON0bits.GO); // Aguarda a o fim da convers�o
    conversao2 = ADRESL + (ADRESH << 8);
    return conversao2;   //retorna valor de leitura em 10 bits
}

int conversao_AD3(void){
    int conversao3;
    ADCON0 = 0b00001001;// Seleciona canal AN2
    ADCON0bits.GO=1; // inicia a convers�o
    while (ADCON0bits.GO); // Aguarda a o fim da convers�o
    conversao3 = ADRESL + (ADRESH << 8);
    return conversao3;      //retorna valor de leitura em 10 bits
    
}

int conversao_AD4(void){
    int conversao4;
    ADCON0 = 0b00001101;// Seleciona canal AN3
    ADCON0bits.GO=1; // inicia a convers�o
    while (ADCON0bits.GO); // Aguarda a o fim da convers�o
    conversao4 = ADRESL + (ADRESH << 8);
    return conversao4;      //retorna valor de leitura em 10 bits

}

int conversao_AD5(){
    int conversao5;
    ADCON0 = 0b00010001;// Seleciona canal AN4
    ADCON0bits.GO=1; // inicia a convers�o
    while (ADCON0bits.GO); // Aguarda a o fim da convers�o
    conversao5 = ADRESL + (ADRESH << 8);
    return conversao5;      //retorna valor de leitura em 10 bits

}

int conversao_AD6(void){
    int conversao6;
    ADCON0 = 0b00010101;// Seleciona canal AN5
    ADCON0bits.GO=1; // inicia a convers�o
    while (ADCON0bits.GO); // Aguarda a o fim da convers�o
    conversao6 = ADRESL + (ADRESH << 8);
    return conversao6;      //retorna valor de leitura em 10 bits

}

int conversao_AD7(void){
    int conversao6;
    ADCON0 = 0b00011001;// Seleciona canal AN6
    ADCON0bits.GO=1; // inicia a convers�o
    while (ADCON0bits.GO); // Aguarda a o fim da convers�o
    conversao6 = ADRESL + (ADRESH << 8);
    return conversao6;      //retorna valor de leitura em 10 bits

}

int conversao_AD8(void){
    int conversao6;
    ADCON0 = 0b00011101;// Seleciona canal AN7
    ADCON0bits.GO=1; // inicia a convers�o
    while (ADCON0bits.GO); // Aguarda a o fim da convers�o
    conversao6 = ADRESL + (ADRESH << 8);
    return conversao6;      //retorna valor de leitura em 10 bits

}

char uart_rd;           //Vari�vel de leitura serial
int leitura1 = 0;       //leitura anal�gica
int leitura2 = 0;
int leitura3 = 0;
int leitura4 = 0;
int leitura5 = 0;
int leitura6 = 0;
int leitura7 = 0;
int leitura8 = 0;
char txt[7];
char txt2[7]; //vari�veis que enviam valores de leitura
char txt3[7];

void Frente();
void Tras();
void Girohorario();
void GiroAntihorario();
void Freio();


void main(){
    //Sensores anal�gicos
    TRISAbits.RA0 = 0x01;       //Determina sensor frontal como entrada
    TRISAbits.RA1 = 0x01;       //Determina como entrada
    TRISAbits.RA2 = 0x01;       //Determina como entrada
    TRISAbits.RA3 = 0x01;       //Determina como entrada
    TRISAbits.RA5 = 0x01;       //Determina como entrada
    TRISEbits.RE0 = 0x01;       //Determina como entrada
    TRISEbits.RE1 = 0x01;       //Determina sensor traseiro como entrada
    TRISEbits.RE2 = 0x01;       //Determina como entrada
    TRISBbits.RB3 = 0x01;       //Determina como entrada
    TRISBbits.RB4 = 0x01;       //Determina como entrada
    
    //sensores digitais
    TRISBbits.RB3 = 0x01;       //determina sensor de parada como entrada
    TRISBbits.RB4 = 0x01;       //determina sensor de curva como entrada
    
    
    //Sa�das PWM
    TRISCbits.RC2 = 0x00;       //PWMA como sa�da
    TRISCbits.RC1 = 0x00;
    
    //Sa�das para os motores/ponte-h
    TRISDbits.RD4 = 0x00;       //AIN1
    TRISDbits.RD5 = 0x00;       //AIN2
    TRISDbits.RD6 = 0x00;       //BIN1
    TRISDbits.RD7 = 0x00;       //BIN2
    
    
    PORTA = 0x00;               //Inicializa todo o PORTA em 0
    PORTE = 0x00;               //Inicializa todo o PORTE em 0
    
    //--Regi�o de configura��o dos registradores do ADC--//
    ADCON1 = 0b00000111;        //habilita as portas AN0 at� AN7 como portas anal�gicas
    ADCON2 = 0b10010101;        //Justificado � direita
                                //TAD de 4
                                //Conversores AD select bits de Fosc/16
    CMCON = 0x07;               //desabilita comparadores
    
    
    //-----leitura dos sensores frontais-------//
    leitura1 = conversao_AD1();
    leitura2 = conversao_AD2();
    leitura3 = conversao_AD3();
    leitura4 = conversao_AD4();
    leitura5 = conversao_AD5();
    leitura6 = conversao_AD6();
        
    int sensores_frontais [] = {leitura1, leitura2, leitura3, leitura4, leitura5, leitura6};  //sensores frontais declarados como vetores
        
    //------leitura dos sensores traseiros---//
        
    leitura7 = conversao_AD7();
    leitura8 = conversao_AD8();
        
    int sensores_traseiros [] = {leitura7, leitura8};       //sensores traseiros

    serial_init(); //inicializa o serial com 9600 de baud rate
    PWM_Init();        //configura pwm em 1,388KHz
    __delay_ms(100);
    
    serial_tx_str("Robo ligado");
    serial_tx(10);
    serial_tx(13);      //quebra de linha
    
    //---->Calibra��o dos sensores<---// Ainda a ser implementada
    for(int i = 0; i <120; i++){
        //calibra��o
        __delay_ms(5);
    }
    
    led = 0x01;
    __delay_ms(1000);
    led = 0x00;
    __delay_ms(500);
    led = 0x01;
    __delay_ms(500);
    led= 0x00;
    
    while(1){
    
        //fun��o de calibra��o atribuindo a sensor de curva
        led = 0x01;
        
        //--C�culo do erro dos sensores frontais atrav�s de m�dia ponderada--//
        int peso [] = {-3, -2, -1, 1, 2 , 3};
        float soma_direito, soma_esquerdo, denominador_direito, denominador_esquerdo;
        float Erro_direito, Erro_esquerdo;
        
        for(int j = 0; j < 3; j++){
            denominador_direito += sensores_frontais[j];
            soma_direito += (sensores_frontais[j] * peso[j]);
        
            denominador_esquerdo += sensores_frontais[5-j];
            soma_esquerdo += (sensores_frontais[5-j] * peso[5-j]);
        }

        Erro_direito = soma_direito / denominador_direito;
        Erro_esquerdo = soma_esquerdo / denominador_esquerdo;
        erro = Erro_direito - Erro_esquerdo;
    
        //----�rea que imprime no serial o valor do erro---//
        serial_tx_str("Erro:");
        sprintf(txt3, "%i", erro);
        serial_tx(32);       //gera espa�o
        serial_tx_str(txt3);
        serial_tx(13); //quebra de linha

    
        
        //------------AREA DO SENSOR DE PARADA A SER IMPLEMENTADO----------//
        
        
        
        
        
        
        
        
        
        //--------AREA DO PID------//
        p = erro * Kp; // Proporcao
  
        integral += erro; // Integral
        i = Ki * integral;
 
        d = Kd * (erro - erroAnterior); // Derivada
        erroAnterior = erro;
  
        Turn = p + i + d;
  
        MotorA = PWM - Turn;   //lado direito
        MotorB = PWM + Turn;   //lado esquerdo
        
        
        //------->AREA DO SENTIDO DAS RODAS<-----//
        
        Frente();
        
        if (MotorA < 0) // Giro para a direita
        {
            AIN1 = 0x00;  //Sentido hor�rio estado baixo
            AIN2 = 0x01;  //Sentido anti-hor�rio estado alto
            
            Girohorario();
        }
        if (MotorB < 0) // Giro para a esquerda
        {
            BIN1 = 0x00;  //Sentido hor�rio estado baixo
            BIN2 = 0x01;  //Sentido anti-hor�rio estado alto
            
            GiroAntihorario();
            
        }

        PWM1_Set_Duty(MotorA);
        PWM2_Set_Duty(MotorB); 

        
        
    }

}




void Frente(){

    AIN1 = 0x01;
    AIN2 = 0x00;
    BIN1 = 0x01;
    BIN2 = 0x00;

}

void Tras(){

    AIN1 = 0x00;
    AIN2 = 0x01;
    BIN1 = 0x00;
    BIN2 = 0x01;


}


void Girohorario(){

    AIN1 = 0x00;
    AIN2 = 0x01;
    BIN1 = 0x01;
    BIN2 = 0x00;


}

void GiroAntihorario(){

    AIN1 = 0x01;
    AIN2 = 0x00;
    BIN1 = 0x00;
    BIN2 = 0x01;


}


void Freio(){       //Essa fun��o deve ser reformulada

    Frente();
    
    PWM1_Set_Duty(187);     //20% de duty cycle
    PWM2_Set_Duty(187); 
    
    
    __delay_ms(500);
    
    AIN1 = 0x01;
    AIN2 = 0x01;
    BIN1 = 0x01;
    BIN2 = 0x01;
    
    PWM1_Set_Duty(0);     //0% de duty cycle
    PWM2_Set_Duty(0);
    
    __delay_ms(2000);
    
    AIN1 = 0x00;
    AIN2 = 0x00;
    BIN1 = 0x00;
    BIN2 = 0x00;
    
    __delay_ms(10000);

}