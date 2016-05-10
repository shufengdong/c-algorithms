#ifndef __StateCalculator_H__
#define	__StateCalculator_H__

#include"YMatrix.h"
#include"MeasureVector.h"

namespace cpscpu {
	class StateCalculator {
	public:
		static void getEstimatedZ_fullState(const MeasureVector *meas, YMatrix * y, const double*  state, double* z_est);

		static void getEstimatedZ_polar(const MeasureVector *meas, YMatrix* y, const double*  state, double* z_est);

		static void getEstimatedZ_rect(MeasureVector *meas, YMatrix * y, const double*  state, double * result);

		static double calBusP_polar(int num, YMatrix * y, const double* state);
		
		static double calBusQ_polar(int num, YMatrix * y, const double* state);
		
		static double calLinePFrom_polar(int branchId, YMatrix * y, const double* state); 

		static double calLinePTo_polar(int num, YMatrix * y, const double* state);
		
		static double calLineQFrom_polar(int branchId, YMatrix * y, const double* state); 
		
		static double calLineQTo_polar(int num, YMatrix * y, const double* state);		
	
		static double calBusP_rect(int num, YMatrix * y, const double* state);
		
		static double calBusQ_rect(int num, YMatrix * y, const double* state);
		
		static double calLinePFrom_rect(int branchId, YMatrix * y, const double* state); 

		static double calLinePTo_rect(int num, YMatrix * y, const double* state);
		
		static double calLineQFrom_rect(int branchId, YMatrix * y, const double* state); 
 
		static double calLineQTo_rect(int num, YMatrix * y, const double* state);

		//for Current Inject Hession Modify Model
		static double* getEstimatedZ_CIHM_rect(const MeasureVector *meas, YMatrix * y, const double*  state);

		static double calBusNetReal_CIHM_rect(int num, YMatrix *admittanceGetter, const double* state);
		
		static double calBusNetImage_CIHM_rect(int num, YMatrix *admittanceGetter, const double* state);

		static double calBusP_CIHM_rect(int num, int n, const double* state);

		static double calBusQ_CIHM_rect(int num, int n, const double* state);

		static double calBusV2_CIHM_rect(int num, int n, const double* state);

	};
}//end of namespace

#endif