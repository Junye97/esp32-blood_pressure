#ifndef PID_H_
	#define PID_H_
	#include <stdint.h>
 
//	#ifndef SPI_H_
//    #include <SPI.h>
//  #endif
  
	class PID {
		private: 

		public: 
			PID();
			void setPID(double pGain, double iGain, double dGain);
			int16_t getPID(double targetPressure, double currentPressure);
	};
	
#endif
