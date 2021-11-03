/*
 * File:   main.c
 * Author: Johann
 *
 * Created on 30 de Maio de 2021, 16:52
 * Last update on July 7th of 2021 at 15:43
 */

/*Este c�digo � de um rob� seguidor de linha da equipe Wolfbotz.
 * Aqui n�s vemos o controle do rob� bem como as tomadas de decis�o de acordo com os padr�es da pista*/
#include "HAL_atmega328p.h"
#include <stdio.h>            //Bilioteca do C
#include "UART.h"             //Biblioteca da comunica��o UART
#include "ADC.h"              //Biblioteca do conversor AD
#include "PWM_10_bits.h"      //Biblioteca de PWM fast mode de 10 bits
#include "Driver_motor.h"     //Biblioteca das fun��es de controle dos motores  //usado para ponte H tb6612fng
#include "PID.h"              //Biblioteca do controle PID
/*============================================================*/

/*==============================================================*/

/*Vari�veis globais*/
int erro = 0;      //vari�vel para c�culo do erro da dire��o do rob� em cima da linha
unsigned int PWMA = 0, PWMB = 0; // Modula��o de largura de pulso enviada pelo PID
unsigned char sensores_frontais[5];
unsigned int PWMR = 100; // valor da for�a do motor em linha reta
unsigned int PWM_Curva = 80; //PWM ao entrar na curva
int u = 0; //valor de retorno do PID 
    
//Vari�veis globais da calibra��o de sensores
unsigned char valor_max[5] = {0, 0, 0, 0, 0};
unsigned char valor_min[5] = {255, 255, 255, 255, 255};
unsigned char valor_max_abs = 255;
unsigned char valor_min_abs = 0;

//vari�veis de controle
char f_parada= 0;   //vari�vel que comanda quando o rob� deve parar e n�o realizar mais sua rotina
char f_calibra = 0; //vari�vel que indica o fim da calibra��o dos sensores e inicio da estrat�gia
char flag = 0;      //vari�vel de controle para identificar o momento de parada
char f_motor = 0;   //vari�vel de controle da calibra��o autom�tica

volatile char ch; //armazena o caractere lido
char buffer[5]; //String que armazena valores de entrada para serem printadas
volatile char flag_com = 0; //flag que indica se houve recep��o de dado*/

/*tempo =65536 * Prescaler/Fosc = 65536 * 1024/16000000 = 4, 19s
 tempo = X_bit_timer * Prescaler/Fosc
 Valor inicial de contagem = 256 - tempo_desejado*Fosc/Prescaler = 256 - 0,001*16000000/1024 = 255
 Valor inicial de contagem = X_bit_timer - tempo_desejado*Fosc/Prescaler */
/*===========================================================================*/

/*Prot�tipo das fun��es*/
void ADC_maq();             //m�quina de estado do conversor AD
void parada();              //Leitura dos sensores laterais
void calibra_sensores();    //calibra sensores manualmente 
void seta_calibracao();     //estabelece o limiar dos valores m�ximos e m�nimos de leitura
void sensores();            //caso um sensor passe do valor, o mesmo � corrigido
void setup();
void setup_logica();
void loop();
void sentido_de_giro();     //l� os sensores frontais e determina o sentido de giro dos motores com o PID
void PWM_limit();           //limita o PWM em 1000 caso a vari�vel passe de 1023 
void estrategia();          //estrategia do rob�
void calibration();         //cont�m toda a rotina de calibra��o
void fim_de_pista();        //verifica se � o fim da psita
void f_timers (void);       //fun��o de temporiza��o das rotinas
void Auto_calibration(void);
void volta_pra_pista(void);
/*===========================================================================*/

/*Interrup��es*/
ISR(USART_RX_vect) {
    ch = UDR0; //Faz a leitura do buffer da serial

    UART_enviaCaractere(ch); //Envia o caractere lido para o computador
    flag_com = 1; //Aciona o flag de comunica��o
}

