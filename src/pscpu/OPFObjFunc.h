#ifndef __cpscpu_OPFObjFunc_H__
#define	__cpscpu_OPFObjFunc_H__

#include "OPFModel.h"
#include "AbstractSM.h"

namespace cpscpu {
	class OPFObjFunc {
	public:

		OPFObjFunc();

		OPFObjFunc(OPFModel * model);

		OPFObjFunc(OPFModel * model, int opfMode);

		virtual ~OPFObjFunc(){};

		// to estimate hessian matrix of the objective function
		virtual void eval_hessian_obj(int n, const double* x, double factor, AbstractSM* result) = 0;

		// to estimate the objective function
		virtual bool eval_f(int n, const double* x, bool new_x, double* obj_value) = 0;

		// to estimate jacobian matrix of the objective function
		virtual bool eval_grad_f(int n, const double* x, bool new_x, double* grad_f) = 0;


		void setModel(OPFModel * model);

		OPFModel * getModel();

		void setOPFMode(int mode);

		int getOPFModel();

	protected:
		OPFModel * model;

		int opfMode;
	};

	class MinPGen : public OPFObjFunc {
	public:
		MinPGen() : OPFObjFunc(){};

		MinPGen(OPFModel * model) : OPFObjFunc(model){};

		MinPGen(OPFModel * model, int opfMode) : OPFObjFunc(model, opfMode){};

		void eval_hessian_obj(int n, const double* x, double factor, AbstractSM* result);

		// to estimate the objective function
		bool eval_f(int n, const double* x, bool new_x, double* obj_value);

		// to estimate jacobian matrix of the objective function
		bool eval_grad_f(int n, const double* x, bool new_x, double* grad_f);	
	};
}

#endif