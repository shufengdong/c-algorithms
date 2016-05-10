#ifndef __cpscpu_MapSparseMatrix_H__
#define __cpscpu_MapSparseMatrix_H__

#include "AbstractSM.h"
#include <map>
using namespace std;

namespace cpscpu {

	class MapSparseMatrix;

	class MapSparseMatrix : public AbstractSM {
	public:
		std::map<int,double> dataMap;
		std::map<int,double>::iterator iter;

	public:
		MapSparseMatrix();
		MapSparseMatrix(int rows, int cols);
		~MapSparseMatrix();
		
		//virtual methods defined in father class
		void setValue(int i, int j, double value, bool isAddTo);
	
		void setValue(int i, int j, double v);

		double getValue(int i, int j);

		void initial(int row, int cols);		
	
		void printOnScreen();

		void getStructure(int * rows, int * cols);

		void formMatrixByRow(int * unzeroNumEachRow, int * cols, double * values);

		void formMatrixByCol(int * unzeroNumEachCol, int * rows, double * values);

		void formValueByRow(double * values);

		void formValueByCol(double * values);

		void getValues(double * values);

		void makeZero();

		void freeSpace();

	};
}//end of namespace

#endif
