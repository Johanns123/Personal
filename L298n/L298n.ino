int IN1 = 2 ;
int IN2 = 4 ;
int velocidadeA = 3;

//motor_B
int IN3 = 8 ;
int IN4 = 6 ;
int velocidadeB = 5;

//variavel auxiliar
int velocidade = 0;

//Inicializa Pinos
void setup(){
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(velocidadeA,OUTPUT);
  pinMode(velocidadeB,OUTPUT);
}

void loop(){
  /*Exemplo de variacao de velocidade no motor B*/
  //Sentido 2
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  //analogWrite(velocidadeB, 255);



  
  //velocidade de 0 a 255
 while (velocidade < 255){
  analogWrite(velocidadeB,velocidade);
  velocidade = velocidade + 10;
  delay(50);
  }
  //velocidade de 255 a 0
  while (velocidade > 0){
  analogWrite(velocidadeB,velocidade);
  velocidade = velocidade - 10;
  delay(50);
}
  digitalWrite(IN3,0);
  digitalWrite(IN4,1);
  while (velocidade < 255){
  analogWrite(velocidadeB,velocidade);
  velocidade = velocidade + 10;
  delay(50);
  }
  //velocidade de 255 a 0
  while (velocidade > 0){
  analogWrite(velocidadeB,velocidade);
  velocidade = velocidade - 10;
  delay(50);
}

}
