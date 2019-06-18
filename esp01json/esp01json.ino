#include <ArduinoJson.h>
#include "WiFiEsp.h"

//=======================================================================
//                    dust setup
//=======================================================================
int measurePin = A5; //Connect dust sensor to Arduino A0 pin
int ledPower = 5; //Connect 3 led driver pins of dust sensor to Arduino D2
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
float voMeasured = 0.0;
float calcVoltage = 0.0;
float dustDensity = 0.0;
float dust_val = 0.0;

StaticJsonBuffer<200> jsonBuffer;  //json변환에 필요한 버퍼 생성
JsonObject& root = jsonBuffer.createObject(); //json형식을 만들어 낼수있는 객체 생성

//=======================================================================
//                    wifi setup
//=======================================================================
unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds 
const unsigned long postingInterval = 5000L; // delay between updates, in millisecondsunsigned long lastConnectionTime = 0; 
 
String rcvbuf; 
boolean getIsConnected = false;

//Web/Server address to read/write from 
const char *host = "39.127.7.68";

IPAddress hostIp(39, 127, 7, 68); 
int SERVER_PORT = 80; 

#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(2, 3); // RX, TX
#endif

char ssid[] = "iptime";            // your network SSID (name)
char pass[] = "12345678";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char server[] = "39.127.7.68";

// Initialize the Ethernet client object
WiFiEspClient client;

//=======================================================================
//                    Power on setup
//=======================================================================
void setup()
{
  pinMode(ledPower,OUTPUT);
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
 for(int i = 0; i < 5; i++){
Serial.print("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
  get_dust();
  delay(1000);
Serial.print("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
 }
 dust();
 
 Serial.println();
 //delay(1000);
}

//=======================================================================
//                    get_Dust(measure)
//=======================================================================
void get_dust() {
Serial.println();
digitalWrite(ledPower,LOW); // power on the LED
delayMicroseconds(samplingTime);
voMeasured = analogRead(measurePin); // read the dust value
delayMicroseconds(deltaTime);
digitalWrite(ledPower,HIGH); // turn the LED off
delayMicroseconds(sleepTime);
// 0 - 5V mapped to 0 - 1023 integer values
// recover voltage
calcVoltage = voMeasured * (5.0 / 1024.0);
// linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
// Chris Nafis (c) 2012
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
root["dust_val"] = dust_val; // key=["dust_val"], value=dust_val
//=======================================================================
//                    Dust
//=======================================================================
void dust() {
  Serial.println();
//  Serial.print("dust_val:");
//  Serial.println(dust_val);

// 연결됐을때, 밑에 명령들 실행
if (client.connect(server, 80)) { 
//if (client.connect("39.127.7.68", "80")) { 
Serial.println("Connecting..."); 

//POST Data Set// 
String jsondata = ""; 
      
//StaticJsonBuffer<200> jsonBuffer;  //json변환에 필요한 버퍼 생성
//JsonObject& root = jsonBuffer.createObject(); //json형식을 만들어 낼수있는 객체 생성

//root["dust_val"] = dust_val; // key=["dust_val"], value=dust_val

root.printTo(jsondata); // printTo => String변수로 변환
Serial.print("jsondata: ");
Serial.println(jsondata);

// send the HTTP POST request 
client.print(F("POST /test")); 
client.print(F(" HTTP/1.1\r\n")); 
client.print(F("Cache-Control: no-cache\r\n")); 
client.print(F("Host: 39.127.7.68:80\r\n")); 
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

    //서버로 부터 값을 받는다.// 
int headcount = 0; 
   
//No Socket available문제 해결// 
  while (client.connected()) {
    if (client.available()) {
    char c = client.read();
    Serial.print(c);
 
 
  //String에 담아서 원하는 부분만 파싱하도록 함// 
    rcvbuf += c; 
        
    if(c == '\r'){ 
    headcount ++; //해더 정보는 생략하기 위해서 설정// 
  //Serial.println(rcvbuf);
  
    if(headcount != 13){ 
    rcvbuf = ""; 
    } 
    } 
      //데이터 영역/ 
      if(headcount == 13){ 
         //JSON파싱// 
         StaticJsonBuffer<200> jsonBuffer; 
         JsonObject& root = jsonBuffer.parseObject(rcvbuf); 
         String result = root["result"]; 
          
         Serial.println(result); 
 
         if(result.equals("success!!")){
           String lcdprintstr = ""; 
           lcdprintstr.concat("dust_val:");
           lcdprintstr.concat(dust_val);
           lcdprintstr.concat("성공입니다!!");
            
          Serial.println(lcdprintstr);
           
           lcdprintstr = ""; 
         } else{
            Serial.println("success!! 오류");
         }
    
         client.stop(); //클라이언트 접속 해제// 
          
         rcvbuf = ""; 
       } 
     } 
   }
   client.flush(); 
   client.stop(); 
}
