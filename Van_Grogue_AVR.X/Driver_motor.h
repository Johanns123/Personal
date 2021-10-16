//Lado direito
#define AIN2 PD6
#define AIN1 PD5 // Quando em HIGH, roda direita anda para frente 

//Lado Esquerdo
#define BIN1 PD4 
#define BIN2 PD3 // Quando em HIGH, roda esquerda anda para frente

void frente() {

    set_bit(PORTD, AIN1); //frente direita
    clr_bit(PORTD, AIN2);
    set_bit(PORTD, BIN2); //frente esquerda
    clr_bit(PORTD, BIN1);
}

void tras() {
    clr_bit(PORTD, AIN1);
    set_bit(PORTD, AIN2); //tras direita
    clr_bit(PORTD, BIN2);
    set_bit(PORTD, BIN1); //tras esquerda

}

void motor_off() {
    clr_bit(PORTD, AIN1);
    clr_bit(PORTD, AIN2);
    clr_bit(PORTD, BIN2);
    clr_bit(PORTD, BIN1);
}

void freio() {
    /*frente();

    setDuty_1(200);
    setDuty_2(200);

    _delay_ms(500);

    tras();

    setDuty_1(150);
    setDuty_2(150);

    _delay_ms(5);

    frente();

    setDuty_1(0);
    setDuty_2(0);

    _delay_ms(2000);

    motor_off();

    _delay_ms(60000);*/
    
    motor_off();        //desliga os motores para deoxar o pr�prio atrito frear o rob�
    setDuty_1(0);
    setDuty_2(0);
}
