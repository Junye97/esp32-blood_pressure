#include "BLOODPRESSURE.h"
#include <Arduino.h>

#ifdef BP_TESTING
  #define BP_OUTPUT_FILE "/bp_output.txt"
  #define PEAKINDS_OUTPUT_FILE "/peaks_out.dat"
  #define BP_OUTPUT_PREFIX "/SBP_DBP_output/KS_"
#else 
  #define INPUT_FILE "/KS_uint16.dat"
  #define OUTPUT_FILE "/out.dat"
  #define ENERGY_OUTPUT_FILE "/output_energy.dat"
#endif
#define TEMP_FILE "/temp.dat"

// Constants and global variables

#define FILTER_NUM_B 6 // Num of filter coeff tabs
#define FILTER_NUM_A 6 
#define NUM_CHANNELS 1 // Support 1 channel filtfilt only
#define channel_count 1 // Variable name following original filtfilt
#define HAMMING_NUM_B 40 
#define HAMMING_NUM_A 1
#define HAMMING_SCALER (21.14)///4.096)

// Filter coefficients
filter_dt filter_b[FILTER_NUM_B] = {6.8515610836736729e-01,  -3.4028173297598676e+00,   6.7794474514361607e+00,  -6.7726741361536948e+00,   3.3926092540878621e+00,  -6.8172077928960340e-01};
filter_dt filter_a[FILTER_NUM_A] = {1.0000000000000000e+00,  -4.2095242957829111e+00,   7.1675248182208104e+00,  -6.1618895570204373e+00,   2.6728563552136859e+00,  -4.6785122274206487e-01};
//int c[NUM_CHANNELS] = {1};

filter_dt hamming_b[HAMMING_NUM_B] = {0.0800000000000000,0.0859568791865601,0.103673236704313,0.132690228199523,0.172256328834389,0.221346797385584,0.278690216503668,0.342801421754595,0.412019966598432,
  0.484553127082551,0.558522332450331,0.632011819136981,0.703118248039567,0.770000000000000,0.830924872773874,0.884314944158707,0.928787439350146,0.963190544083059,0.986633236015984,
  0.998508361741737,0.998508361741737,0.986633236015984,0.963190544083059,0.928787439350146,0.884314944158707,0.830924872773874,0.770000000000000,0.703118248039567,0.632011819136981,
  0.558522332450331,0.484553127082551,0.412019966598432,0.342801421754595,0.278690216503668,0.221346797385584,0.172256328834389,0.132690228199523,0.103673236704313,0.0859568791865601,0.0800000000000000};
filter_dt hamming_a[HAMMING_NUM_A] = {1};

int c[NUM_CHANNELS] = {1};

int bp_file_counter = 0;

Filter* filter = new FilterImpl();

// Local functions
//static void calculate_energy(filter_io* input, filter_io* output, int len);
      
BLOODPRESSURE::BLOODPRESSURE() { 
	// initialisation of BloodPressure class
}

bp BLOODPRESSURE::getPressure(uint16_t *korotkoff, uint16_t *cuff, int len){
	// system will supply reading from the ADC for korotkoff sound and cuff pressure
	// the data are raw 16 bit data from the ADC
	
	bp calculatedPressure;

//	for (int i=0; i< 5; i++){
//		Serial.printf("korotkoff=%d, cuff=%d\r\n", korotkoff[i], cuff[i]);
//	}
 
	// perform blood pressure calculation here

#ifdef FILTER_TESTING

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

#if 1
  // Casting unsigned int to signed int
  for (off_t counter = 0; counter < len; counter++){
    korotkoff[counter] = korotkoff[counter] >> 1; 
  }
	filter_io* filter_input = (filter_io*) korotkoff;
#else
  filter_io* filter_input = new filter_io[len];
  for (off_t counter = 0; counter < len; counter++){
    filter_input[counter] = (filter_io) korotkoff[counter];
  }
  delete(korotkoff);
#endif

	filter_io* filter_buff = new filter_io[len];
  memset(filter_buff, 0, sizeof(filter_io)*len);

	filter->rfilter(filter_input, filter_buff, len, channel_count, filter_a, FILTER_NUM_A, filter_b, FILTER_NUM_B, c, NUM_CHANNELS);
//  Serial.printf("Done forward filtering. Deleting input memory now\n");
	delete(korotkoff);

//  Serial.printf("Initialising output and backward filtering\n");
	filter_io* filter_output = new filter_io[len];
  memset(filter_output, 0, sizeof(filter_io)*len);
  
	filter->rfilter(filter_buff, filter_output, (int) len, (unsigned) channel_count, filter_a, FILTER_NUM_A, filter_b, FILTER_NUM_B, c, NUM_CHANNELS);
  delete(filter_buff);
  Serial.printf("Done filtfilt\n");
  
  std::string output_filename, energy_output_filename, peak_output_filename;

#ifndef BP_TESTING
  output_filename= OUTPUT_FILE;
  energy_output_filename = ENERGY_OUTPUT_FILE;
#else
  
  output_filename = BP_OUTPUT_PREFIX + std::to_string(++bp_file_counter) + "_seg_1_int16.dat";
  energy_output_filename = BP_OUTPUT_PREFIX + std::to_string(bp_file_counter) + "_seg_1_energy_int16.dat";
  peak_output_filename = BP_OUTPUT_PREFIX + std::to_string(bp_file_counter) + "_seg_1_peakinds_int.dat";
  Serial.printf("Now onto file: %d\n", bp_file_counter);
#endif

  File output = (SD.open(&output_filename[0], FILE_WRITE));
  if (output.write((uint8_t*)(filter_output), len*sizeof(filter_io)) != len*sizeof(filter_io)) {Serial.printf("could not write to output file\n"); exit;}
  output.flush();
  output.close();

  Serial.printf("Done writing output. Calculating the energy\n");
  filter_io* energy_output = new filter_io[len];
  memset(energy_output, 0, sizeof(filter_io)*len);

  BLOODPRESSURE::calculate_energy(filter_output, energy_output, len);
  delete(filter_output);

  File energy_file_output = (SD.open(&energy_output_filename[0], FILE_WRITE));
  if (energy_file_output.write((uint8_t*)(energy_output), len*sizeof(filter_io)) != len*sizeof(filter_io)) {Serial.printf("could not write to output file\n"); exit;}
  energy_file_output.flush();
  energy_file_output.close();
  
#ifdef BP_TESTING
  Serial.printf("Calculating SBP\n");
  std::vector<int> peak_inds;
  std::vector<peakFinder_dt> x0;
  for (int count = 0; count < len; count++)
    x0.push_back(energy_output[count]);
  delete(energy_output);

  Serial.printf("x0 size: %d, first few elems: %d %d %d\n", x0.size(), x0[0],x0[1],x0[2]);
  
  filter_io SBP_output = BLOODPRESSURE::find_SBP(&x0, (filter_io*) cuff, len, peak_inds);
  Serial.printf("Peak_inds size: %d", peak_inds.size());
  File peakinds_file_output = (SD.open(&peak_output_filename[0], FILE_WRITE));
  if (peakinds_file_output.write((uint8_t*)(&peak_inds[0]), peak_inds.size()*sizeof(int)) != peak_inds.size()*sizeof(int)) {Serial.printf("could not write to output file\n"); exit;}
  peakinds_file_output.flush();
  peakinds_file_output.close();
  Serial.printf("SBP: %d\n", SBP_output);
#endif
  
#endif
#endif 

	calculatedPressure = {120, 80};	// this is a fake value , return -1 for both value if error
	return calculatedPressure;
}

