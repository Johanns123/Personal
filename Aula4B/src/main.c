#include<stdio.h>
#include<pico/stdlib.h>

#define LED 25

int main()
{
    stdio_init_all;     //inicializa o stdio do pico para utilizar a serial

    gpio_init(LED);     //define a IO usada
    gpio_set_dir(LED, GPIO_OUT);        //define como entrada ou saída
    puts("Hello World!");               //escreve uma string e pula uma linha

    while(1)    //loop infinito
    {
        gpio_put(LED, true);        //nível lógico alto na saída
        sleep_ms(500);              //delays de 500ms
        gpio_put(LED, false);       //nível lógico baixo na saída
        sleep_ms(500);

    }
}