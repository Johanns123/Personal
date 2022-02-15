/*
 * File:   main.c
 * Author: Johann
 *
 * Created on 30 de Maio de 2021, 16:52
 */

/* Este codigo eh de um robo seguidor de linha da equipe Wolfbotz.
 * Aqui nos vemos o controle do robo bem como as tomadas de decisao de acordo com os padroes da pista
 */

#define TOMADA_DE_TEMPO 1
#define MAPEAMENTO 2

#define numero_de_trechos 10

#include "main.h"

/*Variaveis globais*/
unsigned int PWMA = 0, PWMB = 0; // Modulacao de largura de pulso enviada pelo PID
unsigned int PWMR;               // PWM do motor ao entrar na reta
unsigned int PWM_Curva;          // PWM do motor ao entrar na curva

//variaveis de controle
bool f_parada    = 0;   // variaavel que comanda quando o robo deve parar e nao realizar mais sua rotina
bool flag        = 0;   // variavel de controle para identificar o momento de parada
bool flag_curva  = 0;   // cronometragem entre as retas e as cruvas
bool flag_parada = 0;   // inicia e encerra a cronometragem da pista
bool f_stop      = 0;   // encerra a rotina de dados

unsigned int  millisegundos = 0;     // millisegundos
unsigned char vect_duty_cycle[101];  // valores de duty_cycle
unsigned int  vect_PWM[512];         // valroes de PWM

unsigned char SW;               // switch de estrategia
unsigned char estrategia = 0;

/*Variaveis do encoder*/
unsigned char pulse_numberR = 0, 
              pulse_numberL = 0; // variaveis para contagem dos pulsos dos encoders

/*Variaveis da UART*/
volatile char ch;           // armazena o caractere lido
volatile char flag_com = 0; // flag que indica se houve recepcao de dado*/

unsigned char max_timer1, max_timer2, max_timer3_ms, max_timer_3, max_timer4, max_timer5;   
int  u_X = 0;


/*Variáveis usadas na Running*/
unsigned char MaxSpeed[numero_de_trechos] = {0};             // velocidade máxima de cada trecho
unsigned char distancia_total[numero_de_trechos] = {0};    // dados encontrados no mapeamento
unsigned char valor_de_tolerancia[numero_de_trechos] = {0};  // distancia de tolerancia para o robô começar a frear

/*Estes valores serao inseridos manualmente apos o mapeamento*/

/*Interrupcoes*/
ISR(USART_RX_vect) {
    ch = UDR0;               // Faz a leitura do buffer da serial
    UART_enviaCaractere(ch); // Envia o caractere lido para o computador
    flag_com = 1;            // Aciona o flag de comunicacao
}

ISR(TIMER0_OVF_vect) 
{
    TCNT0 = 56; // recarrega o Timer 0 para que a contagem seja 100us novamente
    f_timers(); // funcao de temporizacao das rotinas   
}

ISR(ADC_vect)
{
    sensors_ADC_maq();  // maquina de estado do conversor AD
}


ISR(INT0_vect)
{
    count_pulsesD();
}


ISR(PCINT0_vect)
{
    count_pulsesE();
}


// Funcao principal
int main(void) 
{
    setup();
    while (1) loop();
    return 0;    
}

// ======================================================================================================
// ===================================== RTOS primitivo =================================================
// ======================================================================================================
// ============================== Parte nao visivel ao usuario ==========================================
// ======================================================================================================


void f_timers (void) /* Funcao e chamada a cada 100us */ 
{
    static unsigned char c_timer1 = 1;
    static unsigned char c_timer2 = 1;
    static unsigned char c_timer3 = 1;
    static unsigned char c_timer4 = 1;
    static unsigned char c_timer5 = 1;
        
    /* funcoes a cada 200us | max_timer1 = 2 */
    if(c_timer1 < max_timer1) c_timer1++;
    else
    {
        f_timer1();
        c_timer1 = 1;
    }

    /* funcoes a cada 300us | max_tiemr2 = 3 */
    // 100us -1; 200us-2;300 us-3; 300us de intervalo de tempo
    if (c_timer2 < max_timer2) c_timer2++;
    else 
    {
        f_timer2();
        c_timer2 = 1;
    }
    
    /* funcoes a cada 10ms | max_timer3_ms = 100 */
    if(c_timer3 < max_timer3_ms) c_timer3++;
    else
    {
        f_timer3();
        c_timer3 = 1;
    }

    /* funcoes a cada 1ms | max_timer4 = 10 */
    if(c_timer4 < max_timer4) c_timer4++;
    else
    {
        f_timer4();
        c_timer4 = 1;
    }
    
    /* funcoes a cada 1ms | max_timer5 = 10 */
    if(c_timer5 < max_timer5) c_timer5++;
    else
    {
        f_timer5();
        c_timer5 = 1;
    }    
}

// ======================================================================================================
// =========================== Funcoes nao viriaveis ao usuario =========================================
// ======================================================================================================
void setup() 
{
    disable_interrupts();    // desabilita todas as interrupcoes
    setup_Hardware();       // setup das IO's e das interrupcoes
    enable_interrupts();   // habilita todas as interrupcoes
    setup_variaveis();    // definicao das variaveis logicas
}

