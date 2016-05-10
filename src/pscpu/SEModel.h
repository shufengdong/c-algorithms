#ifndef __cpscpu_SEModel_H__
#define	__cpscpu_SEModel_H__

#include "IEEEDataIsland.hpp"
#include "MeasureVector.h"
#include "YMatrix.h"
#include "ObjectiveFunction.h"

namespace cpscpu {
	class SEModel {
	public:
		int maxIter;
		int iterNum;		
		
		int slackBusCol;		
	
		double tolerance;

		bool isConverged;
		bool printPath;		
		bool isSlackBusVoltageFixed;
		
		double slackBusVoltage;
		double slackBusAngle;
		double objective;
		
		//the following four variables should be set from outside
		double * initialState;
		MeasureVector * measVector;
		IEEEDataIsland * island;
		ObjectiveFunction* obj;

		//should be destroyed by itself
		YMatrix y;				
		double * finalVTheta;
		int* measInObjFunc;		
		double * initialX;
		double* z_est;
		
	public:
		SEModel();	
		~SEModel(){
			freeSpace();
		};

		void freeSpace();

		void setMeasVector(MeasureVector * measVector);
		void setIsland(IEEEDataIsland * island);
		void setObjFunc(ObjectiveFunction * func);
		void setInitialState(double * state);
		
		bool getIsConverged(){return isConverged;};
		double* getFinalVTheta(){return finalVTheta;};
	};
}

#endif