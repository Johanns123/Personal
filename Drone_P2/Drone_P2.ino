#define BLYNK_PRINT Serial

#define PWM1 7
#define PWM2 8
#define PWM3 9
#define PWM4 10

#define motor1 2
#define motor2 3
#define motor3 4
#define motor4 5


//MPU 6050 and complementary filter libraries
#include <MPU6050_tockn.h>
#include <Wire.h>

//Blynk libraries
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

MPU6050 mpu6050(Wire);

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "YourAuthToken";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";

//inicializa variáveis de comando
int botao1;   //Botao para ligar o drone
int botao2;    //botão para uma implementação futura
int targetYaw;    //variável para o yaw -50 - 50 graus
int throttle;     //variável para o throttle 0 - 100%
int targetRoll;   //variável para o Roll -45 - 45 graus
int targetPitch;  //variável para o pitch -45 - 45 graus   
/*Yaw, rolle pitch são os eixos de rotação do drone
 enquanto o throttle é a variável de subida.*/

BLYNK_WRITE(V0)
{
  botao1 = param.asInt(); 
  
}

BLYNK_WRITE(V1)
{
  botao2 = param.asInt(); 
  
}

BLYNK_WRITE(V2)
{
  targetYaw += param.asInt(); //chevar ma melhor aproximação para o Yaw
  
}

BLYNK_WRITE(V3)
{
  throttle = param.asInt(); 
  
}

BLYNK_WRITE(V4)
{
  targetRoll = param.asInt(); 
  
}

BLYNK_WRITE(V5)
{
  targetPitch = param.asInt(); 
  
}

int leitura_dos_comandos [] = {botao1, botao2, targetYaw, throttle, targetRoll, targetPitch};

//inicializa as variáveis do IMU
float estimatedPitch, estimatedRoll, estimatedYaw;


//variáveis globais do PID
int u_pitch, u_roll, u_yaw;
float delta_T;
float last_time;
float Pitch_erro_acumulado, Roll_erro_acumulado, Yaw_erro_acumulado;
float Pitch_ultimo_erro, Roll_ultimo_erro, Yaw_ultimo_erro;
float P = 1;
float I = 0.02;
float D = 0.02;


int PitchPID(float estimated, int target){
    float erro = target - estimated;
    float erro_dot = (erro - Pitch_ultimo_erro)/ delta_T;  //derivação do erro
    Pitch_erro_acumulado += erro*delta_T;
    int u = P*erro + I*Pitch_erro_acumulado + D*erro_dot;
    Pitch_ultimo_erro = erro;
    return u; //retorna os valores após o PID
}

int RollPID(float estimated, int target){
    float erro = target - estimated;
    float erro_dot = (erro - Roll_ultimo_erro)/ delta_T;  //derivação do erro
    Roll_erro_acumulado += erro*delta_T;
    int u = P*erro + I*Roll_erro_acumulado + D*erro_dot;
    Roll_ultimo_erro = erro;
    return u; //retorna os valores após o PID

}

int YawPID(float estimated, int target){
    float erro = target - estimated;
    float erro_dot = (erro - Yaw_ultimo_erro)/ delta_T;  //derivação do erro
    Yaw_erro_acumulado += erro*delta_T;
    int u = P*erro + I*Yaw_erro_acumulado + D*erro_dot;
    Yaw_ultimo_erro = erro;
    return u; //retorna os valores após o PID
}

//variáveis do motor e funções
int f,t,d,e;    //frente, trás, direita e esquerda

void calculo(){
  f = throttle - u_pitch + u_yaw;
  t = throttle + u_pitch + u_yaw;
  d = throttle - u_roll - u_yaw;
  e = throttle + u_roll - u_yaw;
  
}

void motores(){

  if(botao1 == 1){
    ledcWrite(PWM1, f);
    ledcWrite(PWM2, t);
    ledcWrite(PWM3, d);
    ledcWrite(PWM4, e);
  }
}

void setup()
{ 
  pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(PWM3, OUTPUT);
  pinMode(PWM4, OUTPUT);
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(motor3, OUTPUT);
  pinMode(motor4, OUTPUT);  
  // Debug console
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
}

void loop()
{
  //parte dos comandos
  Blynk.run();
 

  //Parte do IMU
  mpu6050.update();
  estimatedPitch = mpu6050.getAngleX()*180/3.141592;   //essas variáveis vão depender de como o MPU6050 estará posicionado
  estimatedRoll = mpu6050.getAngleY()*180/3.141592;
  estimatedYaw = mpu6050.getAngleZ()*180/3.141592;

  //Parte do PID
  delta_T = (millis() - last_time)/1000;
  last_time = millis();
  u_pitch = PitchPID(estimatedPitch, targetPitch);
  u_roll = RollPID(estimatedRoll, targetRoll);
  u_yaw = YawPID(estimatedYaw, targetYaw);


  //parte dos motores
  calculo();
  motores();

  //leitura dos comandos
  Serial.print("Valor dos comandos:");
  for(int i = 0; i < 6; i++){
    Serial.print(leitura_dos_comandos[i]);
    Serial.print(',');
    } 
  Serial.println("");

  Serial.print("Valor estimado:");
  Serial.print(estimatedPitch);
  Serial.print(',');
  Serial.print(estimatedRoll);
  Serial.print(',');
  Serial.println(estimatedYaw);

  int U [] = {u_pitch, u_roll, u_yaw};

  Serial.print("Valor do PID:");
  for(int i = 0; i < 6; i++){
    Serial.print(U[i]);
    Serial.print(',');
    } 
  Serial.println("");

}
