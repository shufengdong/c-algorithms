#ifndef __cpscpu_SparseMatrix_H__
#define __cpscpu_SparseMatrix_H__

#include "AbstractSM.h"

using namespace std;

namespace cpscpu {

	class SparseMatrix;

	struct Triple {int row, col; double value;};

	class MatrixNode {
		friend class SparseMatrix;
	public:
		Triple triple;

		MatrixNode * right;

		MatrixNode * down;
	};

	class SparseMatrix : public AbstractSM {
	public:
		MatrixNode* headNode;
		MatrixNode* colHeadNode;	

	public:
		SparseMatrix();
		SparseMatrix(int rows, int cols);
		~SparseMatrix();
		

		//virtual methods defined in father class
		double getValue(int i, int j);		
		
		void setValue(int i, int j, double value, bool isAddTo);
	
		void setValue(int i, int j, double v);
		
		void getValues(double * values);

		void initial(int row, int cols);		
	
		void printOnScreen();

		void getStructure(int * rows, int * cols);

		void formMatrixByRow(int * unzeroNumEachRow, int * cols, double * values);

		void formMatrixByCol(int * unzeroNumEachCol, int * rows, double * values);

		void formValueByRow(double * values);

		void formValueByCol(double * values);

		void makeZero();

		//this class' own methods
		void freeSpace();

		void add(SparseMatrix * m, double w); //value add
		
		double rowSum(int row);

	};
}//end of namespace

#endif
