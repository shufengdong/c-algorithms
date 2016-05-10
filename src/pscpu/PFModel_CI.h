#ifndef __cpscpu_PFModel_CI_H__
#define	__cpscpu_PFModel_CI_H__

#include"IEEEDataIsland.hpp"
#include"YMatrix.h"
#include"MeasureVector.h"
#include"StateCalculator.h"
#include "PFModel.h"

namespace cpscpu {
	class PFModel_CI : public PFModel {

	public:
		PFModel_CI() : PFModel(){}

		PFModel_CI(IEEEDataIsland * island):PFModel( island ){}

		virtual void initial();
	};
}//end of namespace

#endif