void BLOODPRESSURE::calculate_energy(filter_io* input, filter_io* output, int len) {
  filter_io* buff = new filter_io[len];
  memset(buff, 0, sizeof(filter_io)*len);
  memset(output, 0, sizeof(filter_io)*len);
  memcpy(buff, input, sizeof(filter_io)*len);
  filter_dt curr_value = 0;

  // Square and sqrt
  for (int count=0;count<len;count++){
    curr_value = pow(((filter_dt) buff[count])/INT16_SCALER, 2);
    buff[count] = (filter_io) (sqrt(curr_value)*INT16_SCALER);
//    if (buff[count] < 0)
//      Serial.printf("output buff[%d] = %d; curr_input = %d\n,", count, buff[count], curr_value);
  }

  // Scale hamming window coeffs to prevent overflow
  filter_dt curr_hamming_b[HAMMING_NUM_B];
  for (int ind = 0; ind < HAMMING_NUM_B; ind++) {
    curr_hamming_b[ind] = hamming_b[ind]/HAMMING_SCALER;
  }
  for (int ind = 0; ind < HAMMING_NUM_A; ind++)
    hamming_a[ind] = hamming_a[ind];

  filter->rfilter(buff, output, (int) len, (unsigned) channel_count, hamming_a, HAMMING_NUM_A, curr_hamming_b, HAMMING_NUM_B, c, NUM_CHANNELS);
  memcpy(buff, output, sizeof(filter_io)*len);
  memset(output, 0, sizeof(filter_io)*len);
  filter->rfilter(buff, output, (int) len, (unsigned) channel_count, hamming_a, HAMMING_NUM_A, curr_hamming_b, HAMMING_NUM_B, c, NUM_CHANNELS);
  delete(buff);
}

filter_io BLOODPRESSURE::find_SBP(std::vector<peakFinder_dt>* x0_input, filter_io* CP, int input_len, std::vector<int>& peak_inds){
  #define THRESHOLD_FACTOR 10
  #define SEL_FACTOR 8
//  PeakFinder::findPeaks(std::vector<peakFinder_dt> x0, std::vector<int>& peakInds, peakFinder_dt sel, peakFinder_dt threshold, bool includeEndpoints=true, peakFinder_dt extrema=POSITIVE_PEAKS);

//  std::vector<peakFinder_dt> *x0 = KS;
//  std::copy(std::begin(input), std::end(input), std::back_inserter(x0));
//  for (int count = 0; count < input_len; count++)
//    x0[count] = KS[count];
  std::vector<peakFinder_dt> &x0 = *x0_input;
  
  int minIdx = distance(x0.begin(), min_element(x0.begin(), x0.end()));
  int maxIdx = distance(x0.begin(), max_element(x0.begin(), x0.end()));
  peakFinder_dt default_sel = (x0[maxIdx]-x0[minIdx])/SEL_FACTOR; // default sel
  peakFinder_dt sel = default_sel;

  // Assuming we are applying peak detection on the energy signal
  peakFinder_dt threshold = (x0[maxIdx]-x0[minIdx]) / THRESHOLD_FACTOR;
  
  PeakFinder::findPeaks(x0, peak_inds, sel, threshold, false, MAXIMA);

  int SBP_ind = peak_inds[distance(peak_inds.begin(), max_element(peak_inds.begin(), peak_inds.end()))];
  // Now take the last peak CP as the last peak SBP
//  return CP[SBP_ind];
  return 0;
}
