#include <SoftwareSerial.h>
#define DEBUG true

SoftwareSerial esp8266(2, 3); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
// This means that you need to connect the TX line from the esp to the Arduino's pin 2
// and the RX line from the esp to the Arduino's pin 3

int motorSpeed;
int motorPin = 5;

void setup() {
  Serial.begin(9600);
  esp8266.begin(9600); // your esp's baud rate might be different
 
  Serial.println("AT Command..");
  sendData("AT+RST\r\n", 2000, DEBUG); // reset module
  sendData("AT+CIOBAUD?\r\n", 2000, DEBUG); // check baudrate (redundant)
  sendData("AT+CWMODE=3\r\n", 2000, DEBUG); // configure as access point (working mode: AP+STA)
  sendData("AT+CWLAP\r\n", 5000, DEBUG); // list available access points

  sendData("AT+CWJAP=\"iptime\",\"12345678\"\r\n", 7000, DEBUG); // join the access point
  sendData("AT+CIFSR\r\n", 3000, DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n", 2000, DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,7777\r\n", 2000, DEBUG); // turn on server on port 80
}

void loop() {
  delay(1000);
  Serial.print(".");
  if (esp8266.available()) { // check if the esp is sending a message
    Serial.println("Available()");
    if (esp8266.find("+IPD,")) {
      delay(1000); // wait for the serial buffer to fill up (read all the serial data)

      // read () 함수가 반환하기 때문에 48을 뺀다.
      int connectionId = esp8266.read() - 48;

      // the ASCII decimal value and 0 (the first decimal number) starts at 48
      esp8266.find("pin="); // advance cursor to "pin="
      // 첫번째 숫자를 얻는 과정. 핀 13에서 첫번째 숫자가 1이면 10을 곱하여 10을 얻음
      int pinNumber = (esp8266.read() - 48) * 10;
      pinNumber += (esp8266.read() - 48);

      Serial.print("pinNumber: ");
      Serial.println(pinNumber);

      if (pinNumber == 11) {
        Serial.println("켜짐");
        analogWrite(motorPin, 150);
        esp8266.flush();
      }
      if (pinNumber == 12) {
        Serial.println("꺼짐");
        analogWrite(motorPin, -10);
        esp8266.flush();
      }
      
      // make close command
      String closeCommand = "AT+CIPCLOSE="; 
      closeCommand+=connectionId; // append connection id
      closeCommand+="\r\n";
      sendData(closeCommand,1000,DEBUG); // close connection
    }
  }
}
/*
  Name: sendData
  Description: Function used to send data to ESP8266.
  Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
  Returns: The response from the esp8266 (if there is a reponse)
*/
String sendData(String command, const int timeout, boolean debug) {
  String response = "";
  esp8266.print(command); // send the read character to the esp8266
  long int time = millis();

  while ( (time + timeout) > millis()) {
    while (esp8266.available()) {
      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }

  if (debug) {
    Serial.print(response);
  }
  return response;
}
