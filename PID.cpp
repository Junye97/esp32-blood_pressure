#include "PID.h"
#include <Arduino.h>

PID::PID() { 
	// initialisation of PID class
}

void PID::setPID(double pGain, double iGain, double dGain){
	// setting up of PID gains
	Serial.printf("pGain=%f, iGain=%f, dGain=%f\r\n", pGain, iGain, dGain);
}

int16_t PID::getPID(double targetPressure, double currentPressure){

	int16_t pid=0;
	
	Serial.printf("target pressure=%f, current pressure=%f\r\n", targetPressure, currentPressure);

	// calculation here and return the value, the value must be the value to be set
	// at the DAC to control the PUMP and VALVE
	// POSITIVE value means value for the PUMP speed to pump air
	// NEGATIVE value means value for the VALVE opening to release air
	// both PUMP and DAC are controlled by their respective 12 bit DAC
	
	return pid;
	
}
