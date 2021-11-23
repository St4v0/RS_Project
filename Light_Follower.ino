#include "bump_sensors.h"
#include "motors.h"
#include "encoders.h"
#include "pid.h"

#define GAIN_CONST 200
#define READ_INTERVAL 50
#define FWD  0

unsigned long current_ts;
unsigned long bump_ts = 0;
unsigned long elapsed_t;
float error_line = 0;
BumpSensor_c bumpSensors;
Motors_c motors;
PID_c pid;

void calibration(){
  motors.fullRotate();
  bumpSensors.Calibrate_Bump_Sensors();
  //bumpSensors.Display_Lapsed_Calibration();
  motors.stopMovement();
}

void setup() {
  // put your setup code here, to run once:
  motors.initialise();
  //pid.initialise();
  bumpSensors.initialise();
  setupEncoder0();
  setupEncoder1();
  delay(3000);
  bumpSensors.Calibrate_Bump_Sensors();
  bumpSensors.Display_Lapsed_Calibration();
  motors.setMotorsPwm(FWD, FWD);
}

void loop() {
  // put your main code here, to run repeatedly:
  current_ts = millis();
  motors.setMotorsPwm(FWD, FWD);
  elapsed_t = current_ts - bump_ts;
  if(elapsed_t >= READ_INTERVAL){
    bumpSensors.Read_Bump_Sensors();
    error_line = bumpSensors.getErrorLine();
        Serial.print("The ORIGINl Error Light is: ");
    Serial.println(error_line, 5);
    error_line *= GAIN_CONST;
    Serial.print("The Error Light is: ");
    Serial.println(error_line);
    if(error_line < -1){
      motors.rotate(LOW);
      bumpSensors.Display_Lapsed_Time();
      Serial.println("Turn LEFT!!!");
    }else if(error_line > 3){
      motors.rotate(HIGH);
      bumpSensors.Display_Lapsed_Time();
      Serial.println("Turn RIGHT!!!");
    }else{
      motors.stopMovement();
      bumpSensors.Display_Lapsed_Time();
      Serial.println("STOP MOTION!!!");
    }
//    motors.setMotorsPwm(FWD + error_line, FWD - error_line);
//    motors.powerBothMotors();
    bump_ts = millis();
  }

}
