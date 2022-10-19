#include <Arduino.h>
#include <cmath>

// initialize variables

  #define voltage_duration_pin   34
  #define ledPin   25
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
  Serial.println(voltage_duration);

  if (voltage_duration < 1365){
  t = 0;
  time = 0;

  while (t < 1){


  if (esp_timer_get_time() > time){
  time = esp_timer_get_time();
  sound = 1/(sqrt(2*M_PI*variance))*(exp((-1* pow((t-0.5),2))/(2*variance)));  // Gaussian Function Centered around 0.5 sec
  pwm = int(255*sound);  // mapping this to pwm outputs
  ledcWrite(ledChannel, pwm); // Send via PWM
  t = t + 0.002;  // Since max rate pwm can be written is 490 Hz
  time = time + 2000;  //Sending output only every 2 ms.
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
  t = t + 0.002;  // Since max rate pwm can be written is 490 Hz
  time = time + 2000;  //Sending output only every 2 ms.
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
  t = t + 0.002;  // Since max rate pwm can be written is 490 Hz
  time = time + 2000;  //Sending output only every 2 ms.
  }
  }
  }
}

