#ifndef __cpscpu_AbstractSM_H__
#define __cpscpu_AbstractSM_H__

#include <iostream>

using namespace std;

namespace cpscpu {

	class AbstractSM {
	public:
		int m;
		int n;
		int unzeroNum;

	public:
		AbstractSM(){m = 0; n = 0; unzeroNum = 0;};
		AbstractSM(int rows, int cols){};
		
		virtual ~AbstractSM() {};

		virtual void setValue(int i, int j, double value) = 0;		
		
		virtual void setValue(int i, int j, double value, bool isAddTo) = 0;

		virtual double getValue(int i, int j) = 0;

		virtual void getValues(double * values) = 0;

		virtual void initial(int row, int cols) = 0;
		
		virtual void getStructure(int * rows, int * cols) = 0;

		virtual void formMatrixByRow(int * unzeroNumEachRow, int * cols, double * values) = 0;

		virtual void formMatrixByCol(int * unzeroNumEachCol, int * rows, double * values) = 0;

		virtual void formValueByRow(double * values) = 0;

		virtual void formValueByCol(double * values) = 0;

		virtual void makeZero() = 0;

		//value add
		//virtual void add(AbstractSM * m, double w) = 0;
		
		virtual void printOnScreen() = 0;

		virtual void printFullMatrix(char* filepath);
	
		//virtual void freeSpace() = 0;

		//double rowSum(int row);		
		int getM(){return m;};
		int getN(){return n;};		
		int getUnzeroNum(){return unzeroNum;};		
	};
}//end of namespace

#endif
