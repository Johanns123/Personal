const int PINO_ENB = 5;

//declaracao dos pinos utilizados para controlar o sentido do motor
const int PINO_IN3 = 8;
const int PINO_IN4 = 6;

int i = 0; //declaracao da variavel para as rampas

const int TEMPO_ESPERA = 1000; //declaracao do intervalo de 1 segundo entre os sentidos de rotacao do motor

const int TEMPO_RAMPA = 30; //declaracao do intervalo de 30 ms para as rampas de aceleracao e desaceleracao

void setup() {

  //configuração dos pinos como saida 
  pinMode(PINO_ENB, OUTPUT);
  pinMode(PINO_IN3, OUTPUT);
  pinMode(PINO_IN4, OUTPUT);
  pinMode(0, OUTPUT);

  digitalWrite(0, HIGH);

  //inicia o codigo com os motores parados
  /*digitalWrite(PINO_IN3, LOW);
  digitalWrite(PINO_IN4, LOW);
  digitalWrite(PINO_ENB, LOW);*/

}

void loop() {

  //configura os motores para o sentido horario
  digitalWrite(PINO_IN3, LOW);
  digitalWrite(PINO_IN4, HIGH);
  digitalWrite(PINO_ENB, 100);
  delay(1000);
  //rampa de aceleracao
  /*for (i = 0; i < 256; i=i+10){ 
    analogWrite(PINO_ENB, i);
    delay(TEMPO_RAMPA); //intervalo para incrementar a variavel i
  }

  //rampa de desaceleracao
  for (i = 255; i >= 0; i=i-10){ 
    analogWrite(PINO_ENB, i);
    delay(TEMPO_RAMPA); //intervalo para incrementar a variavel i
  }

  delay(TEMPO_ESPERA); //intervalo de um segundo*/

  //configura os motores para o sentido anti-horario
  digitalWrite(PINO_IN3, HIGH);
  digitalWrite(PINO_IN4, LOW);
  digitalWrite(PINO_ENB, 100);
  delay(1000);

  //rampa de aceleracao
  /*for (i = 0; i < 256; i=i+10){ 
    analogWrite(PINO_ENB, i);
    delay(TEMPO_RAMPA); //intervalo para incrementar a variavel i
  }

  //rampa de desaceleracao
  for (i = 255; i >= 0; i=i-10){ 
    analogWrite(PINO_ENA, i); 
    analogWrite(PINO_ENB, i);
    delay(TEMPO_RAMPA); //intervalo para incrementar a variavel i
  }*/

  //delay(TEMPO_ESPERA); //intervalo de um segundo
  
}
