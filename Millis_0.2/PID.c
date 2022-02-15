#include "PID.h"

#define frontais 1

int PID(int error, unsigned char dado) /* Algoritmo de controle PID usando os sensores frontais */
{
    // =============================================================================================
    // Variáveis locais
    static int Kp = 0, Kd = 0, Ki = 0;
    static int prescale = 0; //prescale na potência de 2: 2^n

    if(dado == frontais)
    {
        Kp = 12, Kd = 64, Ki = 0;
        prescale = 6; //prescale na potência de 2: 2^n //6 = 64

    }
    
    else    /*encoders*/
    {
        Kp = 0, Kd = 0, Ki = 0; 
        prescale = 0; //prescale na potência de 2: 2^n //0 = 1

    }    
    static int integral = 0;
    static int erroAnterior = 0;
    static int p = 0, i = 0, d = 0;
    int PID = 0;
    
    // =============================================================================================
    // Desenvolvimento
    
    if(!error)  integral = 0;
    
    p = (error * Kp);  // Proporcao

    integral += error; // Integral
    
         if(integral >  1023)   integral =  1023; // Limita o valor da integral
    else if(integral < -1023)   integral = -1023;
    
    i = (Ki * integral);

    d = (Kd * (error - erroAnterior)); // Derivada
    
    erroAnterior = error;
    
    PID = (p + i + d) >> prescale; // Divisão: (p + i + d) / 2^prescale
    
    return PID; // retorna o valor do cálculo do PID

} /* end PID */