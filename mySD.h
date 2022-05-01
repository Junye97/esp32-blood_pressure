#ifndef MYSD_H_
	#define MYSD_H_
	// #include <stdint.h>
	#include <SD.h> 
  	#include <SPI.h>
	#include <FS.h>

	class mySD {
		private: 
			

		public: 
			mySD(); // class init
      		void init(); // hardware init
			void printDirectory(File dir, int depth); // Print directory
      		void createDirs(std::string dirs) ;
	};
	
#endif
