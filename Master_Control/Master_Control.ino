#include <stdlib.h>     /* strtof */

const int debugLED = 8;
String inputBuf, imuBuf, tempBuf;

struct Dynamics {
  float yaw;
  float pitch;
  float roll;
  float throttle;
};

static Dynamics copterDynamics, inputDynamics;

float stringToFloat(String str);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200);
  pinMode(debugLED, OUTPUT);
  digitalWrite(debugLED, LOW);
  imuBuf = "";
}

void loop() {
  char c = 0;

  while(Serial1.available()){
    c = Serial1.read();
    if(c == '#')  imuBuf = "";
    imuBuf += c;
  }
  
  if(c == '\n'){
    imuBuf.trim();
    Serial.println(imuBuf);
    
    copterDynamics.yaw = stringToFloat(imuBuf.substring(imuBuf.indexOf('=')+1, imuBuf.indexOf(',')));
    tempBuf = imuBuf.substring(imuBuf.indexOf(',')+1);
    imuBuf = tempBuf;
    copterDynamics.pitch = stringToFloat(imuBuf);
    tempBuf = imuBuf.substring(imuBuf.indexOf(',')+1);
    imuBuf = tempBuf;
    copterDynamics.roll = stringToFloat(imuBuf);

    /*    
    Serial.print("\t\t");
    Serial.print(copterDynamics.yaw);
    Serial.print('\t');
    Serial.print(copterDynamics.pitch);
    Serial.print('\t');
    Serial.print(copterDynamics.roll);
    Serial.print('\t');    
    Serial.println("");
    */
  }

  inputBuf = "";  
  while(Serial.available()){
    c = Serial.read();
    inputBuf += c;
    delay(1);
  }
  if(!inputBuf.equals("")){
    if(inputBuf.indexOf("CONTROL DYNAMICS") != -1){
      digitalWrite(debugLED, HIGH);
      inputBuf.trim();
      
      copterDynamics.yaw = stringToFloat(inputBuf.substring(inputBuf.indexOf('=')+1, inputBuf.indexOf(',')));
      tempBuf = inputBuf.substring(inputBuf.indexOf(',')+1);
      inputBuf = tempBuf;
      copterDynamics.pitch = stringToFloat(inputBuf);
      tempBuf = inputBuf.substring(inputBuf.indexOf(',')+1);
      inputBuf = tempBuf;
      copterDynamics.roll = stringToFloat(inputBuf);
      if(inputBuf.indexOf("AUTO") == -1){
        tempBuf = inputBuf.substring(inputBuf.indexOf('::')+2);
        inputBuf = tempBuf;
        copterDynamics.throttle = stringToFloat(inputBuf);
      }
      else  copterDynamics.throttle = -9.0;
      
      /*    
      Serial.print("\t\t");
      Serial.print(copterDynamics.yaw);
      Serial.print('\t');
      Serial.print(copterDynamics.pitch);
      Serial.print('\t');
      Serial.print(copterDynamics.roll);
      Serial.print('\t');    
      Serial.print(copterDynamics.throttle);
      Serial.print('\t');    
      Serial.println("");
      */
    }
    else{
      Serial1.print(inputBuf);
    }
  }
}

float stringToFloat(String str){
  float ret = str.toInt();
  if(ret >= 0)  ret += (str.substring(str.indexOf('.') + 1).toInt())/100.0;
  else  ret -= (str.substring(str.indexOf('.') + 1).toInt())/100.0;
  return ret;
}
