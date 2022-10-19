
#define pinVoltage 25
#define pinPressure 26
 float voltage;
 int t;
 float pinPressure_value;
 void setup() {
  // put your setup code here, to run once:
pinMode(pinVoltage ,INPUT);
pinMode(pinPressure,OUTPUT);

Serial.begin(9600);

  }

int convertToAnalog(float pinPressure_value) {
  int result = (1023.0/600.0)* pinPressure_value + 682.0;
  return result;
}
void loop() {
  // put your main code here, to run repeatedly:
  t= millis();
  voltage= analogRead(pinVoltage);
  if (voltage<1365){
   for (t=0;t<1000;t++){
    pinPressure_value=  -0.6*t + 200;
    ledcWrite(pinPressure,convertToAnalog(pinPressure_value));
    Serial.println(convertToAnalog(pinPressure_value));
     if (t==1000) {
      break;
     }
   }
  }
  else if (voltage>= 1365 && voltage <= 2730){
    for (t=0;t<2000;t++){
    pinPressure_value=  -0.3*t + 200;
    ledcWrite(pinPressure,convertToAnalog(pinPressure_value));
    Serial.println(convertToAnalog(pinPressure_value));
    
     if (t==2000) {
      break;
     }
   }
  }
  else{
    for (t=0;t<3000;t++){
    pinPressure_value=  -0.2*t + 200;
    ledcWrite(pinPressure,convertToAnalog(pinPressure_value));
    Serial.println(convertToAnalog(pinPressure_value));
     if (t==3000) {
      break;
     }
   }
  }
}

