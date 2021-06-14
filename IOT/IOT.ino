

//---WI-FI---//
#include <ESP8266WiFi.h>
const char* ssid = "........";
const char* password = "........";
WiFiClient espClient;


//--- MQTT ---//
#include <PubSubClient.h>
const char* mqtt_Broker = "iot.eclips.org";
PubSubClient client(espClient);
const char* topicoTemperatura = "labJohann/temperatura";
const char* topicoUmidade = "labJohann/umidade";
const char* mqtt_ClientID = "Termometro_Johann01";

//--- DHT---//
#include <DHT.h>
#define DHTPIN 3
#define DHTTYPE DHT11 //especifica qual sensor- dht11 ou dht 22
DHT dht(DHTPIN, DHTTYPE);
int umidade;
int temperatura;

//--- Display---//
#include <Adafruit_SSD1306.h>
#include <splash.h>
#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);

void setup() {
  //Serial.begin(115200);
  configurarDisplay();
  conectar_WiFi();
  client.setServer(mqtt_Broker, 1883);
}

void loop() {
  if(!client.connected()){
    reconectarMQTT();
  }
  medir_temperatura_e_umidade();
  mostrar_temperatura_umidade();
  publicar_Temperatur_e_Umidade();
}

void conectar_WiFi(){
  delay(10);
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("Conectando");
  display.display();
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    display.print(".");
    display.display();
  }
  
}

void configurarDisplay(){
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);;  //0x3C é o endereço do display. Para saber existe um código no site da arduino para saber o endereço por tecnologia I2C
  display.setTextColor(WHITE);
  display.clearDisplay();
}

void reconectarMQTT(){
  while(!client.connected()){
    client.connect("Termometro_Johann01");
    
  }
}

void medir_temperatura_e_umidade(){
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature(false);
  Serial.print("Temperatur: ");
  Serial.print(temperatura);
  Serial.print(" *C");
  Serial.print("   ");
  Serial.print("Umidade");
  Serial.println(umidade);
  delay(5000); //intervalo do dht11 é de 1s
}

void publicar_Temperatur_e_Umidade(){
  client.publish(topicoTemperatura, String(temperatura).c_str(), true);
  client.publish(topicoUmidade, String(umidade).c_str(), true);

  
}

void mostrar_temperatura_umidade(){
  Mostrar_mensagem_no_display("Temperatura", (temperatura), " C");
  Mostrar_mensagem_no_display("Umidade", (umidade), " %");
}


void Mostrar_mensagem_no_display(const char* texto1, int medicao, const char* texto2){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print(texto1);
  display.setTextSize(5);
  display.setCursor(20,20);
  display.print(medicao);
  display.setTextSize(2);
  display.print(texto2);
  display.display();
  delay(2000);
}
