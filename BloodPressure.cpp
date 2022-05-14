#include "BLOODPRESSURE.h"
#include "FILTER.h"
#include <Arduino.h>

#define FILTER_NUM_B 6 // Num of filter coeff tabs
#define FILTER_NUM_A 6 
#define NUM_CHANNELS 1 // Support 1 channel filtfilt only
#define channel_count 1 // Variable name following original filtfilt

#ifdef FILTER_TESTING
	#define TESTING_FILE_LENGTH 0 //50250
#endif

// Filter coefficients
filter_dt filter_b[FILTER_NUM_B] = {6.8515610836736729e-01,  -3.4028173297598676e+00,   6.7794474514361607e+00,  -6.7726741361536948e+00,   3.3926092540878621e+00,  -6.8172077928960340e-01};
filter_dt filter_a[FILTER_NUM_A] = {1.0000000000000000e+00,  -4.2095242957829111e+00,   7.1675248182208104e+00,  -6.1618895570204373e+00,   2.6728563552136859e+00,  -4.6785122274206487e-01};
//int c[NUM_CHANNELS] = {1};

BLOODPRESSURE::BLOODPRESSURE() { 
	// initialisation of BloodPressure class
}

bp BLOODPRESSURE::getPressure(uint16_t *korotkoff, uint16_t *cuff, int len){
	// system will supply reading from the ADC for korotkoff sound and cuff pressure
	// the data are raw 16 bit data from the ADC
	
	bp calculatedPressure;

	for (int i=0; i< 5; i++){
		Serial.printf("korotkoff=%d, cuff=%d\r\n", korotkoff[i], cuff[i]);
	}
	
	// perform blood pressure calculation here
	Filter* filter = 0;
	filter = new FilterImpl();

	int c[NUM_CHANNELS] = {1};

#ifndef MEMORY_RW

	filter_io input = (SD.open(INPUT_FILE, FILE_READ));
	if (!input) Serial.printf("could not open input file '%s'\n", INPUT_FILE);
	filter_io temp_out = SD.open(TEMP_FILE, FILE_WRITE);
	if (!temp_out) Serial.printf("could not create temporary file\n");
	filter_io output = (SD.open(OUTPUT_FILE, FILE_WRITE));
	if (!output) Serial.printf("could not create output file '%s'\n", OUTPUT_FILE);

	filter->rfilter(&input, &temp_out, len, (unsigned) channel_count, filter_a, FILTER_NUM_A, filter_b, FILTER_NUM_B, c, NUM_CHANNELS);
	temp_out.flush();
  temp_out.close();
  
  filter_io temp_in = SD.open(TEMP_FILE, FILE_READ);
  if (!temp_in) Serial.printf("could not load temporary file\n");
	filter->rfilter(&temp_in, &output, len, (unsigned) channel_count, filter_a, FILTER_NUM_A, filter_b, FILTER_NUM_B, c, NUM_CHANNELS);

	output.flush();
	input.close();
	temp_in.close();
	output.close();
	
#else

  // Casting unsigned int to signed int
  for (off_t counter = 0; counter < len; counter++){
    korotkoff[counter] = korotkoff[counter] >> 1; 
  }
	filter_io* filter_input = (filter_io*) korotkoff;
	filter_io* filter_buff = new filter_io[len];
  memset(filter_buff, 0, sizeof(filter_io)*len);

	filter->rfilter(filter_input, filter_buff, len, channel_count, filter_a, FILTER_NUM_A, filter_b, FILTER_NUM_B, c, NUM_CHANNELS);
  Serial.printf("Done forward filtering. Deleting input memory now\n");
	delete(korotkoff);

  Serial.printf("Initialising output and backward filtering\n");
	filter_io* filter_output = new filter_io[len];
  memset(filter_output, 0, sizeof(filter_io)*len);
  
	filter->rfilter(filter_buff, filter_output, (int) len, (unsigned) channel_count, filter_a, FILTER_NUM_A, filter_b, FILTER_NUM_B, c, NUM_CHANNELS);
  delete(filter_buff);
  Serial.printf("Done backward filtering. Writing output now\n");
  
  File output = (SD.open(OUTPUT_FILE, FILE_WRITE));
  if (output.write((uint8_t*)(filter_output), len*sizeof(filter_io)) != len*sizeof(filter_io)) {Serial.printf("could not write to output file\n"); exit;}
  output.flush();
  output.close();
  delete(filter_output);
  
#endif
	
	calculatedPressure = {120, 80};	// this is a fake value , return -1 for both value if error
	return calculatedPressure;
}
