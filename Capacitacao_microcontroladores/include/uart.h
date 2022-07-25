#include <avr/io.h>
void uart_setup(unsigned int size);
void uart_send_char(unsigned char ch);
void uart_caractere_sending_service(unsigned char ch);
void uart_string_sending_service(char *s);
void uart_hex_sending_service(unsigned char ch);
