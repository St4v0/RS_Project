// this #ifndef stops this file
// from being included mored than
// once by the compiler. 
#ifndef _BEEP_H
#define _BEEP_H

// Class to contain generic PID algorithm.
class Beep_c {
  public:

    // Constructor, must exist.
    Beep_c() {
      pinMode(6, OUTPUT);  
      }

    void beep(float pitch, float duration ){
      
      unsigned long end_dur = millis();
      unsigned long end_pitch = micros();

      while (millis() - end_dur  < duration) {
        if (micros()- end_pitch > pitch) {
          digitalWrite(6,!digitalRead(6));
          end_pitch = micros();
          }
        }
        digitalWrite(6,0);
}
};

#endif
