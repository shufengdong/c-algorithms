#include "IpoptOPF.h"
#include "Jacobian.h"
#include "Hessian.h"
#include "MathUtil.h"

#include "IpIpoptApplication.hpp"
#include <time.h>

using namespace Ipopt;
using namespace cpscpu;
//destructor
IpoptOPF::~IpoptOPF() {}


void IpoptOPF::doOPF() {
	bool b = initialModel();
	if(!b) {
		cout<<"Do power flow failed becase of wrong se model."<<endl;
		model->isConverged = false;
		return;
	}
	IpoptOPF * opf = new IpoptOPF(model, getMode());	
	opf->setObjFunc(getObjFunc());
	SmartPtr<TNLP> mynlp = opf;

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
		delete opf->jacobianM;
		delete opf->hessianM;
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
	delete opf->jacobianM;
	delete opf->hessianM;
}

// returns the size of the problem
bool IpoptOPF::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
                             Index& nnz_h_lag, IndexStyleEnum& index_style) {	
	n = 4 * (model->island->busN);

	// one equality constraint and one inequality constraint
	m = n / 2 + model->meas.measNum;

	jacobianM = new SparseMatrix(m, n);
	hessianM = new SparseMatrix(n, n);

	Jacobian jacobian(&(model->meas), &(model->y), model->initialX);
	jacobian.setMatrix(jacobianM);
	Hessian hessian(&(model->meas), &(model->y), model->initialX);	
	hessian.setMatrix(hessianM);
	if(getMode() == OPF_MODE_PQVTHETA) {	
		int n2 = model->island->busN;
		for (int i = 0; i < n2; i++) {
			int busNumber = model->island->buses[i].busNumber;
			jacobian._bus_p_polar(busNumber, n2, busNumber - 1);		
			jacobianM->setValue(busNumber - 1, busNumber + 2 * n2 - 1, -1.0);
			jacobian._bus_q_polar(busNumber, n2, busNumber + n2 - 1);
			jacobianM->setValue(busNumber - 1 + n2, busNumber + 3 * n2 - 1, -1.0);
		}
		jacobian.formJacobian_fullState(2 * n2);		
		
		for (int i = 0; i < n2; i++) {
			int busNumber = model->island->buses[i].busNumber;
			hessian._bus_p_polar(busNumber, n2, 1.0);				
			hessian._bus_q_polar(busNumber, n2, 1.0);
		}
		hessian.formHessian_fullState();
	} else if(getMode() == OPF_MODE_UI) {
		//jacobian.formJacobian_ui();	
		//hessian.formHessian_ui();
	}
	objFunc->eval_hessian_obj(n, model->initialX, 1.0, hessianM);
	nnz_jac_g = jacobianM->getUnzeroNum();

	nnz_h_lag = hessianM->getUnzeroNum();

	cout<<"There are "<<nnz_jac_g<<" nonzero number in jacobian"<<" and "<<nnz_h_lag<<" nonzeror numbin in hessian."<<endl;
	// use the C style indexing (0-based)
	index_style = TNLP::C_STYLE;
	return true;
}

// returns the variable bounds
bool IpoptOPF::get_bounds_info(Index n, Number* x_l, Number* x_u,
                                Index m, Number* g_l, Number* g_u) {
	if(getMode() == OPF_MODE_PQVTHETA) {									
		for (int i= 0; i < n / 4; i++) { 
			x_l[i] = 0.5;
			x_u[i] = 1.5;
		}
		for (int i= n / 4; i< n / 2; i++)  {
			x_l[i] = -PI;
			x_u[i] = PI;
		}

		for(int i = n / 2; i < n; i ++) {//todo: not good
			x_l[i] = -2e15;
			x_u[i] = 2e15;
		}
	} else if(getMode() == OPF_MODE_UI) {
		int busN = n / 4;
		for (int i= 0; i < busN; i++) { 
			x_l[i] = -1.5;
			x_u[i] = 1.5;
			x_l[i + busN] = -1.5;
			x_u[i + busN] = 1.5;
		}
		for(int i = 2 * busN; i < n; i ++) {//todo: not good
			x_l[i] = -2e15;
			x_u[i] = 2e15;
		}
	}
	for(int i = 0; i < m; i++) {
		g_l[i] = 0;
		g_u[i] = 0;
	}
	return true;
}

// returns the initial point for the problem
bool IpoptOPF::get_starting_point(Index n, bool init_x, Number* x,
							   bool init_z, Number* z_L, Number* z_U,
							   Index m, bool init_lambda,
							   Number* lambda) {
	// Here, we assume we only have starting values for x, if you code
	// your own NLP, you can provide starting values for the dual variables
	// initialize to the given starting point
	for(int i = 0; i < n; i++)
		x[i] = model->initialX[i];
	return true;
}

// returns the value of the objective function
bool IpoptOPF::eval_f(Index n, const Number* x, bool new_x, Number& obj_value) {
	return objFunc->eval_f(n, x, new_x, &obj_value);
}

// return the gradient of the objective function grad_{x} f(x)
bool IpoptOPF::eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f) {
	return objFunc->eval_grad_f(n, x, new_x, grad_f);;
}

// return the value of the constraints: g(x)
bool IpoptOPF::eval_g(Index n, const Number* x, bool new_x, Index m, Number* g) {
	long start = clock();//get current system time(ms)
	
	int busN = model->island->busN; 			
	double* estimated = new double[model->meas.measNum];
	if(getMode() == OPF_MODE_PQVTHETA) {
		for(int i = 0; i < busN; i++) {
			int busNumber = model->island->buses[i].busNumber;
			g[busNumber - 1] = StateCalculator::calBusP_polar(busNumber, &(model->y), x) - x[busNumber + 2 * busN - 1];
			g[busNumber + busN - 1] = StateCalculator::calBusQ_polar(busNumber, &(model->y), x) - x[busNumber + 3 * busN - 1];
		}
		StateCalculator::getEstimatedZ_fullState(&(model->meas), &(model->y), x, estimated);
	} else if(getMode() == OPF_MODE_UI) {//todo: not finished...
		//StateCalculator::getEstimatedZ_ui(&(model->meas), &(model->y), x, estimated);
	}
	for(int i = 0; i < model->meas.measNum; i++)
		g[i + busN * 2] = estimated[i] - model->meas.meas[i].value;
	delete[] estimated;
	if(isDebug)
		cout<<"Time used for eval g:"<<clock() - start<<"ms"<<endl;		
	return true;
}

// return the structure or values of the jacobian
bool IpoptOPF::eval_jac_g(Index n, const Number* x, bool new_x,
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
		if(getMode() == OPF_MODE_PQVTHETA) {
			int n2 = model->island->busN;
			for (int i = 0; i < n2; i++) {
				int busNumber = model->island->buses[i].busNumber;
				jacobian._bus_p_polar(busNumber, n2, busNumber - 1);		
				jacobian._bus_q_polar(busNumber, n2, busNumber + n2 - 1);
			}
			jacobian.formJacobian_fullState(2 * n2);
		} else if(getMode() == OPF_MODE_UI) {
			//jacobian.formJacobian_ui();
		}	
		jacobianM->getValues(values);				
	}
	if(isDebug)
		cout<<"Time used for eval jacobian of g:"<<clock() - start<<"ms"<<endl;			
	return true;
}

//return the structure or values of the hessian
bool IpoptOPF::eval_h(Index n, const Number* x, bool new_x,
                       Number obj_factor, Index m, const Number* lambda,
                       bool new_lambda, Index nele_hess, Index* iRow,
                       Index* jCol, Number* values)
{
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
		if(getMode() == OPF_MODE_PQVTHETA) {
			int n2 = model->island->busN;
			for (int i = 0; i < n2; i++) {
				int busNumber = model->island->buses[i].busNumber;
				hessian._bus_p_polar(busNumber, n2, lambda[busNumber - 1]);				
				hessian._bus_q_polar(busNumber, n2, lambda[busNumber - 1 + n2]);
			}
			hessian.formHessian_fullState(lambda + 2 * n2);
		} else if(getMode() == OPF_MODE_UI) { 
			//hessian.formHessian_ui(lambda);
		}
		objFunc->eval_hessian_obj(n, x, obj_factor, hessianM);

		hessianM->getValues(values);
	}
	if(isDebug)
		cout<<"Time used for eval hessian:"<<clock() - start<<"ms"<<endl;
	return true;
}

void IpoptOPF::finalize_solution(SolverReturn status,
                                  Index n, const Number* x, const Number* z_L, const Number* z_U,
                                  Index m, const Number* g, const Number* lambda, Number obj_value,
								const IpoptData* ip_data, IpoptCalculatedQuantities* ip_cq) 
{
	if(model->vTheta != NULL)
		delete[] model->vTheta;
	int busN = model->island->busN;
	model->vTheta = new double[2 * busN];										
	if(getMode() == OPF_MODE_PQVTHETA) {
		for (Index i = 0; i< 2 * busN; i++) 
			model->vTheta[i] = x[i];	
	} else if(getMode() == OPF_MODE_UI) {
		for (Index i = 0; i < busN; i++) {
			model->vTheta[i] = sqrt(x[i] * x[i] + x[i + busN] * x[i + busN]);	
			model->vTheta[i + busN] = atan2(x[i + busN], x[i]);	
		}
	}	
}
