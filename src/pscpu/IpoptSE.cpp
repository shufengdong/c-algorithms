#include <time.h>
#include <string.h>
#include <assert.h>

#include "Jacobian.h"
#include "Hessian.h"
#include "IpIpoptApplication.hpp"
#include "MathUtil.h"
#include "IpoptSE.h"
#include "MeasureConstants.h"
#include "MapSparseMatrix.h"

using namespace cpscpu;

IpoptSE::~IpoptSE(){}

IpoptSE::IpoptSE(SEModel *model) : AbstractSE(model){}

IpoptSE::IpoptSE(SEModel *model, int mode) : AbstractSE(model, mode){}

void IpoptSE::doSE() {
	bool b = initialModel();
	if(!b) {
		cout<<"Do state estimation failed becase of wrong se model."<<endl;
		model->isConverged = false;
		return;
	}
	IpoptSE * ipoptSE = new IpoptSE(model, getMode());
	
	SmartPtr<TNLP> mynlp = ipoptSE;
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
		delete ipoptSE->jacobianM;
		delete ipoptSE->hessianM;
		return;
	}

	// Ask Ipopt to solve the problem
	status = app->OptimizeTNLP(mynlp);

	if (status == Solve_Succeeded) {
		printf("\n\n*** The problem solved!\n");
		model->isConverged = true;
	} else {
		printf("\n\n*** The problem FAILED!\n");
		model->isConverged = false;
	}	
	delete ipoptSE->jacobianM;
	delete ipoptSE->hessianM;
}

bool IpoptSE::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g, Index& nnz_h_lag, IndexStyleEnum& index_style) {		
	if(getMode() == SE_MODE_VTHETA || getMode() == SE_MODE_U)
		n = 2 * (model->island->busN);
	else if(getMode() == SE_MODE_PQVTHETA || getMode() == SE_MODE_UI)
		n = 4 * (model->island->busN);

	// one equality constraint and one inequality constraint
	m = model->measVector->measNum;
	if(getMode() == SE_MODE_PQVTHETA)
		m += 2 * (model->island->busN); 
	
	jacobianM = new SparseMatrix(m, n);				
	hessianM = new SparseMatrix(n, n);
	
	n += model->measVector->measNum;
	//todo: link bus is not considered.
	if (model->slackBusCol >= 0)
		m++;
    if (model->isSlackBusVoltageFixed)
        m++;	

	Jacobian jacobian(model->measVector, &(model->y), model->initialX);		
	Hessian hessian(model->measVector, &(model->y), model->initialX);	
	jacobian.setMatrix(jacobianM);
	hessian.setMatrix(hessianM);
	if(getMode() == SE_MODE_VTHETA) {
		jacobian.formJacobian_polar();
		hessian.formHessian_polar();	
	} else if(getMode() == SE_MODE_U) {
		jacobian.formJacobian_rect();
		hessian.formHessian_rect();	
	}else if(getMode() == SE_MODE_PQVTHETA) {
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
	} else if(getMode() == SE_MODE_UI) {
		//jacobian.formJacobian_ui();
		//hessian.formHessian_ui();	
	}	

	nnz_jac_g = jacobianM->getUnzeroNum() + model->measVector->measNum;
	if (model->slackBusCol >= 0)
		nnz_jac_g++;
    if (model->isSlackBusVoltageFixed)
        nnz_jac_g++;

	// we only need the lower left corner (since it is symmetric)
	nnz_h_lag = hessianM->getUnzeroNum() + model->measVector->measNum;

	// use the C style indexing (0-based)
	index_style = TNLP::C_STYLE;
	return true;
}

// returns the variable bounds
bool IpoptSE::get_bounds_info(Index n, Number* x_l, Number* x_u,Index m, Number* g_l, Number* g_u) {
	int offset = 0;
	int busN = model->island->busN;		
	if(getMode() == SE_MODE_VTHETA || getMode() == SE_MODE_U) {
		offset = 2 * busN;			
	} else if(getMode() == SE_MODE_PQVTHETA || getMode() == SE_MODE_UI) 
		offset = 4 * busN;
	for(int i = offset; i < n; i++) {
		x_l[i] = -2e15;
		x_u[i] = 2e15;
	}
	if(getMode() == SE_MODE_VTHETA || getMode() == SE_MODE_PQVTHETA) {
		for (int i = 0; i < busN; i++) {
			x_l[i] = 0.5;
			x_u[i] = 1.5;		
			x_l[i + busN] = -PI;
			x_u[i + busN] = PI;
		}
	} else if(getMode() == SE_MODE_U || getMode() == SE_MODE_UI) {
		for (int i = 0; i < busN; i++) {
			x_l[i] = -1.5;
			x_u[i] = 1.5;		
			x_l[i + busN] = -1.5;
			x_u[i + busN] = 1.5;
		}
	} 
	if(getMode() == SE_MODE_PQVTHETA) {
		for (int i = 0; i < busN; i++) {
			x_l[i + 2 * busN] = -100;
			x_u[i + 2 * busN] = 100;		
			x_l[i + 3 * busN] = -100;
			x_u[i + 3 * busN] = 100;
		}	
	} else if(getMode() == SE_MODE_UI) {
		for (int i = 0; i < busN; i++) {//todo: not right
			x_l[i + 2 * busN] = -100;
			x_u[i + 2 * busN] = 100;		
			x_l[i + 3 * busN] = -100;
			x_u[i + 3 * busN] = 100;
		}
	}
	for(int i = 0; i < m; i++) {
		g_l[i] = 0;
		g_u[i] = 0;
	}
	return true;
}

