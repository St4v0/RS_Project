// this #ifndef stops this file
// from being included mored than
// once by the compiler.
#ifndef _BUMPSENSOR_H
#define _BUMPSENSOR_H

#define WINDOW_SIZE 15
#define LEFT_BUMP_SENSOR 4
#define RIGHT_BUMP_SENSOR 5
#define NB_BS_PIN 2

#define GAIN_CONST 150


#define EMIT_PIN 11
#define TIME_OUT 4000
#define MIN_TIME  550
#define CALIBRATION_TIME 2000000 
// Class to operate the linesensor(s).
class BumpSensor_c {
  public:
  
    // Constructor, must exist.
    BumpSensor_c() {

    }
    int INDEX = 0;
    void initialise(){
      for(int i = 0; i < NB_BS_PIN; ++i) pinMode(bs_pin[i], INPUT);
      for(int i = 0; i < NB_BS_PIN; ++i) SUM[i] = 0;
      pinMode(EMIT_PIN, INPUT);
      Serial.println("***The setup is Complete");
      delay(1000);
    }

    void Calibrate_Bump_Sensors(){
      unsigned long temp_elapsed_time;
      PT_Charge();
      bool flag = false;
      while(done == false){
        for(int Which_Sensor = 0; Which_Sensor < NB_BS_PIN; ++Which_Sensor){
          temp_elapsed_time = micros() - start_time;         
          if(digitalRead(bs_pin[Which_Sensor]) == LOW || temp_elapsed_time > TIME_OUT){
            
            if(max_elapsed_time[Which_Sensor] < temp_elapsed_time && temp_elapsed_time < TIME_OUT) max_elapsed_time[Which_Sensor] = temp_elapsed_time;
            if(min_elapsed_time[Which_Sensor] > temp_elapsed_time && temp_elapsed_time > MIN_TIME) min_elapsed_time[Which_Sensor] = temp_elapsed_time;
          }
        }
        if(temp_elapsed_time > CALIBRATION_TIME){
          done = true;     
          calculateScalingFactor();
        }
      }
      while(flag == false){
        for(int k = 0; k < 50; ++k){
           Read_Bump_Sensors();
           AvgCal[0]+=elapsed_time[0];
           AvgCal[1]+=elapsed_time[1];
        }
        flag = true;
        AvgCal[0]=AvgCal[0]/50;
        AvgCal[1]=AvgCal[1]/50;
      }
    }
    
    void Read_Bump_Sensors(){
      unsigned long temp_elapsed_time;
      PT_Charge();
      Start_Time();
      while(done == false){    
        for(int Which_Sensor = 0; Which_Sensor < NB_BS_PIN; ++Which_Sensor){
          temp_elapsed_time = micros() - start_time;         
          if((digitalRead(bs_pin[Which_Sensor]) == LOW /*|| temp_elapsed_time > TIME_OUT*/) && !read_end[Which_Sensor]){
            Which_Sensor == 0 ?  SUM[Which_Sensor] -= Left_Readings[INDEX] : SUM[Which_Sensor] -= Right_Readings[INDEX];
            elapsed_time[Which_Sensor] = temp_elapsed_time;
            read_end[Which_Sensor] = true;
            Which_Sensor == 0 ?  Left_Readings[INDEX] = temp_elapsed_time : Right_Readings[INDEX] = temp_elapsed_time;
            SUM[Which_Sensor] += temp_elapsed_time;
            AVERAGE[Which_Sensor] = static_cast<float>(SUM[Which_Sensor])/WINDOW_SIZE;
          }
        }
        if(elapsed_time[0] != 0 && elapsed_time[1] !=  0){
          done = true;
          INDEX = (INDEX+1) % WINDOW_SIZE;
          Display_Lapsed_Time();
          //getSensorCalibrationValue();
        }
      }
    }
        
    void getSensorCalibrationValue(){
      for(int i = 0; i < NB_BS_PIN; ++i){
        if(elapsed_time[i] < min_elapsed_time[i]){
          min_elapsed_time[i] = elapsed_time[i];
          calculateScalingFactor();
        }
        if(elapsed_time[i] > max_elapsed_time[i]){
          max_elapsed_time[i] = elapsed_time[i];
          calculateScalingFactor();
        }
        sensors_measured_condition[i] = (elapsed_time[i] - min_elapsed_time[i]) * sensors_calibration_factor[i];
      }
    }

    double getErrorLine(){
              return (sensors_measured_condition[0] - sensors_measured_condition[1]);
    }
    
    unsigned long Sensor_Time_Elapsed(int sensor){
      return elapsed_time[sensor];
    }

    void Display_Lapsed_Time(){
//      Serial.print("\nFINISHED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
//      Serial.print("\nBump sensor FINAL VALUES!: ");
          /*Serial.print("\nThe current sensor values are: ");
          Serial.print(elapsed_time[0]);
          Serial.print(", ");
          Serial.print(elapsed_time[1]);
          Serial.print("\n");
          Serial.print(AVERAGE[0]);
          Serial.print(", ");
          Serial.println(AVERAGE[1]);*/
          Serial.print(100*float(AVERAGE[0])/float(AvgCal[0]));
          Serial.print(", ");
          Serial.println(100*float(AVERAGE[1])/float(AvgCal[1]));
    }
    
  private:
    double AvgCal[NB_BS_PIN]= { 0 };// array to store the starting average calibrated value
    const int bs_pin[NB_BS_PIN] = {LEFT_BUMP_SENSOR, RIGHT_BUMP_SENSOR};//pin value array
    bool read_end[NB_BS_PIN];//read state array for each pin
    bool done;//done state variable after each sensor reading
    double SUM[NB_BS_PIN] = { 0 };//moving sum variable for each sensor
    float AVERAGE[NB_BS_PIN] = { 0 };//moving avaerage variable for each sensor
    unsigned long Right_Readings[WINDOW_SIZE] = { 0 };//dynamic readings for right sensor
    unsigned long Left_Readings[WINDOW_SIZE] = { 0 }; //dynamic readings for left sensor
    unsigned long start_time; //t_1
    unsigned long elapsed_time[NB_BS_PIN] = { 0 }; //to store elapsed time
    unsigned long max_elapsed_time[NB_BS_PIN] = { 0 }; //to store max discharge time duting Calibration
    unsigned long min_elapsed_time[NB_BS_PIN] = { TIME_OUT, TIME_OUT}; //to store min discharge time during Calibration
    double sensors_calibration_factor[NB_BS_PIN] = { 0 };//
    float sensors_measured_condition[NB_BS_PIN] = { 0 };
    void Start_Time(){
      start_time = micros(); 
    }
    
    //function to perform charge and start discharge time
    void PT_Charge(){
      for(int k = 0; k < NB_BS_PIN; ++k) elapsed_time[k] = 0;//reset the dischqrge time to zero 
      for(int k = 0; k < NB_BS_PIN; ++k) read_end[k] = false;//reset the read boolean to flase
      for(int i = 0; i < NB_BS_PIN; ++i) pinMode(bs_pin[i], OUTPUT);//reset LED pins to OUTPUT mode
      for(int j = 0; j < NB_BS_PIN; ++j) digitalWrite(bs_pin[j], HIGH);//Set the LED PINS to HIGH to charge capacitor
      delayMicroseconds(30);
      done = false;//reset done state to false
      for(int i = 0; i < NB_BS_PIN; ++i) pinMode(bs_pin[i], INPUT); //set the LEDs to INPUT mode
//    Serial.print("\nDone Charging and starts Discharging");
      Start_Time();
    }

    void calculateScalingFactor(){
      for(int i = 0; i < NB_BS_PIN; ++i){
        sensors_calibration_factor[i] = (static_cast<double>(1) / (max_elapsed_time[i] - min_elapsed_time[i]));
      }
      Serial.print("\nFINISHED CALIBRATION!!!!!!");
      Serial.print(sensors_calibration_factor[0]);
      Serial.print(", ");
      Serial.println(sensors_calibration_factor[1]);
    }
};
#endif
