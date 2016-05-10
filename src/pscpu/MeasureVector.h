#ifndef __cpscpu_MeasureVector_H__
#define __cpscpu_MeasureVector_H__

#include"MeasureInfo.h"

using namespace std;
namespace cpscpu {
	class MeasureVector {
	public:	
		MeasureVector(){
			meas = NULL;
			measNum = -1;
		};

		~MeasureVector(){
			freeSpace();
		};

		void freeSpace(){
			if(meas != NULL)
				delete[] meas;
			meas = NULL;
			measNum = -1;
		};

		MeasureInfo * meas;
		int measNum;

		/**
		* @parse a file to a MeasureVector object
		*/
		void parseFile(string filePath);
		void writeFile(string filePath);
			 
	};
}

#endif
