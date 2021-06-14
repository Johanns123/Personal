#include<DHT.h>
#define DHTPIN 3
#define DHTTYPE DHT11 //especifica qual sensor- dht11 ou dht 22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
}

void loop() {
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature(false);
  Serial.print("Temperatur: ");
  Serial.print(temperatura);
  Serial.print(" *C");
  Serial.print("   ");
  Serial.print("Umidade");
  Serial.println(umidade);
  delay(5000); //intervalo do dht11 é de 1s
  

}
