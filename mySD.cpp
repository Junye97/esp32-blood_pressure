#include "MYSD.h"
#include <Arduino.h>

void(* resetFunc) (void) = 0;

mySD::mySD() {

}

void mySD::init(){

  //Set pins for Micro SD Card on T8 board
  SPI.begin(14, 2, 15, 13); //SCK, MISO, MOSI,SS
  delay(100);
//  int load_trial_max = 10;
//  int curr_trial = 0;
  while (!SD.begin(13, SPI)){
    Serial.printf("Failed to mount SD\n");
//    if (curr_trial >= load_trial_max)
//      resetFunc();
//    curr_trial++;
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
