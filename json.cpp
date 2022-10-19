#include "base64.hpp" // must install base64 by densaugeo
#include <cmath>
// make sure also to install the arduinojson library

void transmitJSON(dataArray,array_length) {

	/*
	Encode float data from array as base64 and add to a JSON file.
	
	dataArray : c-style array with format [m_1,p_1,m_2,p_2,...,m_n,p_n]
		    where m_i is a mic value and p_i is a pressure value
	*/

	// size variables (might want to make these global)
	DATA_SIZE_str = 7; // THIS DEPENDS ON WHAT TYPE WE WANT
	DATA_SIZE_BASE64 = std::ceil(((DATA_SIZE_str * 8.0) / 6.0)); // might need more space
	JSON_size = DATA_SIZE_BASE64 * array_length;

	// init doc and necessary variables
	StaticJsonDocument<600> doc; // create a static type to save space
	JsonArray mic_val_array = doc.createNestedArray("Mic Values");
	JsonArray pressure_val_array = doc.createNestedArray("Pressure Values");

	for (size_t i =0;i<array_length-1;i=i+2) {

		// extract mic and pressure values
		float mic_val = dataArray[i];
		float pressure_val = dataArray[i+1];

		// container for string conversion
		unsigned char mic_val_str[DATA_SIZE_str]; // might have to add one
		unsigned char pressure_val_str[DATA_SIZE_str];

		// conver to str
		dtostrf(mic_val,4,2,mic_val_str);
		dtostrf(pressure_val,4,2,pressure_val_str);

		// set up container for base64-encoded data
		unsigned char mic_base64[DATA_SIZE_BASE64]; // this depends on the size of our datas!
		unsigned char pressure_base64[DATA_SIZE_BASE64];	

		// encode strings as base64
		base64::encode(mic_val_str,DATA_SIZE_str,mic_base64); // encode
		base64::encode(pressure_val_str,DATA_SIZE_str,pressure_base64);

		// write base64-encoded data to JSON
		mic_val_array.add(mic_base64);
		pressure_val_array.add(pressure_base64)

	}

	// Print JSON to Serial port when done
	Serial.println();
	Serial.print("JSON Data\n");
	Serial.print("--------------\n");
	serializeJsonPretty(doc,Serial);

}
