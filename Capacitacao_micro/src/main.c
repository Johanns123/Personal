#include "main.h"


int main()
{
  setup();
  while (1) loop();
  return 0;
}

void setup() 
{
  DDRD = 0x1e;  //0b0001 1110 - PD1, PD2, PD3, PD4 como saida e PD5 como entrada
  PORTD = 0x00;
  DDRB = 0xff;  //defino tudo como saida
  DDRC = 0x00;  //todos os pinos como saida
  TCCR0B = 0x03;  //prescaler de 64
  TCNT0 = 6;      //inicio a contar em 6 para um tempo de 1ms
  TIMSK0 = 0x01;
  PCICR = 0x04;  //habilito PCINT2
  PCMSK2 = 0x20; //habilito o pino PCINT21 do PCINT2  
  MaxTimer1 = 200;
  MaxTimer2 = 10;
  uart_setup(16);           //se colocar display no PD0 comentar esta linha
  adc_setup();              //configuro o conversor AD
  PWM_init();               //inicializo o PWM
  setup_pwm_setFreq(11);    //freq de 250Hz
  tratar_leitura_do_ADC();
  sei();
}

void loop() 
{
  toggle_bit(PORTD, PD2);
  _delay_ms(500);
}

void contador(void)
{
  static unsigned char contador1 = 1, contador2 = 1;

  if(contador1 < MaxTimer1) contador1++;

  else
  {
    contador1 = 1;
    rotina1();
  }

  if(contador2 < MaxTimer2) contador2++;

  else
  {
    contador2 = 1;
    rotina2();
  }
}

void rotina1()
{
  toggle_bit(PORTD, PD3);
}

void rotina2(void)
{

  pwm_set_duty_service(AD_pins[0], PWM_CHANNEL_2);
  sprintf(buffer, "%d\n", AD_pins[0]);
  uart_string_sending_service(buffer);
}

void read_key(void)
{
  static bool flag_bt = 0;
  static unsigned char button = 0;

  button = test_bit(PIND, PD5);

  if(button && !flag_bt)
  {
    flag_bt = 1;
  }

  else if (!button && flag_bt)
  {
    flag_bt = 0;
    toggle_bit(PORTD, PD4);
  }
}