ISR(TIMER0_OVF_vect) 
{
    TCNT0 = 56; //Recarrega o Timer 0 para que a contagem seja 100us novamente
    
    f_timers(); //fun��o de temporiza��o das rotinas   
}//end TIMER_0

ISR(ADC_vect)
{
    ADC_maq();  //m�quina de estado do conversor AD
}//end ADC_int

/*Fun��o principal*/
int main(void) 
{
    setup();

    while (1) loop();
    return 0;
}//end main

//===Fun��es n�o vis�veis ao usu�rio======//
void setup() 
{

    setup_Hardware();   //setup das IO's e das interrup��es
    sei();              //Habilita as interrup��es
    calibration();      //rotina de calibra��o
    setup_logica();     //defini��o das vari�veis l�gicas(vazio por enquanto)

}//end setup


void calibration()
{
     //----> Calibra��o dos Sensores frontais <----//
    set_bit(PORTB, led);
    ADC_maq();  //inicializa a convers�o do AD
    calibra_sensores(); //calibra��o dos sensores //A calibra��o vai conseguir acompanhar o AD
                                                  //ou pode ser que o vetor n�o seja preenchido a tempo?
                                                  //� necess�rio colocar um contador
                                                  //para depois chamar a fun��o de calibra��o?
    
    seta_calibracao(); //estabelece o limiar dos sensores atrav�s dos valores da fun��o de cima
    
    clr_bit(PORTB, led);
    _delay_ms(250);
    set_bit(PORTB, led); //subrotina de acender e apagar o LED 13
    _delay_ms(250);
    clr_bit(PORTB, led);
    _delay_ms(250);
    set_bit(PORTB, led);
    _delay_ms(250);
    clr_bit(PORTB, led);
    _delay_ms(1000);
    
    /*if(f_motor) //para o rob� para iniciar a rotina
    {
        motor_off();
        setDuty_1(0);
        setDuty_2(0);
    }*/
    f_calibra = 1;  //flag para indicar fim da calibra��o
}

void setup_logica()
{
    
}


void loop()//loop vazio
{
    
}

void ADC_maq () 
{
    //inicializo no setup na fun��o calibration e em seguida toda
    //vez que ocorre uma conver��o a interrup��o do AD ocorre
    //e ent�o esta fun��o � chamada pelo vetor de interrup��o
    //do AD, obtendo os dados da convers�o em "paralelo" � rotina
    
    //Leio primeiro o default que seria o primeiro canal
    //e em seguida fa�o uma l�gica circular de leitura dos canais
    
    static unsigned char estado = 10;
    
    switch (estado) {
        
        case 0:
            estado = 1;
            sensores_frontais[0] = ADC_ler();
            ADC_conv_ch(1);
            break;
            
        case 1:
            estado = 2;
            sensores_frontais[1] = ADC_ler();
            ADC_conv_ch(0);
            break;
            
        case 2:
            estado = 3;
            sensores_frontais[2] = ADC_ler();
            ADC_conv_ch(7);
            break;
            
        case 3:
            estado = 4;
            sensores_frontais[3] = ADC_ler();
            ADC_conv_ch(6);
            break;
            
        case 4:
            estado = 0;
            sensores_frontais[4] = ADC_ler();
            ADC_conv_ch(2);
            break;
            
            
        default:
            estado = 0;
            ADC_conv_ch(2);
            sensores_frontais[0] = ADC_ler();
            break; 
    }
    
}//end ADC_maq

