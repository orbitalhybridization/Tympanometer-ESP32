// initialize variables
#define pinVoltage 35
#define pinPressure 26
#define pinPressureout 25
#define sealPin 36
uint pinPressure_value;
float voltage;
float t;
int max_time;
float slope;
int hello;
 const int freq = 500;
  const int ledChannel = 0;
  const int resolution = 8;
  //int seal_check =1;



void setup() {
  Serial.begin(9600);
  pinMode(pinVoltage ,INPUT);
  pinMode(pinPressure,OUTPUT);
  pinMode(sealPin, INPUT);
 // pinMode(pinReadP,INPUT);
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(pinPressure, ledChannel);
  


 voltage = analogRead(pinVoltage);
  Serial.println("Hello voltage is:");
  Serial.println(voltage);
}
void set_slope_time(int voltage) {
  if (voltage < 1365) {
    // smallest
    max_time = 1000000;
    //slope = -0.0006;
  }



 else if (voltage > 2730) {
    // biggest
    max_time = 3000000;
   // slope = -0.0002;
  }



 else {
    //medium
    max_time = 2000000;
   // slope = -0.0003;
  }
   
}


//int convertToAnalog(float pinPressure_value) {
 // unsigned int result = (4095.0/600.0)* pinPressure_value + 2730.0;
  //return result;
//}


//unsigned int convertToPWM(float pinPressure_value) { 
  //unsigned int result1 = (((4095.0/600.0)* pinPressure_value + 2730.0)*255)/4095;
  //return result1;
//}


void loop() {

if (digitalRead(sealPin)) {
float start_time = esp_timer_get_time();
    while (t < max_time) {
      voltage = analogRead(pinVoltage);
      set_slope_time(voltage);
       t = esp_timer_get_time() - start_time;
       pinPressure_value =  ((-255.0/(max_time))*t )+ 255;
       //Serial.println(t);
       //pinPressure_value= 100;
       //Serial.println(convertToPWM(pinPressure_value));
       Serial.println(pinPressure_value);
       ledcWrite(ledChannel,pinPressure_value);  
       //Serial.println(convertToAnalog(pinPressure_value));
       dacWrite(pinPressureout,pinPressure_value);
       
    }
    t = 0;
    start_time = esp_timer_get_time();
    set_slope_time(voltage);

}
else { 
  ledcWrite(ledChannel,0);  
}
}
