#ifndef BLOODPRESSURE_H_
	#define BLOODPRESSURE_H_

  #define FILTER_TESTING
  #define BP_TESTING
  
	#include <stdint.h>
	#include "FILTER.h"
  #include "PEAKFINDER.h"
  
	struct bp
	{
		double systolic;
		double diastolic;
	};
	
	class BLOODPRESSURE {
		private: 
      void calculate_energy(filter_io* input, filter_io* output, int len);
      filter_io find_SBP(std::vector<peakFinder_dt>* x0_input, filter_io* CP, int input_len, std::vector<int>& peak_inds);
//      void SBP_DBP_estimation
		public: 
			BLOODPRESSURE();
			bp getPressure(uint16_t *korotkoff, uint16_t *cuff, int len);
	};
	
#endif