//=========Fun��es vis�veis ao usu�rio===========//
void parada() 
{   
    //cruzamento
    //branco = 0, preto = 1
    static unsigned char flag_count = 0;
    
    //leitura de marcador de parada
    if ((tst_bit(leitura_curva, sensor_de_curva)) && (!tst_bit(leitura_parada, sensor_de_parada)) && !flag_count)
    {
        flag = 1;
        flag_count = 1;
        set_bit(PORTB, PB5);
    }
    
    else if ((!tst_bit(leitura_curva, sensor_de_curva)) && (!tst_bit(leitura_parada, sensor_de_parada)) && flag_count) //verifica se � crizamento
    {
        flag = 0;
        flag_count = 1;
        clr_bit(PORTB, PB5);
    }
    
    else if ((tst_bit(leitura_curva, sensor_de_curva)) && (tst_bit(leitura_parada, sensor_de_parada)) && flag_count)
    {
        flag = 0;
        flag_count = 0;
        clr_bit(PORTB, PB5);
    }
    else if (!(tst_bit(leitura_curva, sensor_de_curva)) && (tst_bit(leitura_parada, sensor_de_parada)) && flag_count)
    {
        flag = 0;
        flag_count = 0;
        clr_bit(PORTB, PB5);
    }


}

void calibra_sensores() 
{
    //=====Fun��o que inicializa a calibra��o====//
    /*for (int i = 0; i < 120; i++) {
        for (int i = 0; i < 5; i++) {
            if (sensores_frontais[i] < valor_min [i]) {
                valor_min[i] = sensores_frontais[i];
            }
            if (sensores_frontais[i] > valor_max [i]) {
                valor_max[i] = sensores_frontais[i];
            }
        }

        //_delay_ms(20);  //tempo o suficiente para o pessoa calibrar os sensores mecanicamente
        
        
        //Ap�s isso determinar o limiar de todos os sensores para que eles tenham os mesmos valores do AD. 
        //Para que todos tenham um limite inferior e superior igual.
        
    }*/

}

void seta_calibracao() {
    //----> Calibra��o dos Sensores frontais <----//

    //fun��o que seta o limiar dos sensores
    //Este � o algoritmo a ser usado no rob�. Desmcomente antes de compilar e comente o outro.
    /*for (int i = 0; i < 5; i++) {
        if (valor_min [i] > valor_min_abs && valor_min[i] !=0 ) //esse !0 foi colocado pois estava havendo um bug ao simular
        {
            valor_min_abs = valor_min [i];
        } 
        
        if (valor_max [i] < valor_max_abs) {
            valor_max_abs = valor_max [i];
        }
        

    }*/
    valor_min_abs = 100; //valores vistos pelo monitor serial
    valor_max_abs = 200;
}

void sensores() 
{

    //======Estabelece o limiar da leitura dos sensores====//
    //fun��o de corre��o da calibra��o
    for (int i = 0; i < 5; i++)
    {
        if (sensores_frontais[i] < valor_min_abs) 
        {
            sensores_frontais[i] = valor_min_abs;
        }
        if (sensores_frontais[i] > valor_max_abs)
        {
            sensores_frontais[i] = valor_max_abs;
        }

    }
    
}

void Auto_calibration(void)/*Sem uso*/
{
    static unsigned char flag_D = 0, flag_E = 0;
    /*Calibra��o autom�tica
     Rob� gira um dos motores num sentido
    *num intervalo de tempo e depois mudar o sentido de giro.
    *Em seguida fazer o mesmo com a outra roda.*/
    
    if(!flag_D)
    {
        direita_frente();
        setDuty_1(300);
        setDuty_2(0);
        flag_D = 1;
    }
    
    else if(flag_D)
    {
        direita_tras();
        setDuty_1(300);
        setDuty_2(0);
        flag_E = 1;
    }
    
    else if(flag_E)
    {
        esquerda_frente();
        setDuty_1(0);
        setDuty_2(300);
        flag_E = 0;
    }
    
    else if(!flag_E)
    {
        esquerda_tras();
        setDuty_1(0);
        setDuty_2(300);
        f_motor = 1;
    }
}

