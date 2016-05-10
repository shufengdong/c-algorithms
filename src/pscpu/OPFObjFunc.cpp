#include "OPFObjFunc.h"

using namespace cpscpu;

OPFObjFunc::OPFObjFunc() {
	opfMode = OPF_MODE_PQVTHETA;
	model = NULL;
}

OPFObjFunc::OPFObjFunc(OPFModel * model) {
	opfMode = OPF_MODE_PQVTHETA;
	setModel(model);
}

OPFObjFunc::OPFObjFunc(OPFModel * model,int opf_mode) {
	setOPFMode(opf_mode);
	setModel(model);
}

void OPFObjFunc::setModel(OPFModel * model) {
	this->model = model;
}

OPFModel * OPFObjFunc::getModel() {
	return model;
}

void OPFObjFunc::setOPFMode(int mode) {
	this->opfMode = mode;
}

void MinPGen::eval_hessian_obj(int n, const double* x, double factor, AbstractSM * result) {
	return;
}

		// to estimate the objective function
bool MinPGen::eval_f(int n, const double* x, bool new_x, double* obj_value) {
	obj_value[0] = 0;
	if(opfMode = OPF_MODE_PQVTHETA) {
		for (int i = 0; i < model->island->busN; i++) {
			if (model->island->buses[i].busType == 2) {
				obj_value[0] += x[model->island->buses[i].busNumber - 1 + 2 * model->island->busN];
			}
		}
	}
	return true;
}

bool MinPGen::eval_grad_f(int n, const double* x, bool new_x, double* grad_f) {
	for(int i = 0; i < n; i++) 
		grad_f[i] = 0;
	if(opfMode = OPF_MODE_PQVTHETA) {
		for (int i = 0; i < model->island->busN; i++) {
			if (model->island->buses[i].busType == 2) {
				grad_f[model->island->buses[i].busNumber - 1 + 2 * model->island->busN] = 1;
			}
		}
	}
	return true;

}