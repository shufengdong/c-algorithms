#ifndef __cpscpu_OPFMode_H__
#define	__cpscpu_OPFMode_H__

#include"PFModel.h"

#define OPF_MODE_PQVTHETA 0
#define OPF_MODE_UI 1

namespace cpscpu {
	class OPFModel : public PFModel {
	public:

		OPFModel() : PFModel(){
			initial();
		};

		OPFModel(IEEEDataIsland * island) : PFModel(island) {
			initial();
		};
		
		void setPControllableBus(int pControllableNum, int * pContrlllableBus) {
			this->pControllableNum = pControllableNum;
			this->pControllableBus = pContrlllableBus;
		};

		void setQControllableBus(int qControllableNum, int * qContrlllableBus) {
			this->qControllableNum = qControllableNum;
			this->qControllableBus = qContrlllableBus;
		};

		void setVControllableBus(int vControllableNum, int * vContrlllableBus) {
			this->vControllableNum = vControllableNum;
			this->vControllableBus = vContrlllableBus;
		};

	private:
		void initial() {
			pControllableBus = NULL;	
			qControllableBus = NULL;
			vControllableBus = NULL;
			pControllableNum = 0;
			qControllableNum = 0;
			vControllableNum = 0;
		};
	public:
		int* pControllableBus;
		int* qControllableBus;
		int* vControllableBus;

		int pControllableNum;
		int qControllableNum;
		int vControllableNum;
	};
}//end of namespace

#endif