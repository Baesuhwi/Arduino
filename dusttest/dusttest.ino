// 미세먼지 측정
//#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
//#include <ESP8266WebServer.h>
//#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
// 0x3F I2C 주소를 가지고 있는 16x2 LCD객체를 생성합니다.(I2C 주소는 LCD에 맞게 수정해야 합니다.)
LiquidCrystal_I2C lcd(0x27, 16, 2);

int measurePin = A3; //Connect dust sensor to Arduino A0 pin
int ledPower = 5; //Connect 3 led driver pins of dust sensor to Arduino D2
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
float voMeasured = 0.0;
float calcVoltage = 0.0;
float dustDensity = 0.0;
int dust_val = 0.0;
void setup() {
  Serial.begin(9600);
  pinMode(ledPower, OUTPUT);

  // I2C LCD를 초기화 합니다..
  lcd.init();
  // I2C LCD의 백라이트를 켜줍니다.
  lcd.backlight();
}
void loop() {
  digitalWrite(ledPower, LOW); // power on the LED
  delayMicroseconds(samplingTime);
  voMeasured = analogRead(measurePin); // read the dust value
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower, HIGH); // turn the LED off
  delayMicroseconds(sleepTime);
  calcVoltage = voMeasured * (5.0 / 1024.0);
  dustDensity = 0.17 * calcVoltage - 0.1;
  dust_val = (0.17 * calcVoltage - 0.1) * 1000;
  Serial.print("Raw Signal Value (0-1023): ");
  Serial.print(voMeasured);
  Serial.print(" - Voltage: ");
  Serial.print(calcVoltage);
  Serial.print(" - Dust Density: ");
  Serial.println(dustDensity); // unit: mg/m3
  Serial.print("// dust_val:");
  Serial.println(dust_val);

  lcd.setCursor(0, 1);
  lcd.print("dust_val: ");
  lcd.print(dust_val);

  delay(1000);
  lcd.clear();
  //int dust_value = 50;

  //Serial.print("dust_value: ");
  //Serial.println(dust_value);
  // dust(dust_val);
}