bool IpoptSE::get_starting_point(Index n, bool init_x, Number* x,
									  bool init_z, Number* z_L, Number* z_U,
									  Index m, bool init_lambda,
									  Number* lambda) {
	assert(init_x == true);
	assert(init_z == false);
	assert(init_lambda == false);
	for(int i = 0; i < n; i++)
		x[i] = model->initialX[i];
	return true;
}

bool IpoptSE::initialModel() {
	bool b = AbstractSE::initialModel();
	if(!b)
		return b;
	if(model->initialX != NULL)
		delete[] model->initialX;
	int busN = model->island->busN;
	if(getMode() == SE_MODE_VTHETA || getMode() == SE_MODE_U) {
		model->initialX = new double[busN * 2 + model->measVector->measNum];
		if (model->initialState != NULL) {
			memcpy(model->initialX, model->initialState, sizeof(double) * busN * 2);
		} else {
			for (int i = 0; i < busN; i++) {
				model->initialX[i] = 1.0;
				model->initialX[i + busN] = 0.0;
			}			
		}  
	} else if(getMode() == SE_MODE_PQVTHETA || getMode() == SE_MODE_UI) {//todo:not finished...
		model->initialX = new double[busN * 4 + model->measVector->measNum];
		if (model->initialState != NULL) {
			memcpy(model->initialX, model->initialState, sizeof(double) * busN * 4);
		} else {
			for (int i = 0; i < busN; i++) {
				model->initialX[i] = 1.0;
				model->initialX[i + busN] = 0.0;
			}			
		}
	}
	if(getMode() == SE_MODE_PQVTHETA && model->initialState == NULL) {
		for(int i = 0; i < busN; i++) {
			int busNumber = model->island->buses[i].busNumber;
			model->initialX[busNumber - 1 + 2 * busN] = StateCalculator::calBusP_polar(busNumber, &(model->y), model->initialX);
			model->initialX[busNumber - 1 + 3 * busN] = StateCalculator::calBusQ_polar(busNumber, &(model->y), model->initialX);
		}
	}
	if(model->z_est != NULL)
		delete[] model->z_est;
	model->z_est = new double[model->measVector->measNum];
	int offset = 0;
	if(getMode() == SE_MODE_VTHETA) {
		StateCalculator::getEstimatedZ_polar(model->measVector, &(model->y), model->initialX, model->z_est);
		offset = 2 * busN;
	} else if(getMode() == SE_MODE_U) {
		StateCalculator::getEstimatedZ_rect(model->measVector, &(model->y), model->initialX, model->z_est);
		offset = 2 * busN;
	} else if(getMode() == SE_MODE_PQVTHETA) {		
		StateCalculator::getEstimatedZ_fullState(model->measVector, &(model->y), model->initialX, model->z_est);
		offset = 4 * busN;
	} else if(getMode() == SE_MODE_UI) {//todo: not finished...
		//StateCalculator::getEstimatedZ_ui(model->measVector, &(model->y), model->initialX, model->z_est);
		offset = 4 * busN;
	} 
	for (int i = 0; i < model->measVector->measNum; i++) {
		 MeasureInfo * m = &model->measVector->meas[i];
		 if((getMode() == SE_MODE_U || getMode() == SE_MODE_UI) && m->measureType == TYPE_BUS_VOLTAGE)			
			model->initialX[i + offset] = m->value * m->value - model->z_est[i];
		else
			model->initialX[i + offset] = m->value - model->z_est[i];
	}
	return true;
}

// returns the value of the objective function
bool IpoptSE::eval_f(Index n, const Number* x, bool new_x, Number& obj_value) {	
	if(getMode() == SE_MODE_VTHETA || getMode() == SE_MODE_U) {
		return model->obj->eval_f(n, x, new_x, &obj_value, model->measInObjFunc,  2 * (model->island->busN));	  
	} else if(getMode() == SE_MODE_PQVTHETA || getMode() == SE_MODE_UI) {
		return model->obj->eval_f(n, x, new_x, &obj_value, model->measInObjFunc,  4 * (model->island->busN));	  
	}	
	return false;
}

