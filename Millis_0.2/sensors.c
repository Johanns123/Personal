#include "sensors.h"
#include "PID.h"
#include "ADC.h"
#include "PWM.h"
#include "motores.h"

#define PID_X       //ao comentar, o PID rotacional � desabilitado

#define atmega328p

unsigned char sensores_de_tensao [2];
unsigned char v_bat; //tens�o na bateria
unsigned char SW;    //switch de estrat�gias
unsigned char sensores_frontais = 0;

void sensors_ADC_maq () //leitura de tens�o da bateria e do switch de estrat�gias em que seu sinal ser� digitalizado
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
            sensores_de_tensao[0] = ADC_ler();
            SW = sensores_de_tensao[0];     //leitura do switch para estrat�gia
            //o Switch est� sendo utilizado em uma porta AD por falta de entrada digital
            ADC_conv_ch(7);
            break;
            
        case 1:
            estado = 0;
            sensores_de_tensao[1] = ADC_ler();
            v_bat = sensores_de_tensao[1];  //sensor de tens�o de bateria
            ADC_conv_ch(6);
            break;
            
            
        default:
            estado = 0;
            ADC_conv_ch(6);
            sensores_de_tensao[0] = ADC_ler();
            break; 
    }
    
}//end ADC_maq



void sensors_le_marcadores(void)
{
    //cruzamento
    //branco = 0, preto = 1
    extern bool flag_curva;
    extern bool flag_parada;
    extern bool flag;
    static bool flag_count = 0;
    static bool s_curva = 0, s_parada = 0;
    
    s_curva =  (tst_bit(leitura_sensores, sensor_de_curva) >> sensor_de_curva);     //l� valor do sensor de curva

    //Utilizar as leituras numa fun��o e gurad�-los num char e seus �ltimos valores realizar
    //uma compara��o para ver a condi��o em que o rob� est�
    s_parada = (tst_bit(leitura_sensores, sensor_de_parada) >> sensor_de_parada);   //l� valor do sensor de parada
    //leitura de marcador de parada
    
    if ((s_curva) && (!s_parada) && !flag_count)
    {
        flag = 1;
        flag_count = 1;
        flag_parada = 1;
        flag_curva = 0;
        set_bit(PORTB, led_placa);
    }

    else if ((!s_curva) && (!s_parada)) //verifica se � crizamento
    {
        flag = 0;
        flag_count = 1;
        flag_curva = 0;
        clr_bit(PORTB, led_placa);
    }

    else if ((s_curva) && (s_parada))
    {
        flag = 0;
        flag_count = 0;
        flag_curva = 0;
        clr_bit(PORTB, led_placa);
    }
    else if (!(s_curva) && (s_parada) && !flag_curva)
    {
        flag = 0;
        flag_count = 0;
        flag_curva = 1;
        clr_bit(PORTB, led_placa);
    }
}

void sensors_sentido_de_giro()
{   
     //O erro final precisa ser melhorado (acompanhar o relat�rio da trinca)
    extern unsigned int PWMA, PWMB;
    //-----> �rea do senstido de giro       
    static int u_W = 0;                     //resultado do PID rotacional
    static unsigned int PWMR = 100;         // valor da for�a do motor em linha reta
    static unsigned int PWM_Curva = 80;     //PWM ao entrar na curva
    static unsigned int PWM_general = 0;
    static int  u_X = 0;                    //resultado do PID translacional
    
    static int delta_enc = 0, erroX = 0, speedX;     //speedX � o setpoint da vel. desejada
    extern char pulse_numberL, pulse_numberR;            //numero de pulsos do dois encoders
        
    static int erro_sensores = 0, erroW = 0, speedW = 0;  //speeW � o setpoint do PID rotacional.
    speedX = 100;   //velocidade/pwm desejado
    
    #ifdef atmega328p
    sensores_frontais = PINC & 0b00011111;   //apago somente os 2 bits mais significativos
                                            //para ler os 5 LSBs
    #endif
    
    sensors_leitura_de_pista(&erro_sensores, &speedW, &speedX, &PWM_general, &PWMR, &PWM_Curva);
    
    #ifdef PID_X                //caso n�o seja definido, u_X ser� sempre 0
    delta_enc = pulse_numberR + pulse_numberL;            //varia��o entro os dois enconders
    erroX = speedX - delta_enc;
    u_X = PID_encoder(erroX);
    #endif
    erroW = speedW - erro_sensores;
    u_W = PID(erroW);
    PWMA = PWM_general + u_W + u_X;
    PWMB = PWM_general - u_W + u_X;
    PWM_limit();
    PWM_setDuty_1(PWMA);
    PWM_setDuty_2(PWMB);

    //H� dois PIDs presentes, o translacional e o rotacional,
    //ambos os setpoints variam dependendo da situa��o que o rob� se enconrtra
    //s�o necess�rios testes para saber em determinadas situa��es qual deve ser o setpoint de cada PID
    
}

