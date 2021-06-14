/*Passos para se desenvolver um PWM via software:
1-Defina a frequência e o período de trabalho desejado, por exemplo 1KHz (1ms);
2- Defina a resolução desejada para este PWM, isto é, em quantas partes o ciclo ativo será
dividido. Neste exemplo, usaremos 100, assim a resolução ficará entre 0 e 100%;
3- Configure um timer para que ele gere uma interrupção respeitando o seguinte cálculo:

Período_estouro = Período_PWM(s)/resolução = 0,001/100 = 10us

4- Na interrupção, incremente uma variável de contagem e ative o pino desejado quando ela
atingir o valor da resolução e desligue o pino quando a contagem atingir o ciclo ativo
desejado.
Exemplo 5: Criando um PWM de 1KHz via software:*/

/*Períodoestouro =PeríodoPWM/resolução=0,001/100 = 10us*/
//0,002/200 = 10us

#include <avr/io.h>
#include <avr/interrupt.h>
#define set_bit(y,bit) (y|=(1<<bit))
#define clr_bit(y,bit) (y&=~(1<<bit))
#define PWM1 PD1 //escolha do pino para o sinal PWM1
#define Resol_PWM1 200 //PWM1 com 200 passos temporais de resolução
/*O modificador "volatile" permite que estas variáveis possam ser utilizadas em qualquer
momento do programa sem ser modificada pela otimização do compilador*/

volatile unsigned int Passo_PWM1 = 0;
volatile unsigned int Ciclo_Ativo_PWM1;
//Interrupção do Timer que controla o PWM


//gere um período de PWM de 2ms com 500Hz
ISR(TIMER0_OVF_vect) {
    TCNT0 = 110; //Recarrega o timer para garantir a próxima interrupção com 10us
    Passo_PWM1++; //incremento do passo de tempo
    if (Passo_PWM1 == Resol_PWM1) {
        Passo_PWM1 = 0; //inicializa o contador
        set_bit(PORTD, PWM1); //na igualdade de comparação coloca o pino PWM1 em 1
    } else if (Passo_PWM1 == Ciclo_Ativo_PWM1)
        clr_bit(PORTD, PWM1); /*quando o contador atinge o valor do ciclo ativo do
PWM1, o pino vai a zero*/
}

int main() {
    DDRD = 0b11111111; //PORTD como saída
    PORTD = 0b00000001; //acende apenas o LED0
    TCCR0B = 0b00000001; //TC0 com prescaler de 1
    TIMSK0 = 0b00000001; //habilita a interrupção do TC0
    TCNT0 = 110; //Inicia a contagem em 96 para gerar a interrupção a cada 10us
    sei(); //habilita as interrupções
    Ciclo_Ativo_PWM1 = 100; //Determina o ciclo ativo para o PWM1 (0 - 200)
    while (1) {
    }
}