#ifndef FILTER_H_
	#define FILTER_H_
	#include <stdint.h>
//  #include <list> 

  #define MEMORY_RW 
  
#ifdef FILTER_TESTING
  #define SD_TESTING
  #include "MYSD.H"
#endif

  #define filter_dt float // Data type definition for filter data

#ifndef MEMORY_RW
  #define filter_io File // Filter IO file type. 
#else
  #define filter_io int16_t
  #define INT16_SCALER (filter_dt) 8000.0
  #define UINT16_SCALER (filter_dt) 16000.0
#endif

//  template<typename T>
//	void array_from_list(T*& array, int& size, const std::list<double>& list);

  class Filter {
  public:
    void rfilter(filter_io* input, filter_io* output, int len, unsigned channel_count, filter_dt* const a, int na, filter_dt* const b, int nb, int* const c, int nc);
  };

  class FilterImpl : public Filter 
   {
   public:
    void rfilter(filter_io* input, filter_io* output, int len, unsigned channel_count, filter_dt* const a, int na, filter_dt* const b, int nb, int* const c, int nc) {}
   private:

   };

#endif
