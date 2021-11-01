int PID(int error)/*Algoritmo de controle PID usando os sensores frontais*/
{
<<<<<<< Updated upstream
    static int Kp = 10, Kd = 4, Ki = 0;
    static int prescale = 2; //prescale na pot�ncia de 2: 2^n //4 = 16
=======
    static int Kp = 12, Kd = 28, Ki = 0;
    static int prescale = 6; //prescale na pot�ncia de 2: 2^n //4 = 16
>>>>>>> Stashed changes
    static int integral = 0;
    static int erroAnterior = 0;
    static int p = 0, i = 0, d = 0;
    int Turn = 0;
   
    p = (error * Kp); // Proporcao

    integral += error; // Integral
    i = (Ki * integral);

    d = (Kd * (error - erroAnterior)); // Derivada
    erroAnterior = error;

    Turn = (p + i + d) >> prescale;
    
    
    return Turn;
}