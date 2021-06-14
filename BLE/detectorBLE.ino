//Detector de Presença Bluetooth LE - Ricardo Merces - @r_merces

// --- WIFI ---
#include <WiFi.h>
const char* ssid     = "laboratorio";
const char* password = "laboratorio01";
WiFiClient esp32Client;

// --- MQTT ---
#include <PubSubClient.h>
PubSubClient client(esp32Client);
const char* mqtt_Broker = "192.168.1.4";
const char* mqtt_topico_controle = "laboratorio/office/iluminacao";
const char* mqtt_topico_monitoracao = "laboratorio/office/monitoracao";
const char* mqtt_ClientID = "esp32-01";
const char* mqtt_user = "curso";
const char* mqtt_pass = "laboratorio01";
boolean statusMonitor = false;

// --- Bluetooth ---
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
int scanTime = 5; // Em Segundos
int nivelRSSI = -60; // Ajustar conforme o ambiente!
String dispositivosAutorizados = "c2:6b:f6:62:fa:3f";
bool dispositivoPresente = false;


// --- Setup ---
void setup() {
  Serial.begin(115200);
  conectaWifi();
  client.setServer(mqtt_Broker, 1883);
  client.setCallback(monitoraTopico);
  Serial.println("Scanning...");
  BLEDevice::init("");
}

// -- Funções Auxiliares ---
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      String dispositivosEncontrados = advertisedDevice.getAddress().toString().c_str();
      if ( dispositivosEncontrados == dispositivosAutorizados  && advertisedDevice.getRSSI() > nivelRSSI) {
        Serial.println("Identificador DETECTADO!");
        Serial.print("RSSI: ");
        Serial.println(advertisedDevice.getRSSI());
        dispositivoPresente = true;
      }
      else{
        dispositivoPresente = false;
      }
    }
};

// --- Scan Bluetooth LE ---
void scanBLE(){
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
}

// --- Conecta no WIFI ---
void conectaWifi(){
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

// --- Conecta no Broker MQTT ---
void conectaMQTT() {
  while (!client.connected()) {
    client.connect(mqtt_ClientID, mqtt_user, mqtt_pass);
    client.subscribe(mqtt_topico_monitoracao);
  }
}

// --- Monitora o tópico laboratorio/office/monitoracao ---
void monitoraTopico(char* mqtt_topico_monitoracao, byte* payload, unsigned int length) {
  if ((char)payload[0] == '1') {
   statusMonitor = 1;
  }
  else {
    statusMonitor = 0;
  }
}

// --- Desabilita o scan ---
void desabilitaScan(){
  if (statusMonitor == 0 ){
    Serial.println("Scan ATIVO");
    scanBLE();
    publicaStatusnoTopico();
  }
  else {
    Serial.println("Scan DESLIGADO!");
    dispositivoPresente = true;
    publicaStatusnoTopico();
  }
}

// --- Publica no tópico Iluminacao (liga/desliga o dispositivo) ---
void publicaStatusnoTopico(){
  if(dispositivoPresente == 1){
    client.publish(mqtt_topico_controle, String("on").c_str(), true);
    Serial.println("Power ON");
  }
  else{
    client.publish(mqtt_topico_controle, String("off").c_str(), true);
    Serial.println("Power OFF");
    }
}

// --- LOOP ---
void loop() {
  if (!client.connected()) {
    conectaMQTT();
  }
  client.loop();
  desabilitaScan();
  delay(2000);
}
