#ifndef __cpscpu_Hession_H__
#define	__cpscpu_Hession_H__

#include "YMatrix.h"
#include "MeasureVector.h"
#include "StateCalculator.h"
#include "Jacobian.h"
#include "AbstractSM.h"

namespace cpscpu {
	class Hessian {
	
	private:
		AbstractSM * matrix;
		MeasureVector * meas;
		YMatrix * y;
		const double * state;

	public:
		Hessian(MeasureVector* meas, YMatrix * y, const double * state);

		void formHessian_polar();	

		void formHessian_polar(const double * lamda);

		void formHessian_fullState();

		void formHessian_fullState(const double * lamda);

		void formHessian_rect();

		void formHessian_rect(const double * lamda);

		void setMatrix(AbstractSM * matrix) {
			this->matrix = matrix;
		};

		AbstractSM * getMatrix() {
			return matrix;
		};
		
	public:
		void _bus_p_polar(int pos, int n, double w);

		void _bus_q_polar(int pos, int n, double w);

		void _line_from_p_polar(int pos, int n, double w);

		void _line_from_q_polar(int pos, int n, double w);

		void _line_to_p_polar(int pos, int n, double w);

		void _line_to_q_polar(int pos, int n, double w);

		//following is for rectangle coordinate, using ei and fi as variables

		void _bus_p_rect(int pos, int n, double w);

		void _bus_q_rect(int pos, int n, double w);

		void _bus_v_rect(int pos, int n, double w);

		void _line_from_p_rect(int pos, int n, double w);

		void _line_from_q_rect(int pos, int n, double w);

		void _line_to_p_rect(int pos, int n, double w);

		void _line_to_q_rect(int pos, int n, double w);


	};
}//end of namespace

#endif