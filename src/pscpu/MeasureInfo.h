#ifndef __cpscpu_MeasureInfo_H__
#define __cpscpu_MeasureInfo_H__

#include <iostream>
#include <string>
using namespace std;

namespace cpscpu {
	class MeasureInfo {

	public:
		int pointNum;

		string positionId;

		int measureType;

		string type;

		double value;

		double value_est;

		double value_true;

		bool isEligible;

		int linePowerType;

		int powerType;

		double weight;

		double sigma ;

		double genMVA;

		string deviceId;

		double upperLimit;

		double lowLimit;

	public:
		  MeasureInfo() {};

		/**
		 * @param positionId id of position which measurement is located
		 * @param measureType measurement type defined in interface @com.ht.aems.measure.MeasureTypeConstants
		 * @param value measurement value
		 */
		MeasureInfo(string positionId, int measureType, double value) {
			this->positionId = positionId;
			this->measureType = measureType;
			this->value = value;
		};

		/**
		 * @parse a string to a MeasureInfo object
		 */
		void parseString(string templine);
		void toString(char * buffer);
	};
}//end of namespace

#endif
