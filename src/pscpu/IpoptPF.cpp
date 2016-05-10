#include <time.h>
#include <assert.h>

#include "Jacobian.h"
#include "Hessian.h"
#include "IpIpoptApplication.hpp"
#include "MathUtil.h"
#include "IpoptPF.h"
#include "MapSparseMatrix.h"

using namespace Ipopt;
using namespace cpscpu;

//destructor
IpoptPF::~IpoptPF(){}

IpoptPF::IpoptPF(PFModel *model) : AbstractPF(model){}

IpoptPF::IpoptPF(PFModel *model, int mode) : AbstractPF(model, mode){}

void IpoptPF::doPF() {
	bool b = initialModel();
	if(!b) {
		cout<<"Do power flow failed becase of wrong pf model."<<endl;
		model->isConverged = false;
		return;
	}
	IpoptPF * pf = new IpoptPF(model, getMode());	

	SmartPtr<TNLP> mynlp = pf;

	// Create a new instance of IpoptApplication
	//  (use a SmartPtr, not raw)
	SmartPtr<IpoptApplication> app = new IpoptApplication();

	// Change some options
	if(!getIsPrintPath())
		app->Options()->SetIntegerValue("print_level", 1);	
	//app->Options()->SetStringValue("hessian_approximation", "limited-memory");
	app->Options()->SetNumericValue("ma27_liw_init_factor", 20);
	app->Options()->SetNumericValue("ma27_la_init_factor", 20);
	app->Options()->SetNumericValue("tol", model->tolerance);
	app->Options()->SetIntegerValue("max_iter", model->maxIter);
	//app->Options()->SetStringValue("mu_strategy", "adaptive");
	//app->Options()->SetStringValue("output_file", "ipopt.out");
	// The following overwrites the default name (ipopt.opt) of the options file
	//app->Options()->SetStringValue("option_file_name", "hs071.opt");

	// Intialize the IpoptApplication and process the options
	ApplicationReturnStatus status;
	status = app->Initialize();
	if (status != Solve_Succeeded) {
		printf("\n\n*** Error during initialization!\n");
		delete pf->jacobianM;
		delete pf->hessianM;
		return;
	}

	// Ask Ipopt to solve the problem
	status = app->OptimizeTNLP(mynlp);

	if (status == Solve_Succeeded) {
		printf("\n\n*** The problem solved!\n");
		model->isConverged = true;
		//iterTimes = app->Statistics()->IterationCount();
		//printf("\n\n*** The problem solved in %d iterations!\n", iter_count);
	} else {
		printf("\n\n*** The problem FAILED!\n");
		model->isConverged = false;
	}
	delete pf->jacobianM;
	delete pf->hessianM;
}

// returns the size of the problem
bool IpoptPF::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
                             Index& nnz_h_lag, IndexStyleEnum& index_style) {	
	n = 2 * (model->island->busN);
	this->jacobianM = new SparseMatrix(n, n);
	this->hessianM = new SparseMatrix(n, n);

	// one equality constraint and one inequality constraint
	m = model->meas.measNum;

	Jacobian jacobian(&(model->meas), &(model->y), model->initialX);
	jacobian.setMatrix(jacobianM);
	Hessian hessian(&(model->meas), &(model->y), model->initialX);	
	hessian.setMatrix(hessianM);
	if(getMode() == PF_MODE_VTHETA) {
		jacobian.formJacobian_polar();	
		hessian.formHessian_polar();
	} else if(getMode() == PF_MODE_RECT) {
		jacobian.formJacobian_rect();	
		hessian.formHessian_rect();
	}
	
	nnz_jac_g = jacobianM->unzeroNum;

	// the hessian is also dense and has 16 total nonzeros, but we
	// only need the lower left corner (since it is symmetric)
	nnz_h_lag = hessianM->getUnzeroNum();

	// use the C style indexing (0-based)
	index_style = TNLP::C_STYLE;

	return true;
}

// returns the variable bounds
bool IpoptPF::get_bounds_info(Index n, Number* x_l, Number* x_u,
                                Index m, Number* g_l, Number* g_u) {
	if(getMode() == PF_MODE_VTHETA) {
		for (int i= 0; i< model->island->busN; i++) {
			x_l[i] = 0.5;
			x_u[i] = 1.5;		
			x_l[i + model->island->busN] = -PI;
			x_u[i + model->island->busN] = PI;
		}
	} else if(getMode() == PF_MODE_RECT) {
		for (int i= 0; i< model->island->busN; i++) {
			x_l[i] = -1.5;
			x_u[i] = 1.5;		
			x_l[i + model->island->busN] = -1.5;
			x_u[i + model->island->busN] = 1.5;
		}
	} 
	for(int i = 0; i < m; i++) {
		g_l[i] = 0;
		g_u[i] = 0;
	}
	return true;
}

