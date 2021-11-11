#include "motors.h"
#include "linesensor.h"
#include "encoders.h"
#include "kinematics.h"
#include "pid.h"
#include "beep.h"
#include "actions.h"
#include "bumpsensor.h"


Motors_c motors;
LineSensor_c ls;
BumpSensor_c bs;
Beep_c beep;
Kinematics_c kn;
Actions_c act;


#define LED_PIN 13  // Pin to activate the orange LED
boolean led_state;  // Variable to "remember" the state
                    // of the LED, and toggle it.
#define PI 3.1415926535897932384626433832795

unsigned long start;
unsigned long lost;
bool already_lost = false;
bool flag_join_line=false;
bool end = false;
bool flag = false;
unsigned long bump_ts;
unsigned long elapsed_t;
// put your setup code here, to run once:
void setup() {

  
  // Initialise the motor gpio
  motors.initialise();
  ls.initialise();
  bs.initialise();
  setupEncoder0();
  setupEncoder1();
  
  Serial.begin(9600);
  delay(1000);
  Serial.println("***RESET***");

  // Set LED pin as an output
  pinMode( LED_PIN, OUTPUT );

  // Set initial state of the LED
  led_state = false;

  beep.beep(1000,100);
}


// put your main code here, to run repeatedly:
void loop() {
  unsigned long current = millis();
  elapsed_t = current - bump_ts;
  if(elapsed_t > 200){
    bs.readBump();
    Serial.print(bs.L_val);
    Serial.print(",");
    Serial.println(0);
    //Serial.println(bs.R_val);
    bump_ts = millis();
  }

 //delay(200);
}
