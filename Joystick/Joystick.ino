#define joystick1X A0  //eixo x
#define joystick1Y A1  //eixo y

void setup() {
  pinMode(joystick1X, INPUT);
  pinMode(joystick1Y, INPUT);
  Serial.begin(9600);
}

void loop() {
  int x, y;
  x = analogRead(joystick1X);
  y = analogRead(joystick1Y);
  Serial.print(x);
  Serial.print("   ,   ");
  Serial.println(y);
  delay(100);
}
