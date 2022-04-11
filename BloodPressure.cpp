#include "BLOODPRESSURE.h"
#include "FILTER.h"
#include <Arduino.h>

#define FILTER_NUM_B 6 // Num of filter coeff tabs
#define FILTER_NUM_A 6 
#define NUM_CHANNELS 1 // Support 1 channel filtfilt only
#define channel_count 1 // Variable name following original filtfilt

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
	filter_dt *filter_input = (filter_dt*) cuff;
	filter_dt *filter_buff = new filter_dt[len];

	filter->rfilter(filter_input, filter_buff, channel_count, (double*) filter_a, FILTER_NUM_A, (double*) filter_b, FILTER_NUM_B, (int*) c, NUM_CHANNELS);
	// fflush(filter_buff);

	filter_dt *filter_output = new filter_dt[len];
	filter->rfilter(filter_buff, filter_output, NUM_CHANNELS, (double*) filter_a, FILTER_NUM_A, (double*) filter_b, FILTER_NUM_B, (int*) c, NUM_CHANNELS);
	// fflush(output);

	delete(filter_buff);

	calculatedPressure = { 120, 80};	// this is a fake value , return -1 for both value if error
	return calculatedPressure;
}

// Naive implementation of filtfilt
double my_filtfilt(double *b, double *a, uint16_t *input){

  return 0;
}
