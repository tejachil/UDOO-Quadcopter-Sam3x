#include <stdlib.h>     /* strtof */
#include <Servo.h>

const int debugLED = 8;
volatile boolean stateLED;
String inputBuf, imuBuf, tempBuf;

enum MOTOR_LOCATIONS {FRONT_LEFT, FRONT_RIGHT, REAR_RIGHT, REAR_LEFT};

Servo brushlessMotor[4];

int motorSpeeds[4];

struct Dynamics {
  float yaw;
  float pitch;
  float roll;
  int thrust;
};

Dynamics copterDynamics, inputDynamics;

const float Kp = 0.75;

/*volatile boolean l;
//TC1 ch 0
void TC3_Handler()
{
        TC_GetStatus(TC1, 0);
        digitalWrite(8, l = !l);
      
}

void startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency) {
        pmc_set_writeprotect(false);
        pmc_enable_periph_clk((uint32_t)irq);
        TC_Configure(tc, channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4);
        uint32_t rc = VARIANT_MCK/128/frequency; //128 because we selected TIMER_CLOCK4 above
        TC_SetRA(tc, channel, rc/2); //50% high, 50% low
        TC_SetRC(tc, channel, rc);
        TC_Start(tc, channel);
        tc->TC_CHANNEL[channel].TC_IER=TC_IER_CPCS;
        tc->TC_CHANNEL[channel].TC_IDR=~TC_IER_CPCS;
        NVIC_EnableIRQ(irq);
}*/

float stringToFloat(String str){
  float ret = str.toInt();
  String temp = str.substring(str.indexOf('.') + 1);
  float dec = temp.toInt();
  dec = dec/100.0;
  if(ret >= 0)  ret += dec;
  else  ret -= dec;
  return ret;
}

void arm(){
  for(int i = 0; i < 4; ++i){
    brushlessMotor[i].attach(4+i);
    brushlessMotor[i].writeMicroseconds(1000);
  }
  delay(6000);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200);
  pinMode(debugLED, OUTPUT);
  digitalWrite(debugLED, LOW);
  imuBuf = "";
  
  //Servo serv;
  //serv.attach(8);
  Serial.println("ARMING MOTORS");
  
  arm();
  
  for(int speed = 1000; speed <= 1070; ++speed){
    brushlessMotor[FRONT_LEFT].writeMicroseconds(speed);
    brushlessMotor[FRONT_RIGHT].writeMicroseconds(speed);
    brushlessMotor[REAR_RIGHT].writeMicroseconds(speed);
    brushlessMotor[REAR_LEFT].writeMicroseconds(speed);
  }
  
  motorSpeeds[FRONT_LEFT] = 1070;
  motorSpeeds[FRONT_RIGHT] = 1070;
  motorSpeeds[REAR_RIGHT] = 1070;
  motorSpeeds[REAR_LEFT] = 1070;
  copterDynamics.thrust = 1070;
  inputDynamics.thrust = 1070;
  

  Serial.println("MOTORS ARMED");
  //startTimer(TC1, 0, TC3_IRQn, 20); //TC1 channel 0, the IRQ for that channel and the desired frequency
}

