//소스 파일은 Relay_2로 첨부하였습니다.

int Relay = 2;
int Botton = 3;

void setup()
{
  pinMode(Relay, OUTPUT);
  pinMode(Botton, INPUT);
}

void loop()
{
  if (Serial.available()) {

    if (Serial.available() == 0) {
      digitalWrite(Relay, HIGH);
      delay(1000);
    }else if(Serial.available() == 1) {
      digitalWrite(Relay, LOW);
      delay(1000);  
    }
    
  }
}
