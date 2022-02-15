#include "sensors.h"
#include "PID.h"
#include "ADC.h"
#include "PWM.h"
#include "motores.h"

#define PID_X       // Ao comentar o PID translacional e desabilitado
#define atmega328p

#define frontais 1
#define encoders 2

// =============================================================================================
// == Variaveis globais ==
// =======================

extern unsigned char SW;                    // Switch de estratégias

unsigned char sensores_de_tensao [2];
unsigned char v_bat;                        // Tensão na bateria
unsigned char sensores_frontais = 0,
              sensores_laterais = 0;        // NEW

// =============================================================================================
// ====             Desenvolvimento de funcoes da biblioteca sensores.h                      ===
// =============================================================================================

void sensors_ADC_maq () /* leitura de tensão da bateria e do switch de estratégias 
                         * em que seu sinal será digitalizado */
{
    /* inicializo no setup na função calibration e em seguida toda
     * vez que ocorre uma converção a interrupção do AD ocorre
     * e então esta função é chamada pelo vetor de interrupção
     * do AD, obtendo os dados da conversão em "paralelo" à rotina */
    
    /* Leio primeiro o default que seria o primeiro canal
     * e em seguida faço uma lógica circular de leitura dos canais */
    
    static unsigned char estado = 10;
    
    switch (estado) {
        
        case 0:
            estado = 1;
            sensores_de_tensao[0] = ADC_ler();
            
            SW = sensores_de_tensao[0];     /* leitura do switch para estratégia
                                             * o Switch está sendo utilizado em 
                                             * uma porta AD por falta de entrada digital */
            ADC_conv_ch(7);
            break;
            
        case 1:
            estado = 0;
            sensores_de_tensao[1] = ADC_ler();
            v_bat = sensores_de_tensao[1];      //sensor de tensao de bateria
            ADC_conv_ch(6);
            break;
            
            
        default:
            estado = 0;
            ADC_conv_ch(6);
            sensores_de_tensao[0] = ADC_ler();
            break; 
    }
    
} /* end ADC_maq */

void sensors_laterais(void)
{
//  =============================================================================================
//  == Area de pre compilacao ==
//  ============================
    
    #ifdef atmega328p
    
        sensores_laterais = PINB & 0b00011000; // NEW : LEITURA SOMENTE DOS PINOS PB3 E PB4
    
    #endif
    
//  =============================================================================================
//  == Variaveis tipo flag ==
//  =========================
    
    extern void fim_de_pista(); // usada na main.c
        
    extern bool flag_curva;     // usada em dados.c
    extern bool flag_parada;    // usada em dados.c
    
    static bool flag_count = 0; // para nao ler duas vezes o mesmo marcador
    
//  =============================================================================================
//  == Testes dos sensores laterais ==
//  ==================================
    
    switch(sensores_laterais)
    {
        /* cruzamento  | 00 */
        case  0: 
  
            flag_count = 1;
            flag_curva = 0;
            clr_bit(PORTB, led_placa);
            break;
        
        /* curva | 01 */
        case  8:  
            
            if(flag_curva) return;
            flag_count = 1;
            flag_curva = 1;
            clr_bit(PORTB, led_placa);           
            
            break;
        
        /* parada | 10 */ 
        case 16: 
           
            if(flag_count) return;
            fim_de_pista();
            flag_count  = 1;
            flag_parada = 1;
            flag_curva  = 0;
            set_bit(PORTB, led_placa);
            break;
        
        /* sem marcador | 11 */
        case 24:  
            
            flag_count = 0;
            flag_curva = 0;
            clr_bit(PORTB, led_placa);
            break;
            
    }
    
} /* end sensors_laterais */

