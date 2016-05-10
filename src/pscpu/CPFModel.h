#ifndef __cpscpu_CPFMode_H__
#define	__cpscpu_CPFMode_H__

#include"PFModel.h"

namespace cpscpu {
	class CPFModel : public PFModel {
	public:

		CPFModel() : PFModel(){};

		CPFModel(IEEEDataIsland * island) : PFModel(island){};

		void setPIncreaseVector(double * v){
			this->pIncreaseVector = v;
		};

		void setQIncreaseVector(double * v){
			this->qIncreaseVector = v;
		};

		double * getPIncreaseVector() {
			return pIncreaseVector;
		};

		double * getQIncreaseVector() {
			return qIncreaseVector;
		};

	private:
		double * pIncreaseVector;
		double * qIncreaseVector;
	};
}//end of namespace

#endif