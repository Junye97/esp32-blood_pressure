#ifndef FILTER_H_
	#define FILTER_H_
	#include <stdint.h>
  #include <list> 
  #define filter_dt double // Data type definition for filter input/output

  template<typename T>
	void array_from_list(T*& array, int& size, const std::list<double>& list);

  class Filter {
  public:
    void rfilter(filter_dt* input, filter_dt* output, unsigned channel_count, double* const a, int na, double* const b, int nb, int* const c, int nc);
  };

  class FilterImpl : public Filter 
   {
   public:
    void rfilter(filter_dt* input, filter_dt* output, unsigned channel_count, double* const a, int na, double* const b, int nb, int* const c, int nc) {}
   };

#endif
