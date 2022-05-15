#include "MYSD.h"
#include <Arduino.h>

void(* resetFunc) (void) = 0;

mySD::mySD() {

}

//void mySD::init() { 
//	// initialisation of mySD class
//	Serial.print("\nInitializing SD card...");
//
//	// we'll use the initialization code from the utility libraries
//	// since we're just testing if the card is working!
//	if (!SD.begin(SD_CHIP_SEL)) {
//  	Serial.println("initialization failed. Things to check:");
//  	Serial.println("* is a card inserted?");
//  	Serial.println("* is your wiring correct?");
//  	Serial.println("* did you change the chipSelect pin to match your shield or module?");
//    Serial.println("Try again in 1 sec...");
//    delay(1000);
//    Serial.printf("Second try: %d\n", SD.begin(SD_CHIP_SEL));
////	while (1);
//	} else {
//	  Serial.println("Wiring is correct and a card is present.");
//	}
//
//	// print the type of card
//	Serial.println();
//	Serial.print("Card type:         ");
//	switch (SD.cardType()) {
//	case CARD_NONE:
//		Serial.println("NONE");
//		break;
//	case CARD_MMC:
//		Serial.println("MMC");
//		break;
//	case CARD_SD:
//		Serial.println("SD");
//		break;
//	case CARD_SDHC:
//		Serial.println("SDHC");
//		break;
//	default:
//		Serial.println("Unknown");
//	}
//
//	// print the type and size of the first FAT-type volume
//	//  uint32_t volumesize;
//	//  Serial.print("Volume type is:    FAT");
//	//  Serial.println(SDFS.usefatType(), DEC);
//
//	Serial.print("Card size:  ");
//	Serial.println((float)SD.cardSize()/1000);
//
//	Serial.print("Total bytes: ");
//	Serial.println(SD.totalBytes());
//
//	Serial.print("Used bytes: ");
//	Serial.println(SD.usedBytes());
//
//	File dir =  SD.open("/");
//	mySD::printDirectory(dir, 0);
//
//}

void mySD::init(){

  //Set pins for Micro SD Card on T8 board
  SPI.begin(14, 2, 15, 13); //SCK, MISO, MOSI,SS
  delay(20);
  if (!SD.begin(13, SPI)){
    Serial.printf("Failed to mount SD\n");
    resetFunc();
    while(true);
  }
  
  // print directory before creating a test file
//  dir = SD.open("/");
//  printDirectory(dir, 0);
  
  // create a test file
//  myfile = SD.open("/ttt.txt", FILE_WRITE);
//  if (myfile) {
//    Serial.println();
//    Serial.println("Writing to file....");
//    myfile.println("this is a test");
//    myfile.close();
//    Serial.println("done");
//    Serial.print(myfile.size());
//    Serial.println();
//  }
  
  // print directory after creating a test file
//  dir = SD.open("/");
//  printDirectory(dir, 0);
//  
//  // read file
//  readmyfile = SD.open("/ttt.txt", FILE_READ);
//  if(readmyfile) {
//    Serial.print("filesize=");
//    Serial.println(readmyfile.size());
//    
//    while(readmyfile.available()) {
//      Serial.write(readmyfile.read());
//    }
//  }
}

void mySD::printDirectory(File dir, int depth) {
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry)
    break;
    
    for (uint8_t i = 0; i < depth; i++)
    Serial.print("|    ");
    
    std::string name = entry.name();
    if (entry.isDirectory()) {
      Serial.print("+----" + String(name.substr(name.find_last_of("/\\") + 1).c_str()) + "\r\n");
      printDirectory(entry, depth + 1);
      } else {
      Serial.print("+--" + String(name.substr(name.find_last_of("/\\") + 1).c_str()));
      Serial.print("\t\t\t(");
      Serial.print(entry.size(), DEC);
      Serial.println(")");
    }
    entry.close();
  }
}

void mySD::createDirs(std::string dirs) {
  std::string dir = "";
  int count = 0;
  for (int i = 0; i < dirs.length(); i++) {
    dir.append(1, dirs[i]);
    count++;
    if (dirs[i] == '/') {
      if (dir.length() > 0)
      SD.mkdir(dir.substr(0, dir.length() - 1).c_str());
      count = 0;
    }
  }
  if (count > 0)
    SD.mkdir(dir.c_str());
}