void setup_variaveis()
{    
    // Funcao que passa ponteiros para funcoes como parametro 
    max_timer1    = 2,
    max_timer2    = 3,
    max_timer3_ms = 100,
    max_timer_3   = 50, 
    max_timer4    = 10,
    max_timer5    = 10;   
    
    /* Conversao de PWM para duty */
    for(int i = 0; i < 101; i++)
    {
        vect_duty_cycle[i] = i*(511/100);
    }
    
    /* Conversao de duty para PWM */
    for(int j = 0; j < 512; j++)
    {
        vect_PWM[j] = j*(100/511);
    }    
    if(!SW)
    {
        estrategia  = TOMADA_DE_TEMPO;
        PWMR        = PWM_calc_pwm(40);  // duty: 40%
        PWM_Curva   = PWM_calc_pwm(30);  // duty: 30%    
    }    
    else
    {
        estrategia  = MAPEAMENTO;
        PWMR        = PWM_calc_pwm(10);  // duty: 10%
        PWM_Curva   = PWM_calc_pwm( 8);  // duty: 8%
    }
    
}


void loop() {} // loop vazio


void parada() 
{     
    sensors_laterais(); // faz a leitura dos sensores laterais e ativa as respectivas flags
    fim_de_pista();     // verifica se e o fim da pista, se verdadeiro = motor_off
}


void fim_de_pista()
{
    static char parada = 0;    
    if(flag)
    {
       parada++;
       flag = 0;
    }
    if(parada > 1)  // dois marcadores de parada = fim de pista
    {
        f_parada = 1;
        parada   = 0;
        motores_freio();
    } 
}


void tratar_estrategia(void) 
{   
    // Decisao em funcao da tomada de tempo ou mapeamento    
    if (f_parada) return;
    sensors_sentido_de_giro();
    if (estrategia == TOMADA_DE_TEMPO)
    {
        Running_Torricielli();
    }
}


/*funcoes do encoder*/
void count_pulsesD() 
{
    static bool direction_m;
    static bool Encoder_C1Last = 0;
    bool Lstate = (tst_bit(leitura_outros_sensores, encoder_C1D) >> encoder_C1D); // variavel de leitura de um dos pinos do encoderD
    if (!Encoder_C1Last && Lstate) 
    {
        // Verifica se Encoder_C1Last eh falso e Lstate e verdadeiro
        bool val = tst_bit(leitura_outros_sensores, encoder_C2D >> encoder_C2D); // variavel de leitura do segundo pino do encoderD
        if (!val && direction_m) direction_m = 0;       // sentido horario
        else if (val && !direction_m) direction_m = 1;  // sentido anti-horario
    }
    Encoder_C1Last = Lstate;
    if (!direction_m) pulse_numberR++; //sentido horario
    else pulse_numberR--;              //sentido anti-horario

}


void count_pulsesE()
{
    static bool direction_m;
    static bool Encoder_C1Last = 0;
    bool Lstate = (tst_bit(leitura_sensores, encoder_C1E) >> encoder_C1E); //variavel de leitura de um dos pinos do encoderE
    if (!Encoder_C1Last && Lstate) 
    { 
        //Verifica se Encoder_C1Last eh falso e Lstate eh verdadeiro
        bool val = (tst_bit(leitura_sensores, encoder_C2E) >> encoder_C2E); //Variavel de leitura do segundo pino do encoderE
        if (!val && direction_m) direction_m = 0;       //sentido horario
        else if (val && !direction_m) direction_m = 1;  //sentido anti-horario
    }
    Encoder_C1Last = Lstate;
    if (!direction_m) pulse_numberL++; //sentido horario
    else pulse_numberL--;              //sentido anti-horario
}

void millis(void)
{
    // static unsigned int f_read = 0;    
    millisegundos++;
}


/* tempo = 200us */
void f_timer1(void) 
{
    parada();       // analisa as condicoes dos sensores laterais de acordo com suas leituras    
}

/* tempo = 300us */
void f_timer2(void)
{
    tratar_estrategia();
}

/* tempo = 10ms */
void f_timer3(void)
{   
    sub_timer();
}

/* tempo = 1ms */
void f_timer4(void)
{
    millis();   // funcao chamada a cada 1ms 
}


/* tempo = 1ms */
void f_timer5(void)
{
    if(f_stop) return;    
    if(estrategia == MAPEAMENTO) dados_coleta();
}

void sub_timer(void)/*sub_timer em que conta uma função temporizada a cada 0,5s*/
{
    static unsigned char c_timer1 = 0;
    
    /* tempo = 500ms ou 0,5 segundos | max_timer_3 = 50 */
    if(c_timer1 < max_timer_3) c_timer1++;
    
    else 
    {   
        if(estrategia == MAPEAMENTO) dados_telemetria();
        c_timer1 = 1;
    }

}

// ======================================================================================================
// ===================================== FINAL DO CODIGO ================================================
// ======================================================================================================