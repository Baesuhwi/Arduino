#include <ArduinoJson.h>

#include "WiFiEsp.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
//=======================================================================
//                    dust setup
//=======================================================================
int measurePin = A3; //Connect dust sensor to Arduino A0 pin
int ledPower = 5; //Connect 3 led driver pins of dust sensor to Arduino D2
int samplingTime = 280;
int deltaTime = 40; //아두이노에서 analogRead를 위해 필요한 시간을 주는 부분으로 ADC(Analog to Digital Convert) 변환에 필요한 시간
int sleepTime = 9680; //발광부에서 작동되는 LED를 잠시 꺼두고 센서를 잠시 Idle(유휴) 상태로 두는 시간
float voMeasured = 0.0;
float calcVoltage = 0.0;
float dustDensity = 0.0;
float dust_val = 0.0;
String iotId = "나의 공기측정1";

//=======================================================================
//                    wifi setup
//=======================================================================
unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 5000L; // delay between updates, in millisecondsunsigned long lastConnectionTime = 0;
const int WIFI_Initial_time = 2000;

String rcvbuf;
boolean getIsConnected = false;

//Web/Server address to read/write from

IPAddress hostIp(13,209,87,1);
int SERVER_PORT = 80;

#ifndef HAVE_HWSERIAL188
#include "SoftwareSerial.h"
SoftwareSerial Serial1(2, 3); // RX, TX
#endif

char ssid[] = "iptime";            // your network SSID (name)
char pass[] = "12345678";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char server[] = "13.209.87.1";

// Initialize the Ethernet client object
WiFiEspClient client;

//=======================================================================
//                    Power on setup
//=======================================================================
void setup()
{
  lcd.init();
  lcd.backlight();

  pinMode(ledPower, OUTPUT);
  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
  Serial1.begin(9600);

  // initialize ESP module
  WiFi.init(&Serial1);
  // 와이파이 존재 유무
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // 와이파이 연결 시도
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  // 연결 됐을때 출력문
  Serial.println("You're connected to the network");
  Serial.println();
  Serial.println("Starting connection to server...");
}
//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop()
{
  get_dust(); // 측정 메소드
  Serial.println("===========================================");
  lcd.clear();
  Serial.println();
}

//=======================================================================
//                    get_Dust(measure)
//=======================================================================
void get_dust() {
  Serial.println();
  digitalWrite(ledPower, LOW); // power on the LED
  delayMicroseconds(samplingTime);
  voMeasured = analogRead(measurePin); // read the dust value
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower, HIGH); // turn the LED off
  delayMicroseconds(sleepTime);
  // 0 - 5V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (5.0 / 1024.0);
  // linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
  // Chris Nafis (c) 2012
  dustDensity = 0.17 * calcVoltage - 0.1;
  dust_val = ((0.17 * calcVoltage - 0.1) * 1000) + 20;

  if (dust_val < 0) {
    dust_val = 20;
  }
  Serial.print("Raw Signal Value (0-1023): ");
  Serial.print(voMeasured);

  Serial.print(" - Voltage: ");
  Serial.print(calcVoltage);

  Serial.print(" - Dust Density: ");
  Serial.println(dustDensity); // unit: mg/m3

  Serial.print("// dust_val:");
  Serial.println(dust_val); // 미세먼지 측정 값

  lcd.setCursor(0, 0);
  lcd.print("Dust_val: ");
  lcd.print(dust_val);
  //=======================================================================
  Serial.println("iotId: " + iotId);
  dust(dust_val, iotId);
}

//=======================================================================
//                    Dust
//=======================================================================
void dust(int dust, String iotId) {
  Serial.println("센서 측정값 전달 대기중...");

  // 연결됐을때, 밑에 명령들 실행
  if (client.connect(server, 80)) {
    Serial.println("Connecting...");

    //POST Data Set//
    String jsondata = "";

    StaticJsonBuffer<200> jsonBuffer;  //json변환에 필요한 버퍼 생성
    JsonObject& root = jsonBuffer.createObject(); //json형식을 만들어 낼수있는 객체 생성
    root["dust_val"] = dust; // key=["dust_val"], value=dust_val
    root["iotId"] = iotId;

    root.printTo(jsondata); // printTo => String변수로 변환
    Serial.print("jsondata: ");
    Serial.println(jsondata);

    // send the HTTP POST request
    client.print(F("POST /dustData"));
    client.print(F(" HTTP/1.1\r\n"));
    client.print(F("Cache-Control: no-cache\r\n"));
    client.print(F("Host: 13.209.87.1:80\r\n"));
    client.print(F("User-Agent: Arduino\r\n"));
    client.print(F("Content-Type: application/json;charset=UTF-8\r\n"));
    client.print(F("Content-Length: "));
    client.println(jsondata.length());
    client.println();
    client.println(jsondata);
    client.print(F("\r\n\r\n"));

    // note the time that the connection was made
    lastConnectionTime = millis();
    getIsConnected = true;
  }

  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
    getIsConnected = false;
  }
  client.flush();
  client.stop();
}
