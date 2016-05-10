#ifndef _CPSCPU_INTERCHANGEDATA_H_
#define _CPSCPU_INTERCHANGEDATA_H_

#include <string>

using namespace std;

namespace cpscpu {
	class InterchangeData {
	public:
		int areaNumber; //no zeros!
		int slackBusNumber;
		string alternateSwingBusName;
		double areaExport; //area interchange export, MW
		double areaTolerance; //area interchange tolerance, MW
		string areaCode;
		string areaName;

	public:
		void parseString(string str);

		void toString(char * buffer);
	};
}//end of namespace

#endif
