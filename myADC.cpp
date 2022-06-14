#include "MYADC.h"
#include <Arduino.h>

/* ADC and DAC connection guide: use VSPI (SPI3) configuration,
    i.e. pin 18, 19, 23 for CLK, MISO, MOSI
    except for CS, which are defined as follows:
*/
#define ADC_CLK 18
#define ADC_MISO 19
#define ADC_MOSI 23
#define ADC_CS 34

#define ADC_MSG_LEN 2

#define ADC_SCK_FREQ 200000

static uint8_t msg[ADC_MSG_LEN];

void myADC::_write_msg(uint8_t* msg, int len) {
  _spi_instance->beginTransaction(SPISettings(ADC_SCK_FREQ, MSBFIRST, SPI_MODE0)); 
  digitalWrite(_spi_instance->pinSS(), LOW);
  _spi_instance->transferBytes(msg, msg, len);
  digitalWrite(_spi_instance->pinSS(), HIGH);
}

void myADC::_write_read_msg(uint8_t* msg, int len) {
  _spi_instance->beginTransaction(SPISettings(ADC_SCK_FREQ, MSBFIRST, SPI_MODE0)); 
  digitalWrite(_spi_instance->pinSS(), LOW);
  _spi_instance->writeBytes(msg, len);
  digitalWrite(_spi_instance->pinSS(), HIGH);
}

void myADC::init(void) {
  _spi_instance = new SPIClass(HSPI);
//  pinMode(DAC_CS, OUTPUT);
//  pinMode(DAC_MOSI, OUTPUT);
//  pinMode(DAC_CLK, OUTPUT);
//  pinMode(DAC_MISO, INPUT);
  
  _spi_instance->begin(ADC_CLK, ADC_MISO, ADC_MOSI, ADC_CS); //SCK, MISO, MOSI,SS
//  pinMode(DAC_CS, OUTPUT);
  pinMode(_spi_instance->pinSS(), OUTPUT); //HSPI SS
  delay(10);
    
  // Set the input range to mode 0110, 0V to +Vref range
  // We can go with the default conversion method (External Clock mode) 
  myADC::set_range_mode(0, SINGLE_0_Vref, EXTERNAL_CLOCK);
  
}

void myADC::set_range_mode(int channel, int adc_range, int adc_mode) {
  msg[0] = 0x80 | (channel << 4) | adc_range;
  myADC::_write_msg(msg, 1);
  msg[0] = 0x88 | (adc_mode << 4);
  myADC::_write_msg(msg, 1);
}

uint16_t myADC::read_channel(int channel, int adc_mode){
  switch(adc_mode){
    case EXTERNAL_CLOCK:
      msg[1] = 0;
      msg[0] = 0x80 | (channel << 4);
      myADC::_write_read_msg(msg, 2);
      break;
    default:
      break;
  }
  
  return (((uint16_t) msg[0]) << 8 | msg[1]);
}
