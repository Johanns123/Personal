#include "PWM.h"

void PWM_limit()
{
    /*Vari�veis externas*/
    extern unsigned int PWMA, PWMB;
    
    //------> Limitando PWM
    static int ExcessoB = 0, ExcessoA = 0;
    
    if (PWMA > 10000)
    {
      ExcessoB = (PWMA - 10000);
      PWMA = 10000;
      PWMB -= ExcessoB;
    }

    else if (PWMB > 10000)
    {
      ExcessoA = (PWMB - 10000);
      PWMB = 10000;
      PWMA -= ExcessoA;
    }

    if (PWMA < 0)
    {
      ExcessoB = (PWMA*(-1) * 2);
      PWMA += (PWMA*(-1)*2);
      PWMB += ExcessoB;
    }

    else if (PWMB < 0)
    {
      ExcessoA = (PWMB*(-1) * 2);
      PWMB += (PWMB*(-1)*2);
      PWMA += ExcessoA;
    }        

}
