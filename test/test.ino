#include "WiFiEsp.h"

// Emulate Serial1 on pins 6/7 if not present

#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(2, 3); // RX, TX
#endif
char ssid[] = "YJU-Guest";            // your network SSID (name)
char pass[] = "";        // your network password
int status = WL_IDLE_STATUS;

WiFiEspClient client;
char server[] = "39.127.7.68";
///WiFiEspServer server(81);

void setup() { 
  Serial.begin(9600);

  pinMode(5, OUTPUT);

  Serial.begin(9600);   // initialize serial for debugging
  Serial1.begin(9600);    // initialize serial for ESP module
  WiFi.init(&Serial1);    // initialize ESP module

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }  

}
void loop() {
  if(client.connect(server, 81)){
    Serial.println("#abc#");
  
  if(Serial.available()){
    char command = Serial.read();
    Serial.print("command: ");
    Serial.println(command);
    if(command == '0'){
      Serial.println("a");
//      digitalWrite(5, HIGH);
      Serial.println("LED ON");
    }
    else if(command == '1'){
      Serial.println("b");
//      digitalWrite(5, LOW);
      Serial.println("LED OFF");
    }
    else{
      Serial.println("c");
      Serial.print("Wrong command: ");
      Serial.println(command);
    }
  }
  }
  
    delay(5000); // 1초마다 전송
}
