#ifndef BLOODPRESSURE_H_
	#define BLOODPRESSURE_H_
	#include <stdint.h>
	
	struct bp
	{
		double systolic;
		double diastolic;
	};
	
	class BLOODPRESSURE {
		private: 
			

		public: 
			BLOODPRESSURE();
			bp getPressure(uint16_t *korotkoff, uint16_t *cuff, int len);
	};
	
#endif