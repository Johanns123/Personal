// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

float Rx;
float Ry;
float Rz;

float RxGyro;
float RyGyro;
float RzGyro;

float RxAcc;
float RyAcc;
float RzAcc;

float RxEst;
float RyEst;
float RzEst;

float TaxaAxz;
float TaxaAyz;

float Axz = 0;
float Ayz = 0;

float t0 = millis();

float wGyro = 20;

float pitch;
float roll;

void angles(void){

  float delta_t = millis() - t0;
  t0 = millis();

  Axz = atan2(RxEst,RzEst);
  Ayz = atan2(RyEst,RzEst);
  
  Axz = Axz + TaxaAxz*delta_t/1000; 
  Ayz = Ayz + TaxaAyz*delta_t/1000;

  RxGyro = sin(Axz)/sqrt(1+cos(Axz)*cos(Axz) * tan(Ayz)*tan(Ayz));
  RyGyro = sin(Ayz)/sqrt(1+cos(Ayz)*cos(Ayz) * tan(Axz)*tan(Axz));
  RzGyro = sqrt(1-RxGyro*RxGyro - RyGyro*RyGyro);

  float R_abs = sqrt(Rx*Rx + Ry*Ry + Rz*Rz);
  float RxNorm = Rx/R_abs;
  float RyNorm = Ry/R_abs;
  float RzNorm = Rz/R_abs;

  RxEst = (RxNorm + RxGyro*wGyro)/(1+wGyro);
  RyEst = (RyNorm + RyGyro*wGyro)/(1+wGyro);
  RzEst = (RzNorm + RzGyro*wGyro)/(1+wGyro);

  float R = sqrt((RxEst*RxEst) + (RyEst*RyEst) + (RzEst*RzEst)); 
  RxEst /= R; 
  RyEst /= R; 
  RzEst /= R;

  pitch = atan2(RxEst,sqrt(RyEst*RyEst+RzEst*RzEst));
  roll = atan2(RyEst,sqrt(RxEst*RxEst + RzEst*RzEst));
 
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);
}

void loop() {

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  TaxaAxz = g.gyro.x;
  TaxaAyz = g.gyro.y;

  Rx = a.acceleration.x;
  Ry = a.acceleration.y;
  Rz = a.acceleration.z;

  angles();

  /* Print out the values */
  Serial.print(4.5*pitch*180/3.141592);
  Serial.print(",");
  Serial.println(4.5*roll*180/3.141592);


  
}