// return the gradient of the objective function grad_{x} f(x)
bool IpoptSE::eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f) {
	if(getMode() == SE_MODE_VTHETA || getMode() == SE_MODE_U) {
		return model->obj->eval_grad_f(n, x, new_x, grad_f, model->measInObjFunc,  2 * (model->island->busN));	
	} else if(getMode() == SE_MODE_PQVTHETA || getMode() == SE_MODE_UI) {
		return model->obj->eval_grad_f(n, x, new_x, grad_f, model->measInObjFunc,  4 * (model->island->busN));		  
	}	
	return false;
}

// return the value of the constraints: g(x)
bool IpoptSE::eval_g(Index n, const Number* x, bool new_x, Index m, Number* g) {
	long start = clock();//get current system time(ms)
	int offset = 0;
	int gIndex = 0;		
	//judge mode first
	int busN = model->island->busN; 
	if(getMode() == SE_MODE_VTHETA) {
		StateCalculator::getEstimatedZ_polar(model->measVector, &(model->y), x, model->z_est);
		offset = 2 * busN;			
	} else if(getMode() == SE_MODE_U){
		StateCalculator::getEstimatedZ_rect(model->measVector, &(model->y), x, model->z_est);
		offset = 2 * busN;			
	} else if(getMode() == SE_MODE_PQVTHETA) {		
		for(int i = 0; i < busN; i++) {
			int busNumber = model->island->buses[i].busNumber;
			g[busNumber - 1] = StateCalculator::calBusP_polar(busNumber, &(model->y), x) - x[busNumber + 2 * busN - 1];
			g[busNumber + busN - 1] = StateCalculator::calBusQ_polar(busNumber, &(model->y), x) - x[busNumber + 3 * busN - 1];
		}
		StateCalculator::getEstimatedZ_fullState(model->measVector, &(model->y), x, model->z_est);
		offset = 4 * busN;
		gIndex += 2 * busN;
	} else if(getMode() == SE_MODE_UI) {//todo: not finished yet!
		//StateCalculator::getEstimatedZ_ui(model->measVector, &(model->y), x, model->z_est);
		offset = 4 * busN;
	}

	//form contraints
	for(int i = 0; i < model->measVector->measNum; i++, gIndex++) { 
		MeasureInfo * m = &model->measVector->meas[i];
		if((getMode() == SE_MODE_U || getMode() == SE_MODE_UI) && m->measureType == TYPE_BUS_VOLTAGE)			
			g[gIndex] = model->z_est[i] + x[i + offset] - m->value * m->value;
		else
			g[gIndex] = model->z_est[i] + x[i + offset] - m->value;	
	}
	if (model->slackBusCol >= 0) {//todo: not right here when u or u, i are variables
		if (model->isSlackBusVoltageFixed)
			g[gIndex++] = x[model->slackBusCol] - model->slackBusVoltage;
		if(getMode() == SE_MODE_U || getMode() == SE_MODE_UI)
			g[gIndex] = x[model->slackBusCol + busN];
		else 
			g[gIndex] = x[model->slackBusCol + busN] - model->slackBusAngle;
	}	
	if(isDebug)
		cout<<"Time used for eval g:"<<clock() - start<<"ms"<<endl;		
	return true;
}

// return the structure or values of the jacobian
bool IpoptSE::eval_jac_g(Index n, const Number* x, bool new_x,
							   Index m, Index nele_jac, Index* iRow, Index *jCol,
							   Number* values) {
	long start = clock();
	const double* state;
	if(x == NULL) {		
	   state = model->initialX;
	} else {
		state = x;
	}
	int busN = model->island->busN;
	int rowStart = 0;
	if(getMode() == SE_MODE_PQVTHETA)
		rowStart = 2 * busN;
	if (values == NULL) {	
		jacobianM->getStructure(iRow, jCol);		
		int rowCount = jacobianM->getM();
		int count = jacobianM->getUnzeroNum();		
		for(int i = rowStart, j = 0; i < jacobianM->getM(); i++, j++, count++) {
			iRow[count] = i;
			jCol[count] = jacobianM->getN() + j;
		}
		if (model->slackBusCol >= 0) {//todo: not right
		   if (model->isSlackBusVoltageFixed) {
			   iRow[count] = rowCount;
			   jCol[count] = model->slackBusCol;
			   count++;
			   rowCount++;
		   }
		   iRow[count] = rowCount;
		   jCol[count] = model->slackBusCol + model->island->busN;
		}
	} else {
		Jacobian jacobian(model->measVector, &(model->y), state);
		jacobian.setMatrix(jacobianM);		
		if(getMode() == SE_MODE_VTHETA) {
			jacobian.formJacobian_polar();
		} else if(getMode() == SE_MODE_U){
			jacobian.formJacobian_rect();
		} else if(getMode() == SE_MODE_PQVTHETA) {			
			for (int i = 0; i < busN; i++) {
				int busNumber = model->island->buses[i].busNumber;
				jacobian._bus_p_polar(busNumber, busN, busNumber - 1);		
				jacobian._bus_q_polar(busNumber, busN, busNumber + busN - 1);
			}
			jacobian.formJacobian_fullState(2 * busN);
		} else if(getMode() == SE_MODE_UI) {//todo: not finished yet!
			//jacobian.formJacobian_ui();
		}
	
		jacobianM->getValues(values);	
		int count = jacobianM->getUnzeroNum();
		for(int i = rowStart; i < jacobianM->getM(); i++, count++)
			values[count] = 1.0;
		if (model->slackBusCol >= 0) {//todo: not right
			if (model->isSlackBusVoltageFixed)
				values[count++] = 1;
			values[count] = 1;
		}
	}
	if(isDebug)
	   cout<<"Time used for eval jacobian of g:"<<clock() - start<<"ms"<<endl;
	return true;
}