void sentido_de_giro()
{
    //-----> �rea do senstido de giro       

    if ((sensores_frontais[0] < 101 && sensores_frontais[4] > 190) || (sensores_frontais[0]  > 190 && sensores_frontais[4] < 101))    
        //Valores vistos na serial
        //se o primeiro sensor ou o �ltimo sensor estiverem lendo branco...
        //necess�rio teste com monitor serial
        //estudar a melhor quantidade de sensores e seu espa�amento
    {
        PWMA = PWM_Curva - u;
        PWMB = PWM_Curva + u;
        frente();
        PWM_limit();
        setDuty_1(PWMA);
        setDuty_2(PWMB);
    } //em cima da linha
        
    else
    { 
        //pra frente - reta
        //--------------->AREA DO PID<---------------

        PWMA = PWMR - u;
        PWMB = PWMR + u;
        frente();
        PWM_limit();
        setDuty_1(PWMA);
        setDuty_2(PWMB);
    }
    
    volta_pra_pista();
    
    //sprintf(buffer, "%d\n", u);
    //UART_enviaString(buffer);

    
    //sprintf(buffer, "%d\t%d\n", PWMA, PWMB);
    //UART_enviaString(buffer);
}

void PWM_limit() {
    //------> Limitando PWM
    static int ExcessoB = 0, ExcessoA = 0;
    
    if (PWMA > 1023)
    {
      ExcessoB = (PWMA - 1023);
      PWMA = 1023;
      PWMB -= ExcessoB;
    }

    else if (PWMB > 1023)
    {
      ExcessoA = (PWMB - 1023);
      PWMB = 1023;
      PWMA -= ExcessoA;
    }

    if (PWMA < 0)
    {
      ExcessoB = (PWMA*(-1) * 2);
      PWMA += (PWMA*(-1)*2);
      PWMB += ExcessoB;
    }

    else if (PWMB < 0)
    {
      ExcessoA = (PWMB*(-1) * 2);
      PWMB += (PWMB*(-1)*2);
      PWMA += ExcessoA;
    }        

}


void volta_pra_pista(void)
{    
    if ((sensores_frontais[1] < 101) && (sensores_frontais[3] > 190))//curva � esquerda
    {
      if (sensores_frontais[2] > 107)
      {
          
        giro_esquerda();
        setDuty_1(PWMA);
        setDuty_2(PWMB);

      }
    }
    
    else if ((sensores_frontais[3] < 101) && (sensores_frontais[1] > 190))//curva � direta
    {
      if (sensores_frontais[2] > 107)
      {
        
        giro_direita();
        setDuty_1(PWMA);
        setDuty_2(PWMB);
      }  
    }
    
    else if ((sensores_frontais[4] > 190) && (sensores_frontais[0] > 190))//saindo da pista, curva � esquerda
    {
        if(sensores_frontais[2] > 190)
        {
            giro_esquerda();
            setDuty_1(PWMA);
            setDuty_2(PWMB); 
        }
        
        else
        {
            frente();
        }
    }
    /*Fim de �rea para voltar para a pista*/
    //Obs.: Os valores mudam de acordo com o N� de sens. e suas posi��es
    //bem como a calibra��o dos mesmos.
}

void volta_pra_pista_calibracao(void)/*A ser usado em uma calib. auto.*/
{    
    if ((sensores_frontais[4] < 200) && (sensores_frontais[0] < 200))
    {
            while (sensores_frontais[1] < 101 && sensores_frontais[2] < 120)
            {
        
                giro_esquerda();
                setDuty_1(PWM_Curva);
                setDuty_2(PWM_Curva);

            }  
    }
    
    else if ((sensores_frontais[4] < 200) && (sensores_frontais[0] < 200))
    {
            while (sensores_frontais[3] < 100 && sensores_frontais[2] < 99)
            {
        
                giro_direita();
                setDuty_1(PWM_Curva);
                setDuty_2(PWM_Curva);

            }  
    }
}

