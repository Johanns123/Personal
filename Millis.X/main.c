#include <avr/io.h>
#include <avr/interrupt.h>

#define cpl_bit(y,bit) (y^=(1<<bit))

/*tempo =65536 ? Prescaler/Fosc = 65536 ? 1024/16000000 = 4, 19s
 tempo = X_bit_timer * Prescaler/Fosc
 Valor inicial de contagem = 256 ? tempo_desejado?Fosc/Prescaler = 256 ? 0,01?16000000/1024 = 98,75 ? 99
 Valor inicial de contagem = X_bit_timer - tempo_desejado*Fosc/Prescaler*/


unsigned int millis = 0;

ISR(TIMER0_OVF_vect) {
    TCNT0 = 99; //Recarrega o Timer 0 para que a contagem seja 10ms novamente
    millis++; //Incrementa a variável millis a cada 10ms
}

int main() {
    //variáveis para controle de temporização
    unsigned int tempo_atual = 0;
    unsigned int tempo_passado_led0 = 0;
    unsigned int tempo_passado_led1 = 0;
    unsigned int tempo_passado_led2 = 0;
    DDRD = 0b11111111;
    PORTD = 0b00000000;
    TCCR0B = 0b00000101; //TC0 com prescaler de 1024
    TCNT0 = 99; //Inicia a contagem em 100 para, no final, gerar 10ms
    TIMSK0 = 0b00000001; //habilita a interrupção do TC0
    sei(); //habilita a chave de interrupção global
    while (1) {
        tempo_atual = millis; //Armazena o tempo decorrido atual (múltiplos de 10ms)
        if ((tempo_atual - tempo_passado_led0) >= 50) { //Verifica se passou 500ms
            cpl_bit(PORTD, 0); //Inverte o estado do LED0
            tempo_passado_led0 = tempo_atual; //Armazena o tempo atual
        }
        if ((tempo_atual - tempo_passado_led1) >= 100) { //Verifica se passou 1s
            cpl_bit(PORTD, 1);
            tempo_passado_led1 = tempo_atual;
        }
        if ((tempo_atual - tempo_passado_led2) >= 200) { //Verifica se passou 2s
            cpl_bit(PORTD, 2);
            tempo_passado_led2 = tempo_atual;
        }
    }
}