#ifndef __cpscpu_PFMode_H__
#define	__cpscpu_PFMode_H__

#include"IEEEDataIsland.hpp"
#include"YMatrix.h"
#include"MeasureVector.h"
#include"StateCalculator.h"

namespace cpscpu {
	class PFModel {
	public:

		virtual ~PFModel();

		PFModel();

		PFModel(IEEEDataIsland * island);

		void freeSpace();
	
		void setIsland(IEEEDataIsland* island);

	public:
		//should destroyed by itself
		double * initialX;		
		double * vTheta;
		
		int maxIter;
		double tolerance;
		bool isConverged ;
		bool printPath;

		//set from outside
		IEEEDataIsland * island;		

		YMatrix y;    
		MeasureVector meas;
	};
} //end of namespace

#endif