#include "dados.h"
//#include "sensor_logic.h"

/*Vari�veis globais desta bilioteca*/
unsigned int matriz_telemetria [100][3];   //matriz de colhimento de dados
unsigned int matriz_pista      [30][3];    //colhe somente distancia, tempo e raio
unsigned int dist_right = 0, dist_left = 0;
bool f_dados_t = 0, f_dados_c = 0;         //t = telemetria, c = coleta
char buffer[5]; //String que armazena valores de entrada para serem printadas


unsigned int dados_valor_pwm(void)
{
    extern unsigned int PWMA, PWMB;
    return ((PWMA + PWMB) / 2);     //retorna a m�dia dos PWMs dos dois motores
}

unsigned int dados_distancia_calculo(void)
{
    static int dist = 0;
    
    extern unsigned char pulse_numberL, pulse_numberR;
    
    dist_right = pulse_numberR * 0.812; //converte o n�mero de pulsos em mm
    dist_left = pulse_numberL * 0.812;
    
    pulse_numberR = 0x00;
    pulse_numberL = 0x00; //zera as vari�veis de c�lculo do raio da curva

    
    dist = (dist_right + dist_left) / 2;    //em uma reta
    
    /*Criar condi��es para reta e curva e criar fun��o de curva*/
    return dist;
}


unsigned int dados_calculo_do_raio() //esta fun��o calcula o raio a partir da dis�ncia percorrida pelas duas rodas do rob�
{
    static unsigned int raio_f = 0;            //raio
    unsigned int diametro = 126;        //126mm, di�metro sas rodas
    
    if((dist_right - dist_left) < 1 || (dist_right - dist_left) > -1)
    {
    
        raio_f = (diametro / 2.0) * ((dist_right + dist_left) / (dist_right - dist_left)); //C�lculo do raio em m�dulo
        
        if(raio_f < 0)
        {
            raio_f *= -1;       //m�dulo
        }
    
    }
    
    else
    {
        raio_f = 0;
    }
        
        /*sprintf(buffer, "Raio %5d\n", raio_f); //Converte para string
        UART_enviaString(buffer); //Envia para o computador
        UART_enviaCaractere(0x0D); //pula linha*/
 

    return raio_f;
}


unsigned int dados_velocid_linear()
{
    static unsigned int distancia_f = 0;
    static unsigned int velocidade  = 0;
    
    distancia_f = dados_distancia_calculo();
    
    velocidade  = distancia_f / 0.5;     //c�lculo da velocidade em mm/s
    
    return velocidade;
}

unsigned int dados_speed_avrg(void)
{
    return(dados_velocid_linear() / 0.5);     //ca?culo da acelera��o em mm/s�
}


void dados_envia()
{
    if(f_dados_t)
    {
        for(int linha = 0; linha < 100; linha++)
        {
            for(int coluna = 0; coluna < 3; coluna++)
            {
                //sprintf(buffer, "%d\t", matriz_telemetria[linha][coluna]);
                //UART_enviaString(buffer);
            }
            //UART_enviaCaractere('\n');
        }  
        f_dados_t = 0;
    }
    
    if(f_dados_c)
    {
      for(int linha = 0; linha < 30; linha++)
        {
            for(int coluna = 0; coluna < 3; coluna++)
            {
                //sprintf(buffer, "%d\t", matriz_pista[linha][coluna]);
                //UART_enviaString(buffer);
            }
            //UART_enviaCaractere('\n');
        }  
        f_dados_c = 0;  
    }
}



void dados_coleta(void)
{
  
    static bool f_record = 0;
    
    static unsigned int i = 0;  //vari�vel de posi��o da linha da matriz
    
    extern bool flag_parada, flag_curva, f_parada;
    extern bool f_stop;
    extern unsigned int millisegundos;
    
    if(flag_parada)//criar uma fun��o para printar na tela
    { 
        if(flag_curva && !f_record)
        {
            matriz_pista[i][tempo]      = millisegundos;             //tempo em milissegundos
            matriz_pista[i][distancia]  = dados_distancia_calculo();  //dist�ncia em mm
            matriz_pista[i][raio]       = dados_calculo_do_raio();    //raio em mm 
            //sprintf(buffer, "%dms\t", millis());
            //UART_enviaString(buffer);
            millisegundos = 0;                  //zerando o timer
            //sprintf(buffer, "%.2fmm\t", distancia_calculo());  //dist�ncia em mil�metros
            //UART_enviaString(buffer);
            f_dados_c = 1;
            dados_envia();
            f_record = 1;                     //flag para gravar uma vez
        }

        else if(!flag_curva && f_record)
        {
            f_record = 0;
        }       
        
       
        if(f_parada)
        {
            matriz_pista[i][tempo]      = millisegundos;
            matriz_pista[i][distancia]  = dados_distancia_calculo();
            //sprintf(buffer, "%dms\t", millis());
            //UART_enviaString(buffer);
            millisegundos = 0;                  //zerando o timer
            //sprintf(buffer, "%.2fmm\t", distancia_calculo());  //dist�ncia em mil�metros
            //UART_enviaString(buffer);
            flag_parada = 0;
            f_stop = 1;
        }
        
        i++;                //pulo a linha da matriz
        
    }
}


void dados_telemetria(void)
{
    static int i = 0;
    
    matriz_telemetria[i][vel_linear]    = dados_velocid_linear();
    matriz_telemetria[i][pwm_medio]     = dados_valor_pwm();
    matriz_telemetria[i][acel_medio]    = dados_speed_avrg();
    
    f_dados_t = 1;
    
    dados_envia();
}
