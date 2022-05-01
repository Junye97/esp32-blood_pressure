#include "PID.h"
#include "BLOODPRESSURE.h"
#include "FILTER.h"

#ifdef SD_TESTING
//  #include "MYSD.h"
#endif 

PID myPID;
BLOODPRESSURE myBP;
mySD mySD_instance;

//FilterImpl<double> myFlt;

uint16_t korotkoff [] = {1,2,3,4,5};   // in real, the data will be around 500 x 90 seconds = 45000 datapoints
uint16_t cuff [] = {1,2,3,4,5};        // in real, the data will be around 500 x 90 seconds = 45000 datapoints

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(115200);
  myPID.setPID(1,2,3);

#ifdef SD_TESTING
  mySD_instance.init();
#endif
 
}

// the loop function runs over and over again forever
void loop() {

  uint16_t pidreturn = myPID.getPID(200,100);
  Serial.printf("pidreturn=%d\r\n", pidreturn);

  bp calculatedbp = myBP.getPressure(&korotkoff[0], &cuff[0], 5);
  // Serial.printf("Systolic=%f, Diastolic=%f\r\n", calculatedbp.systolic, calculatedbp.diastolic);
  Serial.printf("Output written. Going idle.\n");
  while (true) {} 
//  delay(2000);
}