//return the structure or values of the hessian
bool IpoptSE::eval_h(Index n, const Number* x, bool new_x,
                       Number obj_factor, Index m, const Number* lambda,
                       bool new_lambda, Index nele_hess, Index* iRow,
                       Index* jCol, Number* values) {
	long start = clock();
	const double* state = NULL;
	if(x == NULL) {		
	   state = model->initialX;
	} else {
	   state = x;
	}
	int offset = 0;
	int busN = model->island->busN;
	//form jacobian according variables' type
	if(getMode() == SE_MODE_VTHETA || getMode() == SE_MODE_U) {
		offset = busN * 2;
	} else if(getMode() == SE_MODE_PQVTHETA || getMode() == SE_MODE_UI) { 
		offset = busN * 4;
	} 
	//form hessian according variable' type
	if (values == NULL) {		
		hessianM->getStructure(iRow, jCol);
		int idx = hessianM->getUnzeroNum();
		for (int i = offset; i < n; i++, idx++) {
            iRow[idx] = i;
            jCol[idx] = i;
        }		
	} else {
		Hessian hessian(model->measVector, &(model->y), state);	
		hessian.setMatrix(hessianM);		
		hessianM->makeZero();				
		if(getMode() == SE_MODE_VTHETA) {
			hessian.formHessian_polar(lambda);
		} else if(getMode() == SE_MODE_U){
			hessian.formHessian_rect(lambda);
		} else if(getMode() == SE_MODE_PQVTHETA) {
			for (int i = 0; i < busN; i++) {
				int busNumber = model->island->buses[i].busNumber;
				hessian._bus_p_polar(busNumber, busN, lambda[busNumber - 1]);				
				hessian._bus_q_polar(busNumber, busN, lambda[busNumber - 1 + busN]);
			}
			hessian.formHessian_fullState(lambda + 2 * busN);			
		} else if(getMode() == SE_MODE_UI) {//todo: not finished yet!
			//hessian.formHessian_ui(lambda);
		}	
		hessianM->getValues(values);

		int idx = hessianM->getUnzeroNum();
		model->obj->eval_hessian_obj(n, x, model->measInObjFunc, offset, obj_factor, values + idx);       
	}	
	if(isDebug)
		cout<<"Time used for eval hessian:"<<clock() - start<<"ms"<<endl;
	return true;
}

void IpoptSE::finalize_solution(SolverReturn status,
                                  Index n, const Number* x, const Number* z_L, const Number* z_U,
                                  Index m, const Number* g, const Number* lambda, Number obj_value, 
								  const IpoptData* ip_data, IpoptCalculatedQuantities* ip_cq) {
	if(model->finalVTheta != NULL)
		delete[] model->finalVTheta;
	int busN = model->island->busN;
	model->finalVTheta = new double[2 * busN];			
	if(getMode() == SE_MODE_VTHETA || getMode() == SE_MODE_PQVTHETA) {
		for (int i = 0; i < 2 * busN; i++) 
			model->finalVTheta[i] = x[i];
	} else if(getMode() == SE_MODE_U || getMode() == SE_MODE_UI) {		
		for (int i = 0; i < busN; i++) {
			model->finalVTheta[i] = sqrt(x[i] * x[i] + x[i + busN] * x[i + busN]);
			model->finalVTheta[i + busN] = atan2(x[i + busN], x[i]);
		}
		if(model->slackBusCol >= 0)
			for (int i = 0; i < busN; i++)
				model->finalVTheta[i + busN] += model->slackBusAngle;			
	}
}