void loop() {
  char c = 0;

  while(Serial1.available()){
    c = Serial1.read();
    if(c == '#')  imuBuf = "";
    imuBuf += c;
  }
  Serial1.flush();
  if(c == '\n'){
    imuBuf.trim();
    //Serial.println(imuBuf);
    
    tempBuf = imuBuf.substring(imuBuf.indexOf('=')+1);
    imuBuf = String(tempBuf);
    copterDynamics.yaw = stringToFloat(imuBuf);
    tempBuf = imuBuf.substring(imuBuf.indexOf(',')+1);
    imuBuf = String(tempBuf);
    copterDynamics.pitch = stringToFloat(imuBuf);
    tempBuf = imuBuf.substring(imuBuf.indexOf(',')+1);
    imuBuf = String(tempBuf);
    copterDynamics.roll = stringToFloat(imuBuf);

    /*Serial.print("\t\t");
    Serial.print(copterDynamics.yaw);
    Serial.print("\t");
    Serial.print(copterDynamics.pitch);
    Serial.print("\t");
    Serial.print(copterDynamics.roll);  
    Serial.println("");*/
    
  }

  char d = 0;
  while(Serial.available()){
    d = Serial.read();
    if(d == '$')  inputBuf = "";
    inputBuf += d;
  }
  Serial.flush();
  if(d == '\n'){
    if(inputBuf.startsWith("$CONTROL DYNAMICS")){
      digitalWrite(debugLED, stateLED = !stateLED);
      inputBuf.trim();
      
      tempBuf = inputBuf.substring(inputBuf.indexOf('=')+1);
      inputBuf = String(tempBuf);
      inputDynamics.yaw = stringToFloat(inputBuf);
      tempBuf = inputBuf.substring(inputBuf.indexOf(',')+1);
      inputBuf = String(tempBuf);
      inputDynamics.pitch = stringToFloat(inputBuf);
      tempBuf = inputBuf.substring(inputBuf.indexOf(',')+1);
      inputBuf = String(tempBuf);
      inputDynamics.roll = stringToFloat(inputBuf);

      tempBuf = inputBuf.substring(inputBuf.indexOf('::')+2);
      inputBuf = tempBuf;
      inputDynamics.thrust = inputBuf.toInt();
      
      //speed = inputDynamics.throttle * 10;
      //speed = speed + 1000;
      //Serial.println(inputDynamics.thrust);      
      
      // TESTING COMMAND: $CONTROL DYNAMICS #YPRT=12.32,12.33,12.43::1300.00
      
      //if(inputBuf.indexOf("AUTO") != -1)  copterDynamics.throttle = 0.0;
      /*else{
        tempBuf = inputBuf.substring(inputBuf.indexOf('::')+2);
        inputBuf = tempBuf;
        copterDynamics.throttle = stringToFloat(inputBuf);
      }*/
      
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
      //digitalWrite(debugLED, LOW);
    }
    /*else{
      Serial1.print(inputBuf);
    }*/
//    digitalWrite(debugLED, LOW);
  }
      //int deltaThrust = 0; //inputDynamics.thrust - copterDynamics.thrust;
      
      int delta;
      
      delta = (int)(0 - Kp*copterDynamics.pitch - Kp*copterDynamics.roll);
      motorSpeeds[FRONT_LEFT] = inputDynamics.thrust;//+= deltaThrust;
      motorSpeeds[FRONT_LEFT] += delta;
//      Serial.println(motorSpeeds[FRONT_LEFT]);
      brushlessMotor[FRONT_LEFT].writeMicroseconds(motorSpeeds[FRONT_LEFT]);

      delta = (int)(0 - Kp*copterDynamics.pitch + Kp*copterDynamics.roll);
      motorSpeeds[FRONT_RIGHT] = inputDynamics.thrust;//+= deltaThrust;
      motorSpeeds[FRONT_RIGHT] += delta;
      brushlessMotor[FRONT_RIGHT].writeMicroseconds(motorSpeeds[FRONT_RIGHT]);
//      Serial.print(motorSpeeds[FRONT_RIGHT]);

      delta = (int)(0 + Kp*copterDynamics.pitch + Kp*copterDynamics.roll);
      motorSpeeds[REAR_RIGHT] = inputDynamics.thrust;//+= deltaThrust;
      motorSpeeds[REAR_RIGHT] += delta;
      brushlessMotor[REAR_RIGHT].writeMicroseconds(motorSpeeds[REAR_RIGHT]);
  //    Serial.print(motorSpeeds[REAR_RIGHT]);

      delta = (int)(0 + Kp*copterDynamics.pitch - Kp*copterDynamics.roll);
      motorSpeeds[REAR_LEFT] = inputDynamics.thrust;//+= deltaThrust;
      motorSpeeds[REAR_LEFT] += delta;
      brushlessMotor[REAR_LEFT].writeMicroseconds(motorSpeeds[REAR_LEFT]);
/*    //  Serial.print(motorSpeeds[REAR_LEFT]);

      //Serial.println("");
      copterDynamics.thrust = inputDynamics.thrust;
      
      //String debugStr = String(motorSpeeds[FRONT_LEFT]) + "\t" + String(motorSpeeds[FRONT_RIGHT]) + "\t" + String(motorSpeeds[REAR_RIGHT]) + "\t" + String(motorSpeeds[REAR_LEFT]);
      
      //brushlessMotor[FRONT_LEFT].writeMicroseconds(motorSpeeds[FRONT_LEFT]);
//      brushlessMotor[FRONT_RIGHT].writeMicroseconds(motorSpeeds[FRONT_RIGHT]);
      /*brushlessMotor[REAR_RIGHT].writeMicroseconds(motorSpeeds[REAR_RIGHT]);
      brushlessMotor[REAR_LEFT].writeMicroseconds(motorSpeeds[REAR_LEFT]);
   */
//    Serial.println("Hello");
  //delay(20);
}