void calculo_do_erro()
{
    int soma_esquerdo = 0, soma_direito = 0;
    static int denominador = 6;
    int soma_total = 0;   //caso aumente o peso da m�dia_ponderada, tomar cuidado com a vari�vel char
    
    static int peso [] = {-2, -1, 0, 1, 2};
    //os pesos precisar�o ser corrigidos pois os sensores do Van Grogue est�o um pouco assim�tricos
    
    for (int j = 0; j < 2; j++) 
    {
        soma_esquerdo += (sensores_frontais[j] * peso[j]);
        soma_direito  += (sensores_frontais[4-j] * peso[4-j]);
    }

    soma_total = (soma_esquerdo + soma_direito)/ denominador;
    
    erro = 14 - soma_total;   //valor esperado(estar sempre em cima da linha) - valor medido

    
    if(erro > 33)   //corrigindo assimetria(tentando)
    {
        erro = 33;
    }
    
    if(erro < -33)
    {
        erro = -33;
    }
    
    u = PID(erro);
    
    /*for(int i = 0; i < 5; i++)
    {
        sprintf(buffer, "%d\t", sensores_frontais[i]);
        UART_enviaString(buffer);
    }
    UART_enviaCaractere('\n');*/
    
    //sprintf(buffer, "%d\n", erro);
    //UART_enviaString(buffer);
    
}


void estrategia()
{
    
    if (!f_parada)              //se f_parada for 0... 
    {
        sensores();             //seta o limiar da leitura dos sensores
        calculo_do_erro();      //faz a m�dia ponderada e calcula o erro
        sentido_de_giro();      //Verifica se precisa fazer uma curva e o c�lculo do PID
    }
        
}

void fim_de_pista()
{
    static char parada = 0;
    
    if(flag)
    {
       parada++;
       flag = 0;
    }
    
    
    if(parada > 1)  //dois marcadores de parada
    {
        //f_parada = 1;
        //freio();
        parada = 0;
    }
    
}

void f_timers (void) {

    static unsigned char c_timer1 = 0;
    static unsigned char c_timer2 = 0;
    //static unsigned char c_timer3_ms = 0, c_timer3 = 0;
    if(f_calibra)
    {
        
        //fun��es a cada 200us
        if(c_timer1 < 2-1)
        {
            c_timer1++;
        }
        
        else
        {
            parada();
            fim_de_pista();         //Verifica se � o fim da pista
            c_timer1 = 0;
        }
        
        /*300us*/
        if (c_timer2 < 3-1)   //o 0 conta na contagem -> 4-1
        {
            c_timer2++; //100us -1; 200us-2;300 us-3; 400us de intervalo de tempo
        }
        
        else    //a cada 300us
        {
            estrategia();
            c_timer2=0;
        }
        
    }
    
    else
    {
        /**if(c_timer3_ms < 100 - 1)   //10ms
        {
            c_timer3_ms++;
        }
        
        else
        {
            if(c_timer3 < 200 - 1)  //2000ms = 2s
            {
                c_timer3++;
            }
            
            else 
            {   
                if(!f_motor)
                {
                    Auto_calibration();
                }
                c_timer3 = 0;
            }
            c_timer3_ms = 0;
        }*/
        
    }
}//fim do programa

/*Observa��es:
  Foram utilizados somente 5 sensores pois o m�dulo est� assim�trico em rela��o ao rob�, e
  a forma mais simples de corrigir isso � usando os 5 sensores,
  mesmo n�o estando 100% sim�trico.
  Por causa disso foi necess�rio alterar o setpoint do erro para termos um erro = 0
  quando o rob� estivesse acima da linha.
  Foi visto tamb�m pela serial o limite superior e inferior dos sensores e foram
  setados em valor_max_abs e valor_min_abs para testes mais pr�ticos.
  Al�m disso foi feito uma leitura serial dos sensores em uma curva,
  virando tanto pra esquerda quanto para a direita para saber os valores AD
  dos sensores extremos em cada situa��o para se iniciar uma curva*/
