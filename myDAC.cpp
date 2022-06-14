#include "MYDAC.h"
#include <Arduino.h>

#define DAC_CS 15
#define DAC_CLK 14
#define DAC_MISO 12
#define DAC_MOSI 13

#define DAC_MSG_LEN 3 // write 3 bytes a time

#define DAC_SCK_FREQ 200000

// List of DAC commands
#define CODEn_LOADn 0x3

#define DAC0 0x0
#define DAC1 0x1
#define DAC2 0x2
#define DAC3 0x3
#define DAC_ALL 0x4

static uint8_t msg[DAC_MSG_LEN];

//SPIClass myDAC::_spi_instance(HSPI);// = SPIClass(VSPI);

myDAC::myDAC() {
  // initialisation of myDAC class
}

void myDAC::_write_msg(uint8_t* input) {
//  assert(~

  myDAC::_spi_instance->beginTransaction(SPISettings(DAC_SCK_FREQ, MSBFIRST, SPI_MODE2)); 
  digitalWrite(myDAC::_spi_instance->pinSS(), LOW);
  myDAC::_spi_instance->writeBytes(input, DAC_MSG_LEN);
  digitalWrite(myDAC::_spi_instance->pinSS(), HIGH);
//  Serial.printf("Input: 0x%x 0x%x 0x%x\n", input[0], input[1], input[2]);
//  uint32_t to_send = (input[0] << 24 | input[1] << 16 | input[2] << 8);
//  myDAC::_spi_instance->write32(to_send);
//  Serial.printf("Wrote: %d 0x%x\n", to_send, to_send);
  
  myDAC::_spi_instance->endTransaction();
}

void myDAC::init(void) {
  myDAC::_spi_instance = new SPIClass(HSPI);
//  pinMode(DAC_CS, OUTPUT);
//  pinMode(DAC_MOSI, OUTPUT);
//  pinMode(DAC_CLK, OUTPUT);
//  pinMode(DAC_MISO, INPUT);
  
  myDAC::_spi_instance->begin(DAC_CLK, DAC_MISO, DAC_MOSI, DAC_CS); //SCK, MISO, MOSI,SS
//  pinMode(DAC_CS, OUTPUT);
  pinMode(myDAC::_spi_instance->pinSS(), OUTPUT); //HSPI SS
  
  delay(200);
  
  // Set the Vref to internal 4.096V
  msg[0] = 0x77;
  msg[1] = 0x00;
  msg[2] = 0x00;
  
  _write_msg(msg);
  delay(200);

  // Set all channels to 0V
  myDAC::set_channel_output(4, 0);
  delay(50);
//  Serial.printf("SPI Bus: %d\n", myDAC::_spi_instance->_spi_num);
//  spi_t* curr_bus = (myDAC::_spi_instance->bus());
//  assert(curr_bus != NULL);
//  Serial.printf("Current bus num: %d\n", curr_bus->num);
}


void myDAC::set_channel_output(uint8_t channel, uint16_t value){
  assert(channel <= 4 && channel >= 0);

  if (channel == 4){
    Serial.printf("Setting all DAC output to %d\n", value);
  }
//  uint8_t msg[DAC_MSG_LEN];
  msg[0] = (CODEn_LOADn << 4) | (channel);
  msg[1] = (value & 0x0ff0) >> 4;
  msg[2] = ((value & 0x000f) << 4);

  Serial.printf("To write: value= %d, 0x%x 0x%x 0x%x\n", value, msg[0], msg[1], msg[2]);
  _write_msg(msg);
}
