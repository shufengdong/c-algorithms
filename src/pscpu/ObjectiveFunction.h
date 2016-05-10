#ifndef __cpscpu_ObjectiveFunction_H__
#define	__cpscpu_ObjectiveFunction_H__

#include "MeasureVector.h"

namespace cpscpu {
	class ObjectiveFunction {
	public:
		virtual ~ObjectiveFunction(){};

		// to estimate hessian matrix of the objective function
		virtual void eval_hessian_obj(int n, const double* x, int* measInObjFunc, int offset, double factor, double* result) = 0;

		// to estimate the objective function
		virtual bool eval_f(int n, const double* x, bool new_x, double* obj_value, int* measInObjFunc, int offset) = 0;

		// to estimate jacobian matrix of the objective function
		virtual bool eval_grad_f(int n, const double* x, bool new_x, double* grad_f, int* measInObjFunc, int offset) = 0;

		virtual int eval_nele_hess_num(){
			 return 0;
		};

		virtual void eval_nele_hess_pos(int* value) {
		};

		virtual void eval_nele_hess_v(double* value){			
		};
	};

	class WLSObjFunction : public ObjectiveFunction {
	public:
		MeasureVector * measVector;
		double* noneZeroValuesInHessian;//need to be destroyed by this class itself

	public:	
		WLSObjFunction();

		void eval_hessian_obj(int n, const double* x, int* measInObjFunc, int offset, double factor, double* result);

		bool eval_f(int n, const double* x, bool new_x, double* obj_value, int* measInObjFunc, int offset);

		bool eval_grad_f(int n, const double* x, bool new_x, double* grad_f, int* measInObjFunc, int offset);

		~WLSObjFunction();

		void setMeasVector(MeasureVector * measV);
	};

	class SigmoidObjFunction : public ObjectiveFunction {
	public:		
		double * a, * b;
		double shortenRate;

		SigmoidObjFunction();

		void setA(double * a);

		void setB(double * b);

		void setShortenRate(double d);

		void eval_hessian_obj(int n, const double* x, int* measInObjFunc, int offset, double factor, double* result);

		// to estimate the objective function
		bool eval_f(int n, const double* x, bool new_x, double* obj_value, int* measInObjFunc, int offset);

		// to estimate jacobian matrix of the objective function
		bool eval_grad_f(int n, const double* x, bool new_x, double* grad_f, int* measInObjFunc, int offset);	
	};
}

#endif