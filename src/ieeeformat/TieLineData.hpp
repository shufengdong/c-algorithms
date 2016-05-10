#ifndef _CPSCPU_TIELINEDATA_H_
#define _CPSCPU_TIELINEDATA_H_

#include <string>

using namespace std;

namespace cpscpu {
	class TieLineData {
	public:
		int meteredBusNum;
		int meteredAreaNum;
		int nonmeteredBusNum;
		int nonmeteredAreaNum;
		int circuitNum;

	public:
		void parseString(string str);

		void toString(char * buffer);
	};

}//end of namespace

#endif
