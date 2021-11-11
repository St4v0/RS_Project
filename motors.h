// this #ifndef stops this file
// from being included mored than
// once by the compiler. 
#ifndef _MOTORS_H
#define _MOTORS_H



// Class to operate the motor(s).
class Motors_c {
  public:
  #define P5 5
    // Constructor, must exist.
    Motors_c() {

    } 
    #define L_PWM_PIN 10
    #define L_DIR_PIN 16
    #define R_PWM_PIN 9
    #define R_DIR_PIN 15
    
    #define FWD LOW
    #define BWD HIGH
    float inc = 10;
    float start = 0;
    float t_lim = 60;
    float b_lim = -60;
    float zero = 0;

    
    // Use this function to 
    // initialise the pins and 
    // state of your motor(s).
    void initialise() {
      pinMode(L_PWM_PIN, OUTPUT);
      pinMode(R_PWM_PIN, OUTPUT);
      pinMode(L_DIR_PIN, OUTPUT);
      pinMode(R_DIR_PIN, OUTPUT);
    
      // Set initial direction (HIGH/LOW)
      // for the direction pins.
      digitalWrite(L_DIR_PIN, LOW); 
      digitalWrite(R_DIR_PIN, LOW);
      
    
      // Set initial values for the PWM
      // Pins.
      analogWrite(L_PWM_PIN, 0);
      analogWrite(L_PWM_PIN, 0);
    
    
      // Start serial, send debug text.
      Serial.begin(9600);
      delay(1000);
      Serial.println("***RESET***");
    }

    // Write a function to operate
    // your motor(s)
    // ...
    void setMotorPower( float R_pwm, float L_pwm ) {
      if(R_pwm == 0){
        analogWrite( R_PWM_PIN, 0 );
      }
      if(R_pwm>0){
        if(R_pwm>255){
          //Serial.println("Right Value too high");
          R_pwm =0;
        }
        digitalWrite(R_DIR_PIN, LOW);
        analogWrite( R_PWM_PIN, R_pwm );
      }
      if(R_pwm<0){
        if(R_pwm<-255){
          //Serial.println("Right Value too low");
          R_pwm=0;
        }
        digitalWrite(R_DIR_PIN, HIGH);
        analogWrite( R_PWM_PIN, -R_pwm );
      }
      if(L_pwm == 0){
        analogWrite( L_PWM_PIN, 0 );
      }
      if(L_pwm>0){
        if(L_pwm>255){
          //Serial.println("Left Value too high");
          L_pwm =0;
        }
        digitalWrite(L_DIR_PIN, LOW);
        analogWrite( L_PWM_PIN, L_pwm );
      }
      if(L_pwm<0){
        if(L_pwm<-255){
          //Serial.println("Left too low");
          L_pwm=0;
        }
        digitalWrite(L_DIR_PIN, HIGH);
        analogWrite( L_PWM_PIN, -L_pwm );
      }
    }
    
};



#endif
