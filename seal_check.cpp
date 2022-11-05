#include <Arduino.h>

// We assigned a name LED pin to pin number 22
const int LEDPIN = 22; 
// this will assign the name PushButton to pin numer 15
const int PushButton = 15;
//const int SealCheck = 13;
int LEDState = 0;
int buttonNEW;
int buttonOLD =1;
int dt = 100;
// This Setup function is used to initialize everything 
void setup()
{
// This statement will declare pin 22 as digital output 
Serial.begin(9600);
pinMode(LEDPIN, OUTPUT);
// This statement will declare pin 15 as digital input 
pinMode(PushButton, INPUT);
}

void loop()

{
// digitalRead function stores the Push button state 
// in variable push_button_state
buttonNEW = digitalRead(PushButton);
// if condition checks if push button is pressed
// if pressed LED will turn on otherwise remain off 
if ( buttonOLD==0 && buttonNEW == 1 )  { 
  if (LEDState==0){
    digitalWrite(LEDPIN, HIGH);
    digitalWrite(SealCheck, HIGH);
    LEDState=1;
  }
  else{
    digitalWrite(LEDPIN, LOW);
    digitalWrite(SealCheck, LOW);
    LEDState = 0;
  }
}
buttonOLD = buttonNEW;
}