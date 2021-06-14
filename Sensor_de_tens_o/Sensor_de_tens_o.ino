#define Sensor_tensao A0
#define LED 13
#define LED_2 12

int Valor = 0;
float Tensao;
void setup() {
  pinMode(Sensor_tensao, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(LED_2, OUTPUT);
}

void loop() {
  Valor = analogRead(Sensor_tensao);
  Tensao = 5 * ((float)Valor/1023);
  if(Tensao >= 1.32){
    digitalWrite(LED,1);
    digitalWrite(LED_2,1);
    delay(1000);
    digitalWrite(LED,0);
    digitalWrite(LED_2,0);
    delay(300);
  }

}
