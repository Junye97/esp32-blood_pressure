#include "BLOODPRESSURE.h"
#include "FILTER.h"
#include <Arduino.h>

#define FILTER_NUM_B 6 // Num of filter coeff tabs
#define FILTER_NUM_A 6 
#define NUM_CHANNELS 1 // Support 1 channel filtfilt only
#define channel_count 1 // Variable name following original filtfilt

#define FILTER_TESTING

#ifdef FILTER_TESTING
	#define TESTING_FILE_LENGTH 1000 //50250

	// #include "SPIFFS.h"
	// #include <string>
	// #include <iostream>

	void spiffs_dataread(filter_dt *buffer) {
		if(!SPIFFS.begin(true)){
			Serial.println("An Error has occurred while mounting SPIFFS");
			return;
		}
		
		File file = SPIFFS.open("/KS.dat");
		if(!file){
			Serial.println("Failed to open file for reading");
			return;
		}
		
		int line_counter = 0;
		Serial.println("File Content:");
		while((file.available()) & line_counter < TESTING_FILE_LENGTH){
			// Serial.printf("%s\n", file.read());
			// buffer[line_counter] = std::stod(file.read());
			// Serial.printf("line_counter=%d\n", line_counter);
			++line_counter;
		}
		file.close();
	}
#endif


// Filter coefficients
double const filter_b[FILTER_NUM_B] = {6.8515610836736729e-01,  -3.4028173297598676e+00,   6.7794474514361607e+00,  -6.7726741361536948e+00,   3.3926092540878621e+00,  -6.8172077928960340e-01};
double const filter_a[FILTER_NUM_A] = {1.0000000000000000e+00,  -4.2095242957829111e+00,   7.1675248182208104e+00,  -6.1618895570204373e+00,   2.6728563552136859e+00,  -4.6785122274206487e-01};
int const c[NUM_CHANNELS] = {1};

BLOODPRESSURE::BLOODPRESSURE() { 
	// initialisation of BloodPressure class
}

bp BLOODPRESSURE::getPressure(uint16_t *korotkoff, uint16_t *cuff, int len){
	// system will supply reading from the ADC for korotkoff sound and cuff pressure
	// the data are raw 16 bit data from the ADC
	
	bp calculatedPressure;

	for (int i=0; i< len; i++){
		Serial.printf("korotkoff=%d, cuff=%d\r\n", korotkoff[i], cuff[i]);
	}
	
	// perform blood pressure calculation here
	Filter* filter = 0;
	filter = new FilterImpl();

#ifdef FILTER_TESTING

	Serial.print("Heap before malloc: ");
	Serial.println(ESP.getFreeHeap());
	filter_dt* filter_input = new filter_dt[TESTING_FILE_LENGTH];
	Serial.print("Heap after malloc: ");
	Serial.println(ESP.getFreeHeap());

	filter_dt* filter_buff = NULL; //new filter_dt[TESTING_FILE_LENGTH];
	// spiffs_dataread(filter_input);

	for (int line_counter = 0; line_counter < TESTING_FILE_LENGTH; ++line_counter) {
		Serial.printf("%f\n", filter_input[line_counter]);
		NOP();
	}
#else
	filter_dt* filter_input = (filter_dt*) cuff;
	filter_dt* filter_buff = new filter_dt[len];

	filter->rfilter(filter_input, filter_buff, channel_count, (double*) filter_a, FILTER_NUM_A, (double*) filter_b, FILTER_NUM_B, (int*) c, NUM_CHANNELS);
	// fflush(filter_buff);

	filter_dt *filter_output = new filter_dt[len];
	filter->rfilter(filter_buff, filter_output, NUM_CHANNELS, (double*) filter_a, FILTER_NUM_A, (double*) filter_b, FILTER_NUM_B, (int*) c, NUM_CHANNELS);
	// fflush(output);

#endif
	delete(filter_buff);
	delete(filter_input);
	
	calculatedPressure = { 120, 80};	// this is a fake value , return -1 for both value if error
	return calculatedPressure;
}

// Naive implementation of filtfilt
double my_filtfilt(double *b, double *a, uint16_t *input){

  return 0;
}
