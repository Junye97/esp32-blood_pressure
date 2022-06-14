#ifndef MYADC_H_
  #define MYADC_H_
  #include <stdint.h>
 
  #ifndef _SPI_H_INCLUDED
    #include <SPI.h>
  #endif
  
  // ADC modes
  #define EXTERNAL_CLOCK 0
  #define EXTERNAL_ACQUISITION 1
  #define INTERNAL_CLOCK 2
  #define RESET 4
  #define PARTIAL_POWER_DOWN 6
  #define FULL_POWER_DOWN 7
  
  // ADC ranges
  // Format: SINGLE/DIFF_Vlow_Vhigh
  #define SINGLE_0_Vref 0b0110
  
  class myADC {
    private: 
      SPIClass* _spi_instance;
      void _write_msg(uint8_t* msg, int len);
      void _write_read_msg(uint8_t* msg, int len);
      
    public: 
      void init(void);
      void set_range_mode(int channel, int adc_range, int adc_mode);
      uint16_t read_channel(int channel, int adc_mode);
  };
  
#endif
  
