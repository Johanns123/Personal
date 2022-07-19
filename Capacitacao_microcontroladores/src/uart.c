#include "uart.h"

void uart_setup(unsigned int size)
{
  UBRR0H = (unsigned char)(size>>8);  //2 bits mais significativos de ubrr
  UBRR0L = (unsigned char)size;       //8 bits menos significativos de ubrr
  
  //Habilita a interrup��o de recep��o e os pinos TX e RX
  UCSR0B =  (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);

//Configura a UART com 8 bits de dados
  UCSR0C =  (1<<UCSZ01) | (1<<UCSZ00); 
}

void uart_send_char(unsigned char ch) 
{
  UDR0 = ch;
  //Aguarda o buffer ser desocupado
  while (!(UCSR0A & (1<<UDRE0)));
}

void uart_caractere_sending_service(unsigned char ch) 
{
  uart_send_char(ch);
}

void uart_string_sending_service(char *s)
{
  unsigned int i=0;
  while (s[i] != '\x0') 
  {
      uart_send_char(s[i++]);
  };
}

void uart_hex_sending_service(unsigned char ch)
{   
  unsigned char i,temp;
  for (i=0; i<2; i++)
  {
      temp = (ch & 0xF0)>>4;
      if ( temp <= 9)
          uart_send_char('0' + temp);
      else
          uart_send_char('A' + temp - 10);
      ch = ch << 4;    
    }
}