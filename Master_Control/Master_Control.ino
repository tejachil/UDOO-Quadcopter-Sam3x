#include <stdlib.h>     /* strtof */
#include <Servo.h>

const int debugLED = 8;
String inputBuf, imuBuf, tempBuf;

enum MOTOR_LOCATIONS {FRONT_LEFT, FRONT_RIGHT, REAR_RIGHT, REAR_LEFT};

Servo brushlessMotor[4];

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
  
  //Servo serv;
//  serv.attach(8);
  Serial.println("ARMING MOTORS");
  //brushlessMotor[FRONT_LEFT].attach(4);
  /*brushlessMotor[FRONT_RIGHT].attach(5);
  brushlessMotor[REAR_RIGHT].attach(6);
  brushlessMotor[REAR_LEFT].attach(7);
  arm(brushlessMotor[FRONT_LEFT]);
  arm(brushlessMotor[FRONT_RIGHT]);  
  arm(brushlessMotor[REAR_RIGHT]);
  arm(brushlessMotor[REAR_LEFT]);*/
  delay(500);
  Serial.println("MOTORS ARMED");
}

void loop() {
  char c = 0;

  while(Serial1.available()){
//    int bytesAvailable = Serial1.available();
  //  for(int i = 0; i < bytesAvailable; ++i){
      c = Serial1.read();
      if(c == '#')  imuBuf = "";
      imuBuf += c;
    //}
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
/*
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
      else  copterDynamics.throttle = 0.0;
      
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
      /*digitalWrite(debugLED, LOW);
    }
    else{
      Serial1.print(inputBuf);
    }
  }
  
  /*setSpeed(brushlessMotor[FRONT_LEFT], (int)copterDynamics.throttle);
  setSpeed(brushlessMotor[FRONT_RIGHT], (int)copterDynamics.throttle);
  setSpeed(brushlessMotor[REAR_RIGHT], (int)copterDynamics.throttle);
  setSpeed(brushlessMotor[REAR_LEFT], (int)copterDynamics.throttle);*/
    //delay(500);
//    Serial.println("Hello");
  //delay(20);
}

float stringToFloat(String str){
  float ret = str.toInt();
  if(ret >= 0)  ret += (str.substring(str.indexOf('.') + 1).toInt())/100.0;
  else  ret -= (str.substring(str.indexOf('.') + 1).toInt())/100.0;
  return ret;
}

void arm(Servo motor){
  // arm the speed controller, modify as necessary for your ESC  
  setSpeed(motor, 0); 
  delay(1000); //delay 1 second,  some speed controllers may need longer
}

void setSpeed(Servo motor, int speed){
  // speed is from 0 to 100 where 0 is off and 100 is maximum speed
  //the following maps speed values of 0-100 to angles from 0-180,
  // some speed controllers may need different values, see the ESC instructions
  int angle = map(speed, 0, 100, 0, 180);
  motor.write(angle);    
}

