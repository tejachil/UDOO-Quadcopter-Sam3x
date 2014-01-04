const int debugLED = 8;
static char inputBuf[100];
int bufCount;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  Serial1.begin(57600);
  pinMode(debugLED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly: 
  inputBuf[0] = 0;
  bufCount = 0;
  while(Serial1.available()){
    inputBuf[bufCount] = Serial1.read();
    ++bufCount;
  }
  if(bufCount > 0)  Serial.print(inputBuf);
  
  inputBuf[0] = 0;
  bufCount = 0;
  while(Serial.available()){
    inputBuf[bufCount] = Serial.read();
        Serial.print(inputBuf[bufCount]);
    ++bufCount;
    delay(1);
  }
  if(bufCount > 0){
    if(strstr(inputBuf, "CONTROL")){
      digitalWrite(debugLED, HIGH);
    }
    else{
      digitalWrite(debugLED, LOW);
      Serial1.print(inputBuf);
    }
  }
}
