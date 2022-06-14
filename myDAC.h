#ifndef MYDAC_H_
  #define MYDAC_H_
  #include <stdint.h>
 
  #ifndef _SPI_H_INCLUDED
    #include <SPI.h>
  #endif
  
  class myDAC {
    private: 
      SPIClass* _spi_instance;
     
      void _write_msg(uint8_t* input);
      
    public: 
      myDAC();
      void init(void);
      void set_channel_output(uint8_t channel, uint16_t value);
  };
  
#endif
  
