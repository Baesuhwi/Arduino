/* 
 * HTTP Client GET Request
 * Copyright (c) 2018, circuits4you.com
 * All rights reserved.
 * https://circuits4you.com 
 * Connects to WiFi HotSpot. */
 
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
 
/* Set these to your desired credentials. */
const char *ssid = "iptime";  //ENTER YOUR WIFI SETTINGS
const char *password = "12345678";

//Web/Server address to read/write from 
const char *host = "39.127.7.68";   //https://circuits4you.com website or IP address of server
 
//=======================================================================
//                    Power on setup
//=======================================================================

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds 
const unsigned long postingInterval = 5000L; // delay between updates, in millisecondsunsigned long lastConnectionTime = 0; 
 
String rcvbuf; 
boolean getIsConnected = false; 


IPAddress hostIp(39, 127, 7, 68); 
int SERVER_PORT = 80; 
WiFiClient client; // Initialize the Ethernet client object//서버의 정보//

void setup() {
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
 
  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}
 
//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {
 Serial.println();
 delay(1000);
 get_dust();
 Serial.println();
 delay(1000);
}
//=======================================================================
//                    get_Dust
//=======================================================================
void get_dust() {
 int dust_value = 50;

 Serial.print("dust_value: ");
 Serial.println(dust_value);

 dust(dust_value);
}
//=======================================================================
//                    Dust
//=======================================================================
void dust(int dust_value) {
  Serial.println();
  delay(1000);

if (client.connect(hostIp, SERVER_PORT)) { 
Serial.println("Connecting..."); 

//POST Data Set// 
String jsondata = ""; 
      
StaticJsonBuffer<200> jsonBuffer; 
JsonObject& root = jsonBuffer.createObject(); 
root["dustvalue"] = dust_value;
//root["user_id"] = USERID; 
root.printTo(jsondata); //String으로 변환/ 
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
           lcdprintstr.concat("dust_value:"); 
           lcdprintstr.concat(dust_value);
            
          //Serial.println(lcdprintstr);
           
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
//=======================================================================
