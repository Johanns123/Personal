/*
 * File:   pwm.c
 * Author: Luis Alexandre
 *
 * Created on 16 de Fevereiro de 2021, 11:38
 */


#include <xc.h>

#include"pwm.h"


 void PWM2_Init(void)
  {
    //ciclo ativo -> PR2 + 1 = TMR2 (Quando há o estouro do TMR2, passamos de low para high)
    //borda de descida --> CCPR1L:CCP1CON<5:4>
    
    //periodo do pwm = (PR2 + 1) * ciclo de máquina * prescaler do timer2
    //periodo = (255+1) * 0,2us * 16 = 0,8192ms
    //frequencia = 1/periodo = 1220,70Hz = 1,22kHz

    PR2 = 0xFF;             //Inicializa o registrador de controle do timer 2 em 255
    T2CON = 0x06;           //Liga o timer2 e prescaler 1:16
    CCPR2L = 0x00;          //led iniciará desligado
    CCP2CON = 0x0C;        //habilita o PWM
     
  }


  void PWM2_Start(void)
  {

      TRISBbits.TRISB3=0; //liga saída PWM
      


      CCP2CON=0x0F; //CCP -> PWM mode 0x0F

      
      T2CONbits.TMR2ON=1;
 
      //espera PWM normalizar

      PIR1bits.TMR2IF=0;
      while(PIR1bits.TMR2IF == 0);
      PIR1bits.TMR2IF=0;
      while(PIR1bits.TMR2IF == 0);

     


  }


  void PWM2_Set_Duty(unsigned char d)
  {
      unsigned int temp;
      
      temp=(((unsigned long)(d))*((PR2<<2)|0x03))/255;

      CCPR2L= (0x03FC&temp)>>2;
      CCP2CON=((0x0003&temp)<<4)|0x0F;
  }

