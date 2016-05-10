
#ifndef __YMatrix_H__
#define	__YMatrix_H__

#include"IEEEDataIsland.hpp"
#include"SparseMatrix.h"

using namespace std;

namespace cpscpu {
	class YMatrix {
	public:

		YMatrix();

		~YMatrix() {
			freeSpace();
		};

		SparseMatrix admittance[2];	 //	G:admittance[0];	B:admittance[1];

		int * lineFrom;

		int * lineTo;

		double * lineFromAdmittance;

		double * lineToAdmittance;

		void formMatrix(IEEEDataIsland *island);


	private:
		void freeSpace();

		void getBranchAdmittance(BranchData, double[][4]);

		void getBranchAdmittanceFrom(BranchData branch, double v[4]);

		void getBranchAdmittanceTo(BranchData branch, double v[4]);
	};
}//end of namespace

#endif