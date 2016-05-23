#ifndef __cpscpu_NewtonCalculator_H__
#define	__cpscpu_NewtonCalculator_H__

#include"NewtonModel.h"

namespace cpscpu {
	class NewtonAlg{
	public:
		static bool cal(NewtonModel * model, double * result);

		static bool cal2(NewtonModel * model, double * result);

		//static double * cal_CurrentInjection(NewtonPF_CIHM_rect * model);

		//static double * cal_CI_withHession(NewtonPF_CIHM_rect * model);

		//static double * cal_CI_withHession_2(NewtonPF_CIHM_rect * model);

	};
}//end of namespace

#endif