void sensors_leitura_de_pista(int *erro_sensores, int *speedW, int *speedX, unsigned int *PWM_general, unsigned int *PWMR, unsigned int *PWM_Curva)
{
    //foi feito um switch case com base em alguns casos que os sensores frontais poderiam se encontrar
    //os valores de leituras do vetor de sensores foi convertido em digital, mais tarde ser� feito uma imagem mostrando os caso de forma mais vis�vel
    switch (sensores_frontais)
    {
        case 0 :    //cruzamento
            *erro_sensores = 0;
            *speedW = 0;                 //em uma reta ou cruzamento o rotacional � zero
            *PWM_general = *PWMR;
            motores_frente();
            break;
        
        case 3 :
            *erro_sensores = 4;
            *PWM_general = *PWMR;
            motores_frente();
            break;
         
        case 7 :
            *erro_sensores = 6;
            *PWM_general = *PWM_Curva;
            motores_frente();
            break;
            
        case 14 :                       //volta pra pista, gira em torno do pr�prio eixo
            *erro_sensores = 8;
            #ifdef PID_X 
            *speedX = 0;
            #endif
            *PWM_general = *PWMR;
            motores_giro_direita();
            break;
           
        case 17 :
            *erro_sensores = 0;
            *speedW = 0;
            *PWM_general = *PWMR;
            motores_frente();
            break;
        
        case 19 :
            *erro_sensores = 2;
            *PWM_general = *PWMR;
            motores_frente();
            break;
            
        case 24 :
            *erro_sensores = -4;
            *PWM_general = *PWMR;
            motores_frente();
            break;
         
        case 25 :
            *erro_sensores = -2;
            *PWM_general = *PWMR;
            motores_frente();
            break;
            
        case 27 :
            *erro_sensores = 0;
            *speedW = 0;
            *PWM_general = *PWMR;
            motores_frente();
            break;
            
        case 28 :
            *erro_sensores = -6;
            #ifdef PID_X 
            *speedX = 0;
            #endif
            *PWM_general = *PWM_Curva;
            motores_frente();
            break;
            
        case 30 :                   //volta pra pista, gira em torno do pr�prio eixo
            *erro_sensores = -8;
            #ifdef PID_X 
            *speedX = 0;
            #endif
            *PWM_general = *PWMR; 
            motores_giro_esquerda();
            break;      
    }
}
/*void sensors_volta_pra_pista(void)
{    
    

    if (sensores_frontais == 14)//saindo da pista, curva � esquerda
    {
        motores_giro_esquerda();
        PWM_setDuty_1(PWM_RETURN);    //utilizar vari�vel fixa / define
        PWM_setDuty_2(PWM_RETURN); 
        
    }
    
    else if(sensores_frontais == 30)
    {
        motores_giro_direita();
        PWM_setDuty_1(PWM_RETURN);
        PWM_setDuty_2(PWM_RETURN); 
    }
    

}*/



