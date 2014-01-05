const int debugLED = 8;
String inputBuf;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200);
  pinMode(debugLED, OUTPUT);
  digitalWrite(debugLED, LOW);
}

void loop() {
  // put your main code here, to run repeatedly: 
  inputBuf = "";
  while(Serial1.available()){
    char c = Serial1.read();
    inputBuf += c;
  }
  if(!inputBuf.equals("")){
    Serial.print(inputBuf);
    inputBuf = "";
  }
  
  while(Serial.available()){
    char c = Serial.read();
    inputBuf += c;
    delay(1);
  }
  if(!inputBuf.equals("")){
    if(inputBuf.indexOf("CONTROL") != -1){
      digitalWrite(debugLED, HIGH);
    }
    else{
      Serial1.print(inputBuf);
    }
  }
}
