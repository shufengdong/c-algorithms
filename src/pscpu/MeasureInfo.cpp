#include "MeasureInfo.h"
#include <sstream>
#include <fstream>
#include <iomanip>

#include <iostream>
#include <string>
using namespace cpscpu;
void MeasureInfo::parseString(string templine) {
	stringstream lineStream;
	lineStream<<templine;
	lineStream>>pointNum>>positionId>>measureType;
	lineStream>>value>>value_est>>value_true;
	lineStream>>weight>>sigma;
	lineStream>>upperLimit>>lowLimit;
};

void MeasureInfo::toString(char * buffer){
		sprintf(buffer, "%4d   %-11.11s   %2d  %9.4lf   %9.4lf   %9.4lf   %9.4lf   %9.4lf   %9.4lf   %9.4lf",pointNum, positionId.data(), measureType, value, value_est, value_true, weight, sigma, upperLimit, lowLimit); 
};
