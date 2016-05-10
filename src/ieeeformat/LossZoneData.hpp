#ifndef _CPSCPU_LOSSZONEDATA_H_
#define _CPSCPU_LOSSZONEDATA_H_

#include <string>

using namespace std;

namespace cpscpu {
	class LossZoneData {	
	public:
		int lossZoneNumber;
		string lossZoneName;
	public:
		void parseString(string str);

		void toString(char * buffer);
	};
}//end of namespace

#endif
