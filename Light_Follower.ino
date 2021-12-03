#include "bump_sensors.h"
#include "motors.h"
#include "encoders.h"
#include "pid.h"

#define GAIN_CONST 1000
#define READ_INTERVAL 80
#define FWD  0
#define FWD_R 0
#define LIGHT_FOLLOW 0

unsigned long current_ts;
unsigned long bump_ts = 0;
unsigned long elapsed_t;
float error_line = 0;
double heading_signal;
double rotation_velocity1 = 0;
double rotation_velocity2 = 0;
double feedback_signal1 = 0;
double feedback_signal2 = 0;
float lpwm = 0;
float rpwm = 0;

BumpSensor_c bumpSensors;
Motors_c motors;
PID_c heading_controller;
PID_c pid1;
PID_c pid2;

void calibration(){
  motors.fullRotate();
  bumpSensors.Calibrate_Bump_Sensors();
  motors.stopMovement();
}

void setup() {
  // put your setup code here, to run once:
  motors.initialise();
  delay(2000);
  bumpSensors.initialise();
  calibration();
  setupEncoder0();
  setupEncoder1();
  heading_controller.initialise(1, 0, 2);
  pid1.initialise(1, 0.00003, 3);
  pid2.initialise(1, 0.00003, 3);
  bumpSensors.Display_Lapsed_Calibration();
//  motors.setMotorsDirection(FWD, FWD);
}

void loop() {
  // put your main code here, to run repeatedly:
  current_ts = millis();
  elapsed_t = current_ts - bump_ts;
  if(elapsed_t >= READ_INTERVAL){
    bumpSensors.Read_Bump_Sensors();
//    Serial.print("The ORIGINl Error Light is: ");
//    Serial.println(error_line, 5);
    error_line = GAIN_CONST * bumpSensors.getErrorLine();
    Serial.print("The Error Light is: ");
    Serial.println(error_line);
    if(error_line > -2 && error_line < 3)error_line = 0;
    if(error_line < -60) error_line *= 0.2;
    if(error_line > 40)error_line *= 0.2;
    /*if(error_line < -1){
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
    }*/
    heading_signal = heading_controller.update_pid(LIGHT_FOLLOW, error_line, elapsed_t);
//    Serial.print("The heading signal is: ");
//    Serial.println(heading_signal);
    calculateWheelsVel();
    feedback_signal1 = pid1.update_pid(heading_signal, rotation_velocity1, elapsed_t);
    feedback_signal2 = pid2.update_pid(heading_signal, rotation_velocity2, elapsed_t);
//    Serial.print("The Feedback Signal 1 is: ");
//    Serial.println(feedback_signal1);
//    Serial.print("The Feedback Signal 2 is: ");
//    Serial.println(feedback_signal2);
    lpwm = FWD - feedback_signal1;
    rpwm = FWD_R + feedback_signal2;
    
    motors.setMotorsPwm(lpwm, rpwm);
    motors.powerBothMotors();
//    if(error_line > -2 && error_line < 2){
//      while(true) 
//        motors.stopMovement();
//    }    
    right_wheel_count = 0;
    left_wheel_count = 0;
    bump_ts = millis();
  }
}

void calculateWheelsVel(){
    rotation_velocity1 = static_cast<double>(left_wheel_count)/elapsed_t;
    rotation_velocity2 = static_cast<double>(right_wheel_count)/elapsed_t;
    Serial.print("The Velocity 1 is: ");
    Serial.println(rotation_velocity1);
    Serial.print("The Velocity 2 is: ");
    Serial.println(rotation_velocity2);
}
