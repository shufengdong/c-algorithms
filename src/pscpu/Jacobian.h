#ifndef __cpscpu_Jacobian_H__
#define	__cpscpu_Jacobian_H__

#include"YMatrix.h"
#include"MeasureVector.h"
#include"StateCalculator.h"

namespace cpscpu {
	class Jacobian {
	private:
		AbstractSM * matrix;
		YMatrix * y;
		MeasureVector * meas;
		const double * state;
		
	public:
		Jacobian(MeasureVector* meas, YMatrix * y, const double * state);

		void formJacobian_polar();	

		void formJacobian_rect();
		
		void formJacobian_fullState(int startRow);

		//for current injection model with hession modify
		void formJacobian_CIHM_rect_const();
		
		void formJacobian_CIHM_rect_vary(double* state);

		void setMatrix(AbstractSM * matrix) {
			this->matrix = matrix;
		};

		AbstractSM * getMatrix() {
			return matrix;
		};

	public:
		//pos: busNumber,currentRow: in Jacobian Matrix
		void _bus_a_polar(int pos, int n, int currentRow);

		void _bus_v_polar(int pos, int currentRow);

		void _bus_p_polar(int pos, int n, int currentRow);

		void _bus_q_polar(int pos, int n, int index);

		void _line_from_p_polar(int pos, int n, int currentRow);

		void _line_from_q_polar(int pos, int n, int currentRow);

		void _line_to_p_polar(int pos, int n,int currentRow);

		void _line_to_q_polar(int pos, int n, int currentRow);

		void _bus_a_rect(int pos, int n, int currentRow);

		void _bus_v_rect(int pos, int n, int currentRow);

		void _bus_p_rect(int pos, int n, int currentRow);

		void _bus_q_rect(int pos, int n, int index);

		void _line_from_p_rect(int pos, int n, int currentRow);

		void _line_from_q_rect(int pos, int n, int currentRow);

		void _line_to_p_rect(int pos, int n,int currentRow);

		void _line_to_q_rect(int pos, int n, int currentRow);

		//for pf rect
		void _bus_Ux_rect(int pos, int n, int currentRow);

		void _bus_Uy_rect(int pos, int n, int currentRow);
		
		//for CIHM model
		void _bus_freal_CI_rect(int pos, int n, int currentRow);

 		void _bus_fimage_CI_rect(int pos, int n, int currentRow);

		void _bus_v_CI_rect(int pos, int n, int currentRow);
		
		void _bus_p_CI_rect(int pos, int n, int currentRow);
		
		void _bus_q_CI_rect(int pos, int n, int currentRow);
		
		void _bus_e_CI_rect(int pos, int n, int currentRow);
		
		void _bus_f_CI_rect(int pos, int n, int currentRow);
		
		void _bus_ix_CI_rect(int pos, int n, int currentRow);
		
		void _bus_iy_CI_rect(int pos, int n, int currentRow);

	};
}//end of namespace

#endif