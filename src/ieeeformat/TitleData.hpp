#ifndef _CPSCPU_TITLEDATA_H_
#define _CPSCPU_TITLEDATA_H_

#include <string>

using namespace std;

namespace cpscpu {
	class TitleData {
	public:
		string date; //format: DD/MM/YY
		string originatorName;
		double mvaBase;
		int year;
		char season;
		string caseIdentification;

	public:
		void parseString(string str);

		void toString(char * buffer);
	};
}//end of namespace

#endif
