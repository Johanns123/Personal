#include <Servo.h>
#define ANGULO_INICIAL_MOTOR 90

// --- Mapeamento do Joystick ---

#define joystick1X A0  //eixo x
#define joystick1Y A1  //eixo y

// --- Mapeamento do Servo ---
Servo motor;


void setup() {
  motor.attach(9);
  pinMode(joystick1X, INPUT);
  pinMode(joystick1Y, INPUT);
  motor.write(ANGULO_INICIAL_MOTOR);
}

void loop() {
  int posicao_x = analogRead(joystick1X);
  int posicao_y = analogRead(joystick1Y);
  posicao_x = map(posicao_x, 4, 1017, 0, 180);
  motor.write(posicao_x);
  delay(100);

}
