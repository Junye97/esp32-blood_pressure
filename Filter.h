#ifndef FILTER_H_
	#define FILTER_H_
	#include <stdint.h>
  #include <list> 

  #define FILTER_TESTING

#ifdef FILTER_TESTING
  #define SD_TESTING
  #include "MYSD.H"
	// #include "SPIFFS.h"
#endif

  #define filter_io File // Filter IO file type. 
  #define filter_dt double // Data type definition for filter data

  template<typename T>
	void array_from_list(T*& array, int& size, const std::list<double>& list);

  class Filter {
  public:
    void rfilter(filter_io* input, filter_io* output, unsigned channel_count, filter_dt* const a, int na, filter_dt* const b, int nb, int* const c, int nc);
#ifdef FILTER_TESTING
    void filter_io_handle(filter_io* buff);
#endif
  };

  class FilterImpl : public Filter 
   {
   public:
    void rfilter(filter_io* input, filter_io* output, unsigned channel_count, filter_dt* const a, int na, filter_dt* const b, int nb, int* const c, int nc) {}
#ifdef FILTER_TESTING
    void filter_io_handle(filter_io* buff) {} 
#endif
   private:
    // void dt_to_byte(filter_dt input) {}
   };



#endif
