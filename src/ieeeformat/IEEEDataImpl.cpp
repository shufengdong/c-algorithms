#include "BusData.hpp"
#include "BranchData.hpp"
#include "TitleData.hpp"
#include "LossZoneData.hpp"
#include "TieLineData.hpp"
#include "InterchangeData.hpp"
#include <string>
#include <stdio.h>

using namespace std;
using namespace cpscpu;

void BusData::parseString(string templine) {
	sscanf(templine.substr(0,4).data(),"%d",&busNumber);
	
	name = templine.substr(5,12);
	
	sscanf(templine.substr(18,2).data(),"%d",&area);

	sscanf(templine.substr(20,3).data(),"%d",&lossZone);

	sscanf(templine.substr(24,2).data(),"%d",&busType);

	sscanf(templine.substr(27,6).data(),"%lf",&finalVoltage);
    
	sscanf(templine.substr(33,7).data(),"%lf",&finalAngle);
    
	sscanf(templine.substr(40,9).data(),"%lf",&loadP);

	sscanf(templine.substr(49,10).data(),"%lf",&loadQ);
    
	sscanf(templine.substr(59,8).data(),"%lf",&generationP);
    
	sscanf(templine.substr(67,8).data(),"%lf",&generationQ);
    
	sscanf(templine.substr(76,7).data(),"%lf",&voltageBase);
    
	sscanf(templine.substr(84,6).data(),"%lf",&desireV);
	
	sscanf(templine.substr(90,8).data(),"%lf",&maximum);
	
	sscanf(templine.substr(98,8).data(),"%lf",&minimum);
    
	sscanf(templine.substr(106,8).data(),"%lf",&G);
	
	sscanf(templine.substr(114,8).data(),"%lf",&B);
	
	sscanf(templine.substr(123,4).data(),"%d",&remoteControlBus);
}

void BusData::toString(char * buffer) {			
	sprintf(buffer, "%4d %-12.12s %2d%3d %2d %6.3lf%7.2lf%9.3lf%10.3lf%8.3lf%8.3lf %7.2lf %6.3lf%8.1lf%8.1lf%8.4lf%8.4lf %4d", 
		busNumber, name.data(),area, lossZone, busType, finalVoltage, finalAngle, loadP, loadQ, generationP, generationQ, voltageBase,
		desireV, maximum, minimum, G, B, remoteControlBus); 
}

void BranchData::parseString(string templine){			

	sscanf(templine.substr(0,4).data(),"%d",&tapBusNumber);	

	sscanf(templine.substr(5,4).data(),"%d",&zBusNumber);

	sscanf(templine.substr(10,2).data(),"%d",&area);
    
	lossZone = 0;
	sscanf(templine.substr(12,2).data(),"%d",&lossZone);

	circuit = 1;
	sscanf(templine.substr(16,1).data(),"%d",&circuit);
    
	type = 0;
	sscanf(templine.substr(18,1).data(),"%d", &type);
    
	sscanf(templine.substr(19,10).data(),"%lf",&R);
    
	sscanf(templine.substr(29,11).data(),"%lf",&X);
    
	sscanf(templine.substr(40,10).data(),"%lf",&lineB);
    
	sscanf(templine.substr(50,5).data(),"%d",&mvaRating1);
    
	sscanf(templine.substr(55,6).data(),"%d",&mvaRating2);//note this, not equal as standard desribed.
    
	sscanf(templine.substr(61,6).data(),"%d",&mvaRating3);//note this, not equal as standard desribed. 

	sscanf(templine.substr(68,4).data(),"%d",&controlBusNumber);
    
	sscanf(templine.substr(73,1).data(),"%d",&side);
    
	sscanf(templine.substr(76,6).data(),"%lf",&transRatio);
    
	sscanf(templine.substr(83,7).data(),"%lf",&transAngle);
    
	sscanf(templine.substr(90,7).data(),"%lf",&minimumTap);
    
	sscanf(templine.substr(97,7).data(),"%lf",&maximumTap);
    
	sscanf(templine.substr(105,6).data(),"%lf",&stepSize);
    
	sscanf(templine.substr(112,7).data(),"%lf",&minimum);
    
	sscanf(templine.substr(119,7).data(),"%lf",&maximum);
}

void BranchData::toString(char * buffer) {			
	sprintf(buffer, "%4d %4d %2d%2d  %d %d%10.5lf%11.5lf%10.5lf%-5d %-5d %-5d %4d %d  %6.3lf %7.3lf%7.3lf%7.3lf %6.4lf %7.4lf%7.4lf", 
		tapBusNumber, zBusNumber, area, lossZone, circuit, type, R, X, lineB, 
		mvaRating1, mvaRating2, mvaRating3, controlBusNumber, side, 
		transRatio, transAngle, minimumTap, maximumTap, stepSize, minimum, maximum);				
}

void LossZoneData::parseString(string str) {
	sscanf(str.substr(0, 3).data(), "%d", &lossZoneNumber);

	lossZoneName = str.substr(4, 12);					
}

void LossZoneData::toString(char * buffer) {
	sprintf(buffer, "%3d %12.12s", lossZoneNumber, lossZoneName.data());	
}

void TitleData::parseString(string str) {
	date = str.substr(1,8);

	originatorName = str.substr(10, 20);

	sscanf(str.substr(31, 6).data(),"%lf",&mvaBase);
    
	sscanf(str.substr(38, 4).data(),"%d",&year);
    
	sscanf(str.substr(43, 1).data(),"%c",&season);

	caseIdentification = str.substr(45, 28);
}

void TitleData::toString(char * buffer) {			
	sprintf(buffer, " %8.8s %20.20s %6.2lf %4d %c %28.28s", date.data(), originatorName.data(), mvaBase, year, season, caseIdentification.data());
}

void InterchangeData::parseString(string str) {
	sscanf(str.substr(0, 2).data(), "%d", &areaNumber);

	sscanf(str.substr(3, 4).data(),"%d",&slackBusNumber);

	alternateSwingBusName = str.substr(8, 12);			
	
	sscanf(str.substr(20, 8).data(),"%lf",&areaExport);

	sscanf(str.substr(29, 6).data(),"%lf",&areaTolerance);
    
	areaCode = str.substr(37, 6);

	areaName = str.substr(45, 30);
}

void InterchangeData::toString(char * buffer) {			
	sprintf(buffer, "%2d %4d %12.12s%8.3lf %6.3lf  %6.6s  %30.30s",
		areaNumber, slackBusNumber, alternateSwingBusName.data(), areaExport, areaTolerance, areaCode.data(), areaName.data());
}

void TieLineData::parseString(string str) {
	sscanf(str.substr(0, 4).data(), "%d", &meteredBusNum);

	sscanf(str.substr(6, 2).data(),"%d",&meteredAreaNum);

	sscanf(str.substr(10, 4).data(), "%d", &nonmeteredBusNum);

	sscanf(str.substr(16, 2).data(),"%d",&nonmeteredAreaNum);		

	sscanf(str.substr(20, 1).data(),"%d",&circuitNum);		
}

void TieLineData::toString(char * buffer) {			
	sprintf(buffer, "%4d  %2d  %4d  %2d  %d",
		meteredBusNum, meteredAreaNum, nonmeteredBusNum, nonmeteredAreaNum, circuitNum);

}
