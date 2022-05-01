#include "BLOODPRESSURE.h"
#include "FILTER.h"
#include <Arduino.h>

#define FILTER_NUM_B 6 // Num of filter coeff tabs
#define FILTER_NUM_A 6 
#define NUM_CHANNELS 1 // Support 1 channel filtfilt only
#define channel_count 1 // Variable name following original filtfilt

#ifdef FILTER_TESTING
	#define TESTING_FILE_LENGTH 0 //50250
	#define INPUT_FILE "/KS_double.dat"
	#define OUTPUT_FILE "/out.dat"
	#define TEMP_FILE "/temp.dat"

	// #include "SPIFFS.h"
	// #include <string>
	// #include <iostream>

	// void spiffs_dataread(filter_dt *buffer) {
	// 	if(!SPIFFS.begin(true)){
	// 		Serial.println("An Error has occurred while mounting SPIFFS");
	// 		return;
	// 	}
		
	// 	File file = SPIFFS.open(INPUT_FILE);
	// 	if(!file){
	// 		Serial.println("Failed to open file for reading");
	// 		return;
	// 	}
		
	// 	int line_counter = 0;
	// 	Serial.println("File Content:");
	// 	while((file.available()) & line_counter < TESTING_FILE_LENGTH){
	// 		// Serial.printf("%s\n", file.read());
	// 		// buffer[line_counter] = std::stod(file.read());
	// 		// Serial.printf("line_counter=%d\n", line_counter);
	// 		++line_counter;
	// 	}
	// 	file.close();
	// }

#endif


// Filter coefficients
filter_dt filter_b[FILTER_NUM_B] = {6.8515610836736729e-01,  -3.4028173297598676e+00,   6.7794474514361607e+00,  -6.7726741361536948e+00,   3.3926092540878621e+00,  -6.8172077928960340e-01};
filter_dt filter_a[FILTER_NUM_A] = {1.0000000000000000e+00,  -4.2095242957829111e+00,   7.1675248182208104e+00,  -6.1618895570204373e+00,   2.6728563552136859e+00,  -4.6785122274206487e-01};
int c[NUM_CHANNELS] = {1};

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

	int c[NUM_CHANNELS] = {1};
	filter_io input = (SD.open(INPUT_FILE, FILE_READ));
	if (!input) Serial.printf("could not open input file '%s'\n", INPUT_FILE);
	filter_io temp_out = SD.open(TEMP_FILE, FILE_WRITE);
	if (!temp_out) Serial.printf("could not create temporary file\n");
	filter_io output = (SD.open(OUTPUT_FILE, FILE_WRITE));
	if (!output) Serial.printf("could not create output file '%s'\n", OUTPUT_FILE);

	filter->rfilter(&input, &temp_out, channel_count, filter_a, FILTER_NUM_A, filter_b, FILTER_NUM_B, c, NUM_CHANNELS);
	temp_out.flush();
  temp_out.close();
  
  filter_io temp_in = SD.open(TEMP_FILE, FILE_READ);
  if (!temp_in) Serial.printf("could not load temporary file\n");
  
	filter->rfilter(&temp_in, &output, channel_count, filter_a, FILTER_NUM_A, filter_b, FILTER_NUM_B, c, NUM_CHANNELS);
	output.flush();

	input.close();
	temp_in.close();
	output.close();
	
#else
	filter_dt* filter_input = (filter_dt*) cuff;
	filter_dt* filter_buff = new filter_dt[len];

	filter->rfilter(filter_input, filter_buff, channel_count, filter_a, FILTER_NUM_A, filter_b, FILTER_NUM_B, (int*) c, NUM_CHANNELS);
	// fflush(filter_buff);

	filter_dt *filter_output = new filter_dt[len];
	filter->rfilter(filter_buff, filter_output, NUM_CHANNELS, filter_a, FILTER_NUM_A, filter_b, FILTER_NUM_B, (int*) c, NUM_CHANNELS);
	// fflush(output);

#endif
//	delete(filter_buff);
//	delete(filter_input);
	
	calculatedPressure = { 120, 80};	// this is a fake value , return -1 for both value if error
	return calculatedPressure;
}
