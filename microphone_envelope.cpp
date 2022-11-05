#include <Arduino.h>
#include <cmath>

// initialize variables

  #define voltage_duration_pin   34
  #define ledPin   25
  #define outputPin 26
  #define seal_check_pin   39
  const int freq = 500;
  const int ledChannel = 0;
  const int resolution = 8;


void setup() {
  Serial.begin(9600);
   ledcSetup(ledChannel, freq, resolution);
   ledcAttachPin(ledPin, ledChannel);
}

void loop() {

  float sound;
  const float variance = 0.5;
  float t;
  int pwm;
  float time;
  int voltage_duration = analogRead(voltage_duration_pin);
  int voltage;
  int output = analogRead(outputPin);
  int seal_check = analogRead(seal_check_pin);
  Serial.println(output);

  if (seal_check > 1365){

  if (voltage_duration < 1365){
  t = 0;
  time = 0;

  while (t < 1){


  if (esp_timer_get_time() > time){
  time = esp_timer_get_time();
  sound = 1/(sqrt(2*M_PI*variance))*(exp((-1* pow((t-0.5),2))/(2*variance)));  // Gaussian Function Centered around 0.5 sec
  pwm = int(255*sound);  // mapping this to pwm outputs
  ledcWrite(ledChannel, pwm); // Send via PWM
  voltage = int(4095*sound);  // mapping this to pwm outputs
  dacWrite(outputPin, voltage);
  t = t + 0.0002;
  time = time + 200;  //Sending output only every 0.2 ms.
  }
  }
  }

  else if (voltage_duration >= 1365 and voltage_duration <= 2730){
  t = 0;
  time = 0;

  while (t < 2){


  if (esp_timer_get_time() > time){
  time = esp_timer_get_time();
  sound = 1/(sqrt(2*M_PI*variance))*(exp((-1* pow((t-1),2))/(2*variance)));  // Gaussian Function Centered around 1 sec
  pwm = int(255*sound);  // mapping this to pwm outputs
  ledcWrite(ledChannel, pwm); // Send via PWM
  voltage = int(4095*sound);  // mapping this to pwm outputs
  dacWrite(outputPin, voltage);
  t = t + 0.0002;
  time = time + 200;  //Sending output only every 0.2 ms.
  }
  }
  }

  else {
  t = 0;
  time = 0;

  while (t < 3){


  if (esp_timer_get_time() > time){
  time = esp_timer_get_time();
  sound = 1/(sqrt(2*M_PI*variance))*(exp((-1* pow((t-1.5),2))/(2*variance)));  // Gaussian Function Centered around 1.5 sec
  pwm = int(255*sound);  // mapping this to pwm outputs
  ledcWrite(ledChannel, pwm); // Send via PWM
  voltage = int(4095*sound);  // mapping this to pwm outputs
  dacWrite(outputPin, voltage);
  t = t + 0.0002;
  time = time + 200;  //Sending output only every 2 ms.
  }
  }
  }
}
else 
ledcWrite(ledChannel, 0); // Send via PWM
voltage = 0;
}

