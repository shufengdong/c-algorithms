#ifndef __cpscpu_NewtonPF_polar_H__
#define	__cpscpu_NewtonPF_polar_H__

#include"AbstractPF.h"
#include"NewtonModel.h"

namespace cpscpu {
	class NewtonPF : public AbstractPF, public NewtonModel {

	public:

		NewtonPF();

		NewtonPF(PFModel * model);

		void doPF();

		/**
		 * @return variable num
		 */
		int getDimension();

		/**
		 * @return max interation number
		 */
		int getMaxIter();

		/**
		 * @return tolerance denoting whether that algorithm is converged
		 */
		double getTolerance();

		/**
		 * @return initial value of state variables
		 */
		void getInitial(double* state);

		/**
		 * @param state value of state variables
		 * @return get jocobian matrix under a certain state
		 */
		void getJocobian(double *state, AbstractSM * matrix);

		/**
		 * @return measurement value
		 */
		void getZ(double* z);

		/**
		 * @param state value of state variables
		 * @return computing value of measurement point
		 */
		void calZ(double* state, double* z_est);

		void getResult(double * result);

	private :
		double * result;
	};
}//end of namespace

#endif