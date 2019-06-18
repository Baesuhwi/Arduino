#include <SoftwareSerial.h>
 
SoftwareSerial espSerial(2, 3); // RX, TX
 
void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);
}
 
void loop() { // run over and ov
  if (espSerial.available()) {
    Serial.write(espSerial.read());
  }
  if (Serial.available()) {
    espSerial.write(Serial.read());
  }
}
