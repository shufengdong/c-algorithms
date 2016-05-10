#ifndef __CPSCPU_NewtonModel_H__
#define	__CPSCPU_NewtonModel_H__

#include"AbstractSM.h"

namespace cpscpu {
	class NewtonModel {
	public:

		virtual int getDimension() = 0;

		 /**
		 * @return max interation number
		 */
		virtual int getMaxIter() = 0;

		/**
		 * @return tolerance denoting whether that algorithm is converged
		 */
		virtual double getTolerance() = 0;


		/**
		 * @return initial value of state variables
		 */
		virtual void getInitial(double* v) = 0;

		/**
		 * @param state value of state variables
		 * @return get jocobian matrix under a certain state
		 */
		virtual void getJocobian(double *state, AbstractSM * matrix) = 0;

		/**
		 * @return measurement value
		 */
		virtual void getZ(double* z) = 0;

		/**
		 * @param state value of state variables
		 * @return computing value of measurement point
		 */
		virtual void calZ(double* state, double* z_est) = 0;
	};
}//end of namespace

#endif