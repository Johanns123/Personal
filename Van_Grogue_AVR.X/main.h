/*Prot�tipo das fun��es*/
void setup();
void setup_logica();        //vari�veis utilizadas na l�gica
void loop();
void estrategia();          //estrategia do rob�
//---------------------------------------------------------------//
void Auto_calibration(void);
void calibration();         //cont�m toda a rotina de calibra��o
//--------------------------------------------------------------------//
void parada();              //Leitura dos sensores laterais
void fim_de_pista();        //verifica se � o fim da pista 
//---------------------------------------------------------------------//
void count_pulsesE();
void count_pulsesD();
void millis(void);

//---------------------------------------------------------------------//
void f_timers (void);       //fun��o de temporiza��o das rotinas
void f_timer1(void);
void f_timer2(void);
void f_timer3(void);
void f_timer4(void);
void f_timer5(void);
/*===========================================================================*/

/*Macros*/
#define NOP() __asm__ __volatile__ ("nop")