#ifndef _CPSCPU_BUS_H_
#define _CPSCPU_BUS_H_

#include <string>

using namespace std;

namespace cpscpu {
	class BusData {
	public:
		int busNumber;
	    
		string name;
		int area;
		int lossZone;
		int busType;
		double finalVoltage;
		double finalAngle;
		double loadP;
		double loadQ;
		double generationP;
		double generationQ;
		double voltageBase;
		double desireV;
		double maximum;
		double minimum;
		double G;
		double B;
		int remoteControlBus;

	public:
		BusData() {
			busType = 0;
		};

		void parseString(string templine);

		void toString(char * buffer);
	};
}

#endif
