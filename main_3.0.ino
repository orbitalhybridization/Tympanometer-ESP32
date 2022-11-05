#include "base64.hpp" // must install base64 by densaugeo
#include <ArduinoJson.h>
#include <cmath>

// DEFINE TIMEOUTS
#define PIN_RED    19 // GIOP19
#define PIN_GREEN  18 // GIOP19
#define PIN_BLUE   17 // GIOP17
#define pinVoltage 25
#define ENVELOPE_MIC 39
#define CARRIER_MIC 34
#define PRESS_SENSOR 35
#define SEAL_DETECTOR 32
#define SEAL_ON 33
#define SEAL_TIMEOUT_us 5000000 // how long to wait for getting a seal

#define CURRENT_MIC_SOURCE CARRIER_MIC // make mic sources switchable

// DEFINE GLOBALS
int carrier_val; // containers for peripheral reads
int envelope_val;
int pressure_val; 
float currentTime; // timer 
float upTime; // time of measurement startup
int potentiometer_v; // potent. for duration
int max_time; // duration of test

unsigned int cyclenum = 0; // index of cycles before averaging
unsigned int data_count = 0; // amount of data saved to final array

bool printed; // unfortunate ... but funny.

float dataArray[6000]; // array of data to save
//constexpr int NUM_TO_AVG = 1; // number of data points to average into one
//unsigned int P_array_temp[NUM_TO_AVG] = {}; // arrays for temp storing read values which are averaged
//unsigned int Mic_array_temp[NUM_TO_AVG]  = {};


const unsigned int NUM_TO_AVG = 1;
unsigned int P_array_temp[NUM_TO_AVG];
unsigned int Mic_array_temp[NUM_TO_AVG];

size_t DATA_SIZE_str = 7; // THIS DEPENDS ON WHAT TYPE WE WANT
size_t DATA_SIZE_BASE64 = std::ceil(((DATA_SIZE_str * 8.0) / 6.0)); // might need more space


void setup() {
  // serial for debug
  //Serial.begin(9600);
  Serial.begin(115200);
  // Set our duration based on potentiometer voltage
  potentiometer_v = analogRead(pinVoltage);
  set_time(potentiometer_v);

  pinMode(PIN_RED,OUTPUT); // digital out
  pinMode(PIN_GREEN, OUTPUT); // digital out 
  pinMode(ENVELOPE_MIC,INPUT); // analog in
  pinMode(CARRIER_MIC,INPUT); // analog in
  pinMode(PRESS_SENSOR,INPUT); // analog in
  pinMode(SEAL_DETECTOR,INPUT); // digital in
  pinMode(SEAL_ON,OUTPUT); // digital out 

  //analogReadResolution(8);

  //attachInterrupt(digitalPinToInterrupt(SEAL_DETECTOR),startMeasurement,RISING); //interrupt for set t=0 when seal detected
}

void set_time(int voltage) {
  /*
   * Set up total time to run measurement.
   * Bsaed on potentiometer set.
   */
   
  if (voltage < 1365) {
    max_time = 1000000;
  }
 else if (voltage > 2730) {
    max_time = 3000000;
  }
 else {
    max_time = 2000000;
  }
   
}

float averageArray(unsigned int * arr) {
  /*
   * Average elements of an array with array and num elements. 
   * Reinventing the wheel! :D
   */

  unsigned int sum = 0;
  for (int i=0; i< NUM_TO_AVG; i++){
    sum += arr[i];
  }
  float avg = ((float)sum) / ((float)NUM_TO_AVG);
  return avg;
  
}

unsigned int mapPWMToPressure(unsigned int in) {
  /*
   * Map [0,255] to [-400,200] 
   */
   unsigned int result = ((in - 170.0) * 600.0) / 255.0;
   //unsigned int result = ((in - 170.0) * 600.0) / 4085.0;
   return result;
}

