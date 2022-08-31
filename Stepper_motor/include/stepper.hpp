#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#define CLOCKWISE           1
#define COUNTERCLOCKWISE    0


enum stepper_mode
{
    Full_swing_High = 1, Full_swing_Easy, Half_swing_hard 
};

class Stepper
{

    public:
    void passo_completo_alto_torque(void);  //define qual tipo de configuracao
    void passo_completo_baixo_torque(void);
    void meio_passo(void);
    void set_stepper_motor_mode(uint8_t mode);  //seleciona qual o modo de operacao
    void Full_torque(void); //atua as funções para girar no modo 1
    void Half_torque(void); //atua as funções para girar no modo 2
    void High_precision(void); //atua as funções para girar no modo 3
    void select_angle(uint16_t angle, bool direction);  //seleciona o angulo de giro e a direcao
    bool run(void); //inicia o motor, termina o giro ao completar o angulo retornando um boolean
    void clear(void);   //limpa variaveis de controle do run()
    uint8_t chose_speed(uint8_t time); //seleciona a velocidade/tempo de acionamento das bobinas
    void select_direction(uint8_t direction); //seleciona a direção do giro
    void start_motor(uint8_t direction);  //inicia o motor
    
    private:
        /* data */
    
};



void passo_completo_alto_torque(void);  //define qual tipo de configuracao
void passo_completo_baixo_torque(void);
void meio_passo(void);
void set_stepper_motor_mode(uint8_t mode);  //seleciona qual o modo de operacao
void Full_torque(void); //atua as funções para girar no modo 1
void Half_torque(void); //atua as funções para girar no modo 2
void High_precision(void); //atua as funções para girar no modo 3
void select_angle(uint16_t angle, bool direction);  //seleciona o angulo de giro e a direcao
bool run(void); //inicia o motor, termina o giro ao completar o angulo retornando um boolean
void clear(void);   //limpa variaveis de controle do run()
uint8_t chose_speed(uint8_t time); //seleciona a velocidade/tempo de acionamento das bobinas
void select_direction(uint8_t direction); //seleciona a direção do giro
void start_motor(uint8_t direction);  //inicia o motor