// returns the initial point for the problem
bool IpoptPF::get_starting_point(Index n, bool init_x, Number* x,
							   bool init_z, Number* z_L, Number* z_U,
							   Index m, bool init_lambda,
							   Number* lambda) {
	// Here, we assume we only have starting values for x, if you code
	// your own NLP, you can provide starting values for the dual variables
	// if you wish
	assert(init_x == true);
	assert(init_z == false);
	assert(init_lambda == false);

	// initialize to the given starting point
	for(int i = 0; i < n; i++)
		x[i] = model->initialX[i];
	return true;
}

// returns the value of the objective function
bool IpoptPF::eval_f(Index n, const Number* x, bool new_x, Number& obj_value) {
	return true;
}

// return the gradient of the objective function grad_{x} f(x)
bool IpoptPF::eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f) {
	return true;
}

// return the value of the constraints: g(x)
bool IpoptPF::eval_g(Index n, const Number* x, bool new_x, Index m, Number* g) {
	long start = clock();//get current system time(ms)
	
	double* estimated = new double[model->meas.measNum];
	if(getMode() == PF_MODE_VTHETA) {
		StateCalculator::getEstimatedZ_polar(&(model->meas), &(model->y), x, estimated);
	} else if(getMode() == PF_MODE_RECT) {
		StateCalculator::getEstimatedZ_rect(&(model->meas), &(model->y), x, estimated);
	}
	for(int i = 0; i < model->meas.measNum; i++) 
		g[i] = estimated[i] - model->meas.meas[i].value;	
	delete[] estimated;
	if(isDebug)
		cout<<"Time used for eval g:"<<clock() - start<<"ms"<<endl;			
	return true;
}

// return the structure or values of the jacobian
bool IpoptPF::eval_jac_g(Index n, const Number* x, bool new_x,
                           Index m, Index nele_jac, Index* iRow, Index *jCol,
                           Number* values) { 
	long start = clock();
	const double* state;
	if(x == NULL) {
		state = model->initialX;
	} else
		state = x;
	
	if (values == NULL) {
		jacobianM->getStructure(iRow, jCol);		
	} else {
		Jacobian jacobian(&(model->meas),&(model->y),state);
		jacobian.setMatrix(jacobianM);	
		if(getMode() == PF_MODE_VTHETA) {
			jacobian.formJacobian_polar();
		} else if(getMode() == PF_MODE_RECT) {
			jacobian.formJacobian_rect();
		}	
		jacobianM->getValues(values);				
	}
	if(isDebug)
		cout<<"Time used for eval jacobian of g:"<<clock() - start<<"ms"<<endl;
	return true;
}

//return the structure or values of the hessian
bool IpoptPF::eval_h(Index n, const Number* x, bool new_x,
                       Number obj_factor, Index m, const Number* lambda,
                       bool new_lambda, Index nele_hess, Index* iRow,
                       Index* jCol, Number* values) {
	long start = clock();
	const double* state;
	if(x == NULL) {
		state = model->initialX;
	} else 
		state = x;
	
	if (values == NULL) {
		//form hessian's structure according to variables mode
		hessianM->getStructure(iRow, jCol);	
	} else {				
		Hessian hessian(&(model->meas),&(model->y), state);	
		hessian.setMatrix(hessianM);
		hessianM->makeZero();
		//form hessian's nonzero values according to variables mode
		if(getMode() == PF_MODE_VTHETA) {
			hessian.formHessian_polar(lambda);
		} else if(getMode() == PF_MODE_RECT) 
			hessian.formHessian_rect(lambda);		

		hessianM->getValues(values);
	}
	if(isDebug)
		cout<<"Time used for eval hessian:"<<clock() - start<<"ms"<<endl;
	return true;
}

void IpoptPF::finalize_solution(SolverReturn status,
                                  Index n, const Number* x, const Number* z_L, const Number* z_U,
                                  Index m, const Number* g, const Number* lambda, Number obj_value, 
								  const IpoptData* ip_data, IpoptCalculatedQuantities* ip_cq) {
	if(getMode() == PF_MODE_VTHETA) {
		for (Index i = 0; i< n; i++) 
			model->vTheta[i] = x[i];	
	} else if(getMode() == PF_MODE_RECT) {
		int  n = model->island->busN;
		for (Index i = 0; i < n; i++) {
			model->vTheta[i] = sqrt(x[i] * x[i] + x[i + n] * x[i + n]);	
			model->vTheta[i + n] = atan2(x[i + n], x[i]);	
		}
	}
}