void loop() {

    if (digitalRead(SEAL_DETECTOR)) {
//
      digitalWrite(SEAL_ON,HIGH);

      upTime = esp_timer_get_time(); // set relative time

      while ((currentTime - upTime) < max_time) { // continue to read values for duration
          
          currentTime = esp_timer_get_time();
          //Serial.print("Uptime: ");
          analogWrite(PIN_GREEN,255); // seal on  
          analogWrite(PIN_RED,0);
          analogWrite(PIN_BLUE,0);
          //Serial.println("Measuring...");

          // read values
          P_array_temp[cyclenum] = analogRead(PRESS_SENSOR); 
          //Serial.println(P_array_temp[cyclenum]);
          Mic_array_temp[cyclenum] = analogRead(CURRENT_MIC_SOURCE);
          //Serial.println(Mic_array_temp[cyclenum]);
      
          cyclenum += 1;
      
          if (cyclenum == NUM_TO_AVG) { // AVERAGE
            
            // save pressure then mic average values
            float avgP = averageArray(P_array_temp);
            float avgM = averageArray(Mic_array_temp);
            dataArray[data_count] = mapPWMToPressure(avgP);
            dataArray[data_count+1] = avgM;
            data_count += 2; // increment num of total data saved
            
            Serial.println(data_count);
      
            // reset temp arrays to get next batch
            P_array_temp[NUM_TO_AVG];
            Mic_array_temp[NUM_TO_AVG];
      
            // reset cyclenum
            cyclenum = 0;
            
          }

      }
      
      // time has passed once while loop completes
      Serial.println("Time's up!");
      // time's up! 
      Serial.print("Measurement completed in ");
      Serial.print((currentTime - upTime) / 1000000);
      Serial.print(" seconds.\n");
  
      Serial.println("----------------------------------------");
  
      // Show metadata
      Serial.print("Number of values collected: ");
      //Serial.flush();
      Serial.println(data_count);
      Serial.print("\n\n");

      transmitJSON(dataArray,data_count);
      
     } // end seal detected

     else {
      Serial.println("Ready to measure...");
      analogWrite(PIN_GREEN,0); // led  
      analogWrite(PIN_RED,0);
      analogWrite(PIN_BLUE,255); // waiting state
      digitalWrite(SEAL_ON,LOW); // tell peripherals
      //while((currentTime - upTime) > SEAL_TIMEOUT_us) { // wait for seal on
      //  currentTime = esp_timer_get_time();

      } // no seal
  
      // timeout passed.
      //analogWrite(PIN_GREEN,0); // led  
      //analogWrite(PIN_RED,255);
      //analogWrite(PIN_BLUE,0); 
      //digitalWrite(SEAL_ON,LOW);
      //Serial.println("No seal, u messed up!! Tearing down.");
      //delay(5000);

      
} // loop


void transmitJSON(float * dataArray, unsigned int array_length) {

  /*
  Encode float data from array as base64 and add to a JSON file.
  
  dataArray : c-style array with format [m_1,p_1,m_2,p_2,...,m_n,p_n]
        where m_i is a mic value and p_i is a pressure value
  */

  size_t JSON_size = DATA_SIZE_BASE64 * array_length;

  // init doc and necessary variables
  DynamicJsonBuffer json_buffer; // create a dynamic type to save stack space
  JsonObject & doc = json_buffer.createObject();
  JsonArray& mic_val_array = doc.createNestedArray("Mic Values");
  JsonArray& pressure_val_array = doc.createNestedArray("Pressure Values");

  for (size_t i =0;i<array_length-1;i=i+2) {

    // extract mic and pressure values
    float pressure_val = dataArray[i];
    float mic_val = dataArray[i+1];

    // container for string conversion
    char mic_val_str[DATA_SIZE_str]; // might have to add one
    char pressure_val_str[DATA_SIZE_str];

    // conver to str
    dtostrf(mic_val,4,2,mic_val_str);
    dtostrf(pressure_val,4,2,pressure_val_str);

    // set up container for base64-encoded data
    unsigned char mic_base64[DATA_SIZE_BASE64]; // this depends on the size of our datas!
    unsigned char pressure_base64[DATA_SIZE_BASE64];  

    // encode strings as base64
    encode_base64((unsigned char *)mic_val_str,DATA_SIZE_str,mic_base64); // encode
    encode_base64((unsigned char *)pressure_val_str,DATA_SIZE_str,pressure_base64);

    // write base64-encoded data to JSON
    mic_val_array.add(mic_base64);
    pressure_val_array.add(pressure_base64);

  }

  // Print JSON to Serial port when done
  Serial.println();
  Serial.print("JSON Data\n");
  Serial.print("--------------\n");

  doc.printTo(Serial);
  Serial.print("--------------\n\n");
  
  Serial.println("Pressure values");
  for (int i = 0;i<data_count ; i+=2) {
    Serial.print(dataArray[i]);
    Serial.print(",");
  }

  Serial.println("\n Mic Values");
  for (int i = 1;i<data_count ; i+=2) {
    Serial.print(dataArray[i]);
    Serial.print(",");
  }
  
  /*
  Serial.println("\n All Values");
  for (int i = 1;i<data_count ; i++) {
    Serial.print(dataArray[i]);
    Serial.print(",");
    }
  */
  
  //delay(10000);
  

}





    
