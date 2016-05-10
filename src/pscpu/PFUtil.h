#ifndef __cpscpu_PFUtil_H__
#define	__cpscpu_PFUtil_H__

#include "IEEEDataIsland.hpp"
#include "MeasureConstants.h"
#include "MeasureVector.h"
#include "MathUtil.h"
#include <iostream>
#include <string>
#include <cmath>


namespace cpscpu {
	class PFUtil {
	public:

		static void formMeas4PF_polar(IEEEDataIsland *island, MeasureVector * meas);

		static void formMeas4PF_rect(IEEEDataIsland *island, MeasureVector * meas);

		static void formMeas4OPF(IEEEDataIsland *island, MeasureVector * meas, int pControllableNum, int qControllableNum, int vControllableNum, 
			int* pControllableBus, int* qControllableBus, int* vControllableBus, bool isSlackBusVFixed);
		
		static void formMeas4PF_CI(IEEEDataIsland *island, MeasureVector * meas);
		
		static void formMeasure(int type, int positionId, double value, MeasureInfo* info);

		static void find_CNnode(IEEEDataIsland *island){
			for(int i=0;i<island->busN;i++){
				if( (island->buses[i].busType == 0 || island->buses[i].busNumber == 1) && abs(island->buses[i].generationP-island->buses[i].loadP)<1e-6 && abs(island->buses[i].generationQ-island->buses[i].loadQ)<1e-6){
					island->buses[i].busType = 4;
				}	
			}
		}
	};
}//end of namespace

#endif