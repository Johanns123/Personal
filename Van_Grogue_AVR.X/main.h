/*Protótipo das funções*/
void setup();
void setup_logica();        //variáveis utilizadas na lógica
void loop();
void estrategia();          //estrategia do robô
//---------------------------------------------------------------//
void Auto_calibration(void);
void calibration();         //contêm toda a rotina de calibração
//--------------------------------------------------------------------//
void parada();              //Leitura dos sensores laterais
void fim_de_pista();        //verifica se é o fim da pista 
//---------------------------------------------------------------------//
void count_pulsesE();
void count_pulsesD();
void millis(void);

//---------------------------------------------------------------------//
void f_timers (void);       //função de temporização das rotinas
void f_timer1(void);
void f_timer2(void);
void f_timer3(void);
void f_timer4(void);
void f_timer5(void);
/*===========================================================================*/

/*Macros*/
#define NOP() __asm__ __volatile__ ("nop")