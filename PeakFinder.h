// PeakFinder.h
#ifndef PEAKFINDER_H
  #define PEAKFINDER_H
  
  #include <stdint.h>
  #include <vector>
  
  #define peakFinder_dt int16_t

  #define MAXIMA 1
  #define MINIMA -1
  
  namespace PeakFinder {
//      const float peak_EPS = 2.2204e-16f;
      const peakFinder_dt peak_EPS = -1;
  
      /*
          Inputs
          x0: input signal        
          extrema: 1 if maxima are desired, -1 if minima are desired
          includeEndpoints - If true the endpoints will be included as possible extrema otherwise they will not be included
          Output
          peakInds: Indices of peaks in x0
      */
      void findPeaks(std::vector<peakFinder_dt> x0, std::vector<int>& peakInds, peakFinder_dt sel, peakFinder_dt threshold, bool includeEndpoints=true, peakFinder_dt extrema=MAXIMA);
  }

#endif
