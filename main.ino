#include <Arduino.h>
#include <math.h> 
// DEFINE PINS
// TODO: add actual pin numbers
#define PIN_RED    19 // GIOP19
#define PIN_GREEN  18 // GIOP19
#define PIN_BLUE   17 // GIOP17

#define ENVELOPE_MIC 3
#define CARRIER_MIC 6
#define PRESS_SENSOR 7
#define SEAL_DETECTOR 32
#define SEAL_ALERT 33

// DEFINE TIMEOUTS
#define SEAL_TIMEOUT_ms 10000 // how long to wait for getting a seal
#define READ_TIMEOUT_ms 10000 // how long to wait after getting no signal

// DEFINE GLOBALS
int8 cyclenum = 0; // number of reads from each peripheral (resets at NUM_TO_AVG)
float avgP; // get avg pressure from sampling
float avgMic; // avg mic audio from sampling
float dataArray[600]; // array of data to save
int data_count = 0; // index into the array of data to save % was originally int8 but i don't think that exists in arduino, it might just be int 
int NUM_TO_AVG = 100; // number of data points to average into one
float P_array_temp[NUM_TO_AVG]; // arrays for temp storing read values which are averaged
float Mic_array_temp[NUM_TO_AVG];


void setup() {
  // Set up pin I/O
  pinMode(PIN_RED,OUTPUT); // digital out
  pinMode(PIN_GREEN, OUTPUT); // digital out 
  pinMode(ENVELOPE_MIC,INPUT); // analog in
  pinMode(CARRIER_MIC,INPUT); // analog in
  pinMode(PRESS_SENSOR,INPUT); // analog in
  pinMode(SEAL_DETECTOR,INPUT); // digital in
  pinMode(SEAL_ALERT,OUTPUT); // digital out 
  
  // have an interrupt for hermatic seal broken
  attachInterrupt(digitalPinToInterrupt(SEAL_DETECTOR),seal_broken,FALLING);

}

void seal_broken() {
  digitalWrite(PIN_GREEN,0); // alert seal lost
  digitalWrite(PIN_RED, 255); 
  Serial.print("Seal was lost bro wtf.");
  return;
}

void loop() {
  
  // put your main code here, to run repeatedly:
  
  int upTime = milis(); // get start time to calculate timeout
  
  // wait for seal
  while (!digitalRead(SEAL_DETECTOR)) {
    int currentTime = milis();
    if ((currentTime - upTime) > SEAL_TIMEOUT_ms) {
      Serial.print("No seal, u messed up!! Tearing down.");
      return;
    }
  }
  
  // seal hits
  digitalWrite(SEAL_ALERT,HIGH);
  digitalWrite(SEAL_LED,HIGH); // led  

  // SAMPLING

    // logically figure out how much time has elapsed
    // decimate in real time (get every 10th value)
    // OR get 10 values and then average
    // interpolation?
    // both mics at once
   // AM modulated = A*sin A function of time , gaussian
  // AM is a gaussian
  
  bool sweepFinished; // keep track of pressure, switch to true when we hit last pressure

  while (!sweepFinished) {
  cyclenum += 1;
  P_array_temp[cyclenum] = analogRead(PRESS_SENSOR); // read from peripherals
  Mic_array_temp[cyclenum] = analogRead(CARRIER_MIC);
  if cyclenum == NUM_TO_AVG { // if we're on the 100th cycle, then average
      int sumMic = 0;
      int sumP = 0;
      for (i=0; i< NUM_TO_AVG; i++){
    sumMic += Mic_array_temp[i]; 
    sumP += P_array_temp[i];
      }
      avgP = sumP/NUM_TO_AVG;
      avgMic = sumMic/NUM_TO_AVG;
   
  dataArray[data_count] = avgMic; // save values
    dataArray[data_count+1] = avgP;
  data_count += 2;
    // reset temp arrays to get next batch
    P_array_temp = [];
    Mic_array_temp = [];
    
    // reset cycle_num
    cycle_num = 0;
  
  }
  
 }

  if data_count == 600 { 
    StaticJsonDocument<600> doc; // this should go above
    JsonArray DATA_json = doc.to<JsonArray>();
    DATA_json.add(dataArray);
    serializeJson(doc, Serial);
  } 
}

def mapVoltToPressure(float in) {
  ;
}

def mapVoltToDB(float in) {
  ;
}