void sensors_sentido_de_giro()
{   
    /* O erro final precisa ser melhorado (acompanhar o relatório da trinca) */
    
    extern unsigned int PWMA, PWMB;
    extern unsigned int PWMR;                               // PWM do motor ao entrar na reta
    extern unsigned int PWM_Curva;                          // PWM do motor ao entrar na curva
    
    // ---> Área do senstido de giro <---
    
    static          int u_W = 0;                            // resultado do PID rotacional
    static          int u_X = 0;                            // resultado do PID translacional
    
    static unsigned int PWM_general = 0;
       
    static int  delta_enc = 0, erroX = 0, speedX;           // speedX é o setpoint da vel. desejada
    extern char pulse_numberL, pulse_numberR;               // numero de pulsos do dois encoders       
    static int  erro_sensores = 0, erroW = 0, speedW = 0;   // speeW é o setpoint do PID rotacional.
    
    speedX = 100;   // Velocidade/PWM desejado
        
//  =============================================================================================
//  == Area de pre compilacao ==
//  ============================
               
    #ifdef PID_X                                    /* caso não seja definido, u_X será sempre 0
                                                     * variação entro os dois enconders */
        delta_enc = pulse_numberR + pulse_numberL;   
        erroX = speedX - delta_enc; // Variável de processo (PV)
        u_X   = PID(erroX, encoders); // Variável manipulada  (MV)
    
    #endif 

    sensors_frontais(&erro_sensores, &speedW, &speedX, &PWM_general, &PWMR, &PWM_Curva);
    
//  =============================================================================================
//  == Desenvolvimento ==
//  =====================
    
    erroW = speedW - erro_sensores;   // Calculo do erro rotacional
    u_W   = PID(erroW, frontais);               // Envia o erro para ser calculada a variável de correcao em PID()
    
    PWMA = PWM_general + u_W + u_X; // Valores de correcao "u_W" e "u_X" calculados e atualizando o PWMA
    PWMB = PWM_general - u_W + u_X; // Valores de correcao "u_W" e "u_X" calculados e atualizando o PWMB
    
    PWM_limit();                    // Impede que os valores do PWMA e PWMB ultrapassem o limite pré-definido
    
    PWM_setDuty_1(PWMA);            // Envia o valor de PWM calculado para o motor_1
    PWM_setDuty_2(PWMB);            // Envia o valor de PWM calculado para o motor_2
    
    //PWM_calc_duty(PWMA);                // Converte o valor de PWMA atual para duty_cycle_A
    //PWM_calc_duty(PWMB);                // Converte o valor de PWMB atual para duty_cycle_B
    
    /* Há dois PIDs presentes, o translacional(u_X) e o rotacional(u_W),
     * ambos os setpoints variam dependendo da situação que o robô se enconrtra
     * são necessários testes para saber em determinadas situações qual deve 
     * ser o setpoint de cada PID */
    
} /* end sensors_sentido_de_giro */

void sensors_frontais(int *erro_sensores, int *speedW, int *speedX, unsigned int *PWM_general, unsigned int *PWMR, unsigned int *PWM_Curva)
{
    
//  =============================================================================================
//  == Area de pre compilacao ==
//  ============================
    
    #ifdef atmega328p
    
        sensores_frontais = PINC & 0b00011111;   /* Apago somente os 3 bits mais significativos
                                                  * para ler os 5 LSBs */
    #endif
   
//  =============================================================================================
//  == Desenvolvimento ==
//  =====================
        
    /* foi feito um switch case com base em alguns casos que os sensores frontais
     * poderiam se encontrar. Os valores de leituras do vetor de sensores foi convertido 
     * em digital, mais tarde será feito uma imagem mostrando os caso de forma mais visível */
    
    switch (sensores_frontais)
    {
        case 0 :    //cruzamento
            *erro_sensores = 0;
            *speedW = 0;                 // em uma reta ou cruzamento o rotacional é zero
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
            
        case 14 :                       // volta pra pista, gira em torno do próprio eixo
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
            
        case 30 :                   //volta pra pista, gira em torno do próprio eixo
            *erro_sensores = -8;
            #ifdef PID_X 
                *speedX = 0;
            #endif
            *PWM_general = *PWMR; 
            motores_giro_esquerda();
            break;      
    }

} /* end sensors_frontais */