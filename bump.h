// this #ifndef stops this file
// from being included mored than
// once by the compiler.
#ifndef _BUMPSENSOR_H
#define _BUMPSENSOR_H

#define WINDOW_SIZE 15 //window size for moving average in calibration readings
#define LEFT_BUMP_SENSOR 4//left sensor pin number
#define RIGHT_BUMP_SENSOR 5//right sensor pin number
#define NB_BS_PIN 2
#define R_EXPONENT_WEIGHT 0.05//Exponent weight for right sensor
#define L_EXPONENT_WEIGHT 0.05//Exponent weight for left sensor
#define GAIN_CONST 150
float Exponent_Weights[NB_BS_PIN] = {L_EXPONENT_WEIGHT, R_EXPONENT_WEIGHT};//array of exponent weights

#define EMIT_PIN 11
#define TIME_OUT 0
#define MIN_TIME  200000//default minimum time (pior to calibration) based on observed values
#define CALIBRATION_TIME 4000000 //calibration time in microseconds 
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
    }

    void Calibrate_Bump_Sensors(){
      unsigned long temp_elapsed_time;
      float Moving_average = 0;
      unsigned long cal_start_time = micros();
      unsigned long elapsed_cal_time = 0;
      while(done == false){
        PT_Charge();
        while(read_end[0] == false || read_end[1] == false){
          for(int Which_Sensor = 0; Which_Sensor < NB_BS_PIN; ++Which_Sensor){
            temp_elapsed_time = micros() - start_time;         
            if(digitalRead(bs_pin[Which_Sensor]) == LOW && !read_end[Which_Sensor]){
              Which_Sensor == 0 ?  SUM[Which_Sensor] -= Left_Readings[INDEX] : SUM[Which_Sensor] -= Right_Readings[INDEX];
              Which_Sensor == 0 ?  Left_Readings[INDEX] = temp_elapsed_time : Right_Readings[INDEX] = temp_elapsed_time;
              SUM[Which_Sensor] += temp_elapsed_time;
              Moving_average = static_cast<float>(SUM[Which_Sensor])/WINDOW_SIZE;  
              read_end[Which_Sensor] = true;          
              if(max_average_time[Which_Sensor] < Moving_average) max_average_time[Which_Sensor] = Moving_average;
              if(min_average_time[Which_Sensor] > Moving_average) min_average_time[Which_Sensor] = temp_elapsed_time;
            }
          }
        }
        INDEX = (INDEX+1) % WINDOW_SIZE;
        elapsed_cal_time = micros() - cal_start_time;
        if(elapsed_cal_time > CALIBRATION_TIME){
           done = true;     
           calculateScalingFactor();
           for(int k = 0; k < NB_BS_PIN; ++k) SUM[k] = 0;
           for(int j = 0; j < WINDOW_SIZE; ++j) Left_Readings[j] = 0;
           for(int i = 0; i < WINDOW_SIZE; ++i) Right_Readings[i] = 0;
        }
      }
      /*while(flag == false){
        for(int k = 0; k < 50; ++k){
           Read_Bump_Sensors();
           AvgCal[0]+=elapsed_time[0];
           AvgCal[1]+=elapsed_time[1];
        }
        flag = true;
        AvgCal[0]=AvgCal[0]/50;
        AvgCal[1]=AvgCal[1]/50;
      }*/
    }
    
    void Read_Bump_Sensors(){
      unsigned long temp_elapsed_time;
      PT_Charge();//charge the bump sensors
      while(done == false){    
        for(int Which_Sensor = 0; Which_Sensor < NB_BS_PIN; ++Which_Sensor){//loop through the sensor array
          temp_elapsed_time = micros() - start_time;//take elapsed time
          if((digitalRead(bs_pin[Which_Sensor]) == LOW /*|| temp_elapsed_time > TIME_OUT*/) && !read_end[Which_Sensor]){//if sensor is discharged and hasn't been previously read
//            Which_Sensor == 0 ?  SUM[Which_Sensor] -= Left_Readings[INDEX] : SUM[Which_Sensor] -= Right_Readings[INDEX];
//            elapsed_time[Which_Sensor] = temp_elapsed_time;
//            read_end[Which_Sensor] = true;
//            Which_Sensor == 0 ?  Left_Readings[INDEX] = temp_elapsed_time : Right_Readings[INDEX] = temp_elapsed_time;
//            SUM[Which_Sensor] += temp_elapsed_time;
//            AVERAGE[Which_Sensor] = static_cast<float>(SUM[Which_Sensor])/WINDOW_SIZE;
            elapsed_time[Which_Sensor] = temp_elapsed_time;//store elapsed time
            AVERAGE[Which_Sensor] = ((1-Exponent_Weights[Which_Sensor])* prev_elapsed_time[Which_Sensor]) + (Exponent_Weights[Which_Sensor] * elapsed_time[Which_Sensor]);//Exponential moving average calculation
            prev_elapsed_time[Which_Sensor] =  AVERAGE[Which_Sensor]; //current average becomes previous average for new read
            read_end[Which_Sensor] = true;//set read flag to true
          }
        }
        if(elapsed_time[0] != 0 && elapsed_time[1] !=  0){//if both sensors have been read
          done = true;//set the done boolean as true to break out at next loop iteration 
//          INDEX = (INDEX+1) % WINDOW_SIZE;//
          Display_Lapsed_Time();//display elapsed time
          getSensorCalibrationValue();//calculate the calibration value
        }
      }
    }
        
    void getSensorCalibrationValue(){
      for(int i = 0; i < NB_BS_PIN; ++i){
        if(elapsed_time[i] < min_average_time[i]){
          min_average_time[i] = AVERAGE[i];
          calculateScalingFactor();
        }
        if(AVERAGE[i] > max_average_time[i]){
          max_average_time[i] = AVERAGE[i];
          calculateScalingFactor();
        }
        sensors_measured_condition[i] = (AVERAGE[i] - min_average_time[i]) * sensors_calibration_factor[i];
      }
    }

    double getErrorLine(){
      return (sensors_measured_condition[0] - sensors_measured_condition[1]);
    }
    
    unsigned long Sensor_Time_Elapsed(int sensor){
      return elapsed_time[sensor];
    }

   void Display_Lapsed_Calibration(){
//      Serial.print("\nFINISHED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
//      Serial.print("\nBump sensor FINAL VALUES!: ");
          Serial.println("\nThe Max and Min sensor values are: ");
          
          Serial.print(max_average_time[0]);
          Serial.print(", ");
          Serial.println(max_average_time[1]);
          Serial.print(min_average_time[0]);
          Serial.print(", ");
          Serial.println(min_average_time[1]);
    }

    void Display_Lapsed_Time(){
//      Serial.print("\nFINISHED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
//      Serial.print("\nBump sensor FINAL VALUES!: ");
          //Serial.print("\nThe current sensor values are: ");
          /*Serial.print("L ");
          Serial.print(elapsed_time[0]);
          Serial.print(", R ");
          Serial.println(elapsed_time[1]);*/
          Serial.print(", ");
          Serial.print("L ");
          Serial.print(AVERAGE[0]);
          Serial.print(", R ");
          Serial.println(AVERAGE[1]);
    }
    
  private:
    unsigned long prev_elapsed_time[NB_BS_PIN] = { 0 };



  
    double AvgCal[NB_BS_PIN]= { 0 };// array to store the starting average calibrated value
    const int bs_pin[NB_BS_PIN] = {LEFT_BUMP_SENSOR, RIGHT_BUMP_SENSOR};//pin value array
    bool read_end[NB_BS_PIN] = {false, false};//read state array for each pin
    bool done;//done state variable after each sensor reading
    double SUM[NB_BS_PIN] = { 0 };//moving sum variable for each sensor
    float AVERAGE[NB_BS_PIN] = { 0 };//moving avaerage variable for each sensor
    unsigned long Right_Readings[WINDOW_SIZE] = { 0 };//dynamic readings for right sensor
    unsigned long Left_Readings[WINDOW_SIZE] = { 0 }; //dynamic readings for left sensor
    unsigned long start_time; //t_1
    unsigned long elapsed_time[NB_BS_PIN] = { 0 }; //to store elapsed time
    unsigned long max_average_time[NB_BS_PIN] = { 0 }; //to store max discharge time duting Calibration
    unsigned long min_average_time[NB_BS_PIN] = { MIN_TIME, MIN_TIME}; //to store min discharge time during Calibration
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
        sensors_calibration_factor[i] = (static_cast<double>(1) / (max_average_time[i] - min_average_time[i]));
      }
//      Serial.print("\nFINISHED CALIBRATION!!!!!!");
//      Serial.print(sensors_calibration_factor[0]);
//      Serial.print(", ");
//      Serial.println(sensors_calibration_factor[1]);
    }
};
#endif
