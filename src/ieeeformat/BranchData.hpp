#ifndef _CPSCPU_BRANCHDATA_H_
#define _CPSCPU_BRANCHDATA_H_

#include <string>

using namespace std;

namespace cpscpu {
	class BranchData{
		public:
		int tapBusNumber;
		int zBusNumber;
		int area;
		int lossZone;
		int circuit;
		int type;
		double R;
		double X;
		double lineB;
		int mvaRating1;
		int mvaRating2;
		int mvaRating3;
		int controlBusNumber;
		int side;
		double transRatio;
		double transAngle;
		double minimumTap;
		double maximumTap;
		double stepSize;
		double minimum;
		double maximum;

		public:
		void parseString(string templine);
		void toString(char * buffer);
	};
}//end of namespace
#endif