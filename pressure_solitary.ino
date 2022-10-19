#include <Arduino.h>
#include <cmath>

// initialize variables
#define pinVoltage 25
#define pinPressure 26
#define pinReadP 36
float pinPressure_value;
float voltage;
float t;
int max_time;
float slope;
int hello;

void setup() {
  Serial.begin(9600);
  pinMode(pinVoltage ,INPUT);
  pinMode(pinPressure,OUTPUT);
  pinMode(pinReadP,INPUT);

  voltage = analogRead(pinVoltage);
  Serial.println("Hello voltage is:");
  Serial.println(voltage);
  
  if (voltage < 1365) {
    // smallest 
    max_time = 1000000;
    slope = -0.0006;
  }

  else if (voltage > 2730) {
    // biggest
    max_time = 3000000;
    slope = -0.0002;
  }

  else {
    //medium
    max_time = 2000000;
    slope = -0.0003;
  }
   
}

int convertToAnalog(float pinPressure_value) {
  int result = (4095.0/600.0)* pinPressure_value + 2730.0;
  return result;
}

void loop() {
//if (seal_check = digitalRead(sealPin)) { 
 float start_time = esp_timer_get_time();
  voltage = analogRead(pinVoltage);
    while (t < max_time) {
       t = esp_timer_get_time() - start_time;
       pinPressure_value =  slope*t + 200;
       
       analogWrite(pinPressure,convertToAnalog(pinPressure_value));
       Serial.println(convertToAnalog(pinPressure_value));
       
    }
}
//}
