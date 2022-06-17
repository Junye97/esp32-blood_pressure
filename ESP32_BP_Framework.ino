#include "PID.h"
#include "BLOODPRESSURE.h"
//#include "FILTER.h"
#include "MYDAC.h"
#include "MYADC.h"

//#define PID_TESTING

#ifdef BP_TESTING
  #ifndef FILTER_TESTING
    #define FILTER_TESTING
  #endif
  #define NUM_BP_TEST_FILES 50
  int input_file_counter = 0;
#endif

#ifdef SD_TESTING
#include "MYSD.h"
mySD mySD_instance;
#endif 

PID myPID;
BLOODPRESSURE myBP;

//SPISettings PID_SPI_Settings;

//FilterImpl<double> myFlt;

#ifdef PID_TESTING
  myDAC myDAC_instance;
  uint16_t dac_input;
#endif

uint16_t cuff [] = {1,2,3,4,5};        // in real, the data will be around 500 x 90 seconds = 45000 datapoints
#ifndef FILTER_TESTING
  uint16_t korotkoff [] = {1,2,3,4,5};   // in real, the data will be around 500 x 90 seconds = 45000 datapoints
#else
  uint16_t* korotkoff = NULL;
#endif

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(115200);
  myPID.setPID(1,2,3);

#ifdef SD_TESTING
  mySD_instance.init();
#endif

#ifdef PID_TESTING
  myDAC_instance.init();
  dac_input = 0xffff;
//  Serial.printf("CS %d CLK %d MISO %d MOSI %d\n", SS, SLK, MISO, MOSI);
#endif

}

// the loop function runs over and over again forever
void loop() {

  uint16_t pidreturn = myPID.getPID(200,100);
//  Serial.printf("pidreturn=%d\r\n", pidreturn);
  int len = 5;

#ifdef FILTER_TESTING
#ifdef MEMORY_RW
  std::string input_filename;

#ifndef BP_TESTING
  input_filename = INPUT_FILE;
#else
  while (input_file_counter < NUM_BP_TEST_FILES) {
  input_filename = "/SBP_DBP_test/KS_" + std::to_string(++input_file_counter) + "_seg_1_uint16.dat";
#endif
  File input = (SD.open(&input_filename[0], FILE_READ));
  if (!input) Serial.printf("could not open input file '%s'\n", input_filename);
  len = input.size()/sizeof(filter_io);
  Serial.printf("Allocating memory for korotkoff\n");
  korotkoff = new uint16_t[len];
  Serial.printf("Reading the korotkoff...\n");
  if (input.read((uint8_t*) korotkoff, input.size()) == -1) {Serial.printf("could not read from input file\n"); exit;}
  input.close();
  Serial.print("Done reading!\n");
#endif 
  bp calculatedbp = myBP.getPressure(&korotkoff[0], &cuff[0], len);
  Serial.printf("Output written. Going idle.\n");
  }
  while (true) {} 
#endif

  // Serial.printf("Systolic=%f, Diastolic=%f\r\n", calculatedbp.systolic, calculatedbp.diastolic);
#ifdef PID_TESTING
  
  myDAC_instance.set_channel_output(0, dac_input);
//  while (true) {}
//  Serial.printf("DAC code = %x \n", dac_input);
  dac_input+= 0x33;
  delay(50);
//  if (dac_input >= (uint16_t) 0x1000)
//    dac_input = 0;
#endif 

}
