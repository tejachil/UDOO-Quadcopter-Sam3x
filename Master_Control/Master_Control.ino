#include <stdlib.h>     /* strtof */
#include <Servo.h>

const int debugLED = 8;
volatile boolean stateLED;
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
  brushlessMotor[FRONT_LEFT].attach(4);
  brushlessMotor[FRONT_RIGHT].attach(5);
  brushlessMotor[REAR_RIGHT].attach(6);
  brushlessMotor[REAR_LEFT].attach(7);
  arm(brushlessMotor[FRONT_LEFT]);
  arm(brushlessMotor[FRONT_RIGHT]);  
  arm(brushlessMotor[REAR_RIGHT]);
  arm(brushlessMotor[REAR_LEFT]);
  Serial.println("MOTORS ARMED");
  //startTimer(TC1, 0, TC3_IRQn, 50); //TC1 channel 0, the IRQ for that channel and the desired frequency
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
    Serial.println(imuBuf);
    
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
    if(d == '$')  imuBuf = "";
    inputBuf += d;
  }
  Serial.flush();
  if(d == '\n'){
    digitalWrite(debugLED, stateLED = !stateLED);
    if(inputBuf.startsWith("$CONTROL DYNAMICS")){
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
      copterDynamics.throttle = stringToFloat(inputBuf);
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
  
  setSpeed(brushlessMotor[FRONT_LEFT], (int)copterDynamics.throttle);
  setSpeed(brushlessMotor[FRONT_RIGHT], (int)copterDynamics.throttle);
  setSpeed(brushlessMotor[REAR_RIGHT], (int)copterDynamics.throttle);
  setSpeed(brushlessMotor[REAR_LEFT], (int)copterDynamics.throttle);
    //delay(500);
//    Serial.println("Hello");
  //delay(20);
}


