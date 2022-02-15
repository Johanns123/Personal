#define COLETA 1
#define TELEMETRIA 2

#include "dados.h"
//#include "sensor_logic.h"

/*Variaveis globais desta bilioteca*/
unsigned int matriz_telemetria [1][3];    //matriz de colhimento de dados
unsigned int matriz_pista      [1][3];     //colhe somente distancia, tempo e raio
unsigned int dist_right = 0, dist_left = 0;
char buffer[5]; //String que armazena valores de entrada para serem printadas


unsigned int dados_valor_pwm(void)
{
    extern unsigned int PWMA, PWMB;    
    return ((PWMA + PWMB) / 2);
}


unsigned int dados_distancia_calculo(void)
{
    static int dist = 0;    
    extern unsigned char pulse_numberL, pulse_numberR;    
    dist_right = pulse_numberR * 0.812; //converte o numero de pulsos em mm
    dist_left = pulse_numberL * 0.812;    
    pulse_numberR = 0x00;
    pulse_numberL = 0x00; //zera as variaveis de calculo do raio da curva    
    dist = (dist_right + dist_left) / 2;    //em uma reta    
    /*Criar condicoes para reta e curva e criar funcao de curva*/
    return dist;
}


unsigned int dados_calculo_do_raio() //esta funcao calcula o raio a partir da distancia percorrida pelas duas rodas do robo
{
    static unsigned int raio_f = 0;            //raio
    unsigned int diametro = 126;        //126mm, diametro sas rodas
    if((dist_right - dist_left) < 1 || (dist_right - dist_left) > -1)
    {    
        raio_f = (diametro / 2.0) * ((dist_right + dist_left) / (dist_right - dist_left)); //Calculo do raio em modulo
    }    
    else raio_f = 0;  
    
    return abs(raio_f);
}


unsigned int dados_velocid_linear()
{
    static unsigned int distancia_f = 0;
    static unsigned int velocidade  = 0;    
    distancia_f = dados_distancia_calculo();    
    velocidade  = distancia_f / 0.5;     //calculo da velocidade em mm/s    
    return velocidade;
}

unsigned int dados_speed_avrg(void)
{
    return(dados_velocid_linear() / 0.5);     //calculo da aceleracao em mm/s^2
}


void dados_envia(unsigned char tipo_de_dado)
{
    if(tipo_de_dado == TELEMETRIA)
    {
        
        for(int coluna = 0; coluna < 3; coluna++)
        {
            UART_enviaHex(matriz_telemetria[0][coluna]);
        }
        UART_enviaCaractere('\n');
          
    }    
    else if(tipo_de_dado == COLETA)
    {
        for(int coluna = 0; coluna < 3; coluna++)
        {
            UART_enviaHex(matriz_pista[0][coluna]);
        }
        UART_enviaCaractere('\n');  
    }
}


void dados_coleta(void)
{  
    static bool f_record = 0;   
    static unsigned int i = 0;  // variavel de posisao da linha da matriz    
    extern bool flag_parada,
                flag_curva,     // flag que indica uma curva
                f_parada;       // flag geral que para o robo
    extern bool f_stop;
    extern unsigned int millisegundos;
    if(flag_parada)//Passou pelo primeiro marcador de parada?
    { 
        if(flag_curva && !f_record)//eh uma curva?
        {
            matriz_pista[0][tempo]      = millisegundos;              // tempo em milissegundos
            matriz_pista[0][distancia]  = dados_distancia_calculo();  // distancia em mm
            matriz_pista[0][raio]       = dados_calculo_do_raio();    // raio em mm 
            millisegundos = 0;                                        // zerando o timer;
            dados_envia(COLETA);
            f_record = 1;                                             // flag para gravar uma vez
        }
        else if(!flag_curva && f_record)
        {
            f_record = 0;
        }                      
        if(f_parada)//Robo parou?
        {
            matriz_pista[i][tempo]      = millisegundos;
            matriz_pista[i][distancia]  = dados_distancia_calculo();
            dados_envia(COLETA);
            millisegundos = 0;          // zerando o timer
            flag_parada = 0;
            f_stop = 1;                 // encerra a rotina de dados 
        }        
    }
    
}


void dados_telemetria(void)
{    
    matriz_telemetria[0][vel_linear]    = dados_velocid_linear();
    matriz_telemetria[0][pwm_medio]     = dados_valor_pwm();
    matriz_telemetria[0][acel_medio]    = dados_speed_avrg();  
    dados_envia(TELEMETRIA);
    
}