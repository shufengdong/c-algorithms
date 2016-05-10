#include"ObjectiveFunction.h"

using namespace cpscpu;

WLSObjFunction::WLSObjFunction() {
	noneZeroValuesInHessian = NULL;
}

WLSObjFunction::~WLSObjFunction() {
	if(noneZeroValuesInHessian != NULL)
		delete[] noneZeroValuesInHessian;
}

void WLSObjFunction::eval_hessian_obj(int n, const double* x, int* measInObjFunc, int offset, double factor, double* values) {
	for (int i = 0; i < measVector->measNum; i++)
		values[i] = factor * measVector->meas[i].weight;//todo: not consider measInObjFunc
}

bool WLSObjFunction::eval_f(int n, const double* x, bool new_x, double* obj_value, int* measInObjFunc, int offset) {
	obj_value[0] = 0;
	for (int i =0; i < measVector->measNum; i++) {
		double v = x[i + offset];
		obj_value[0] += measVector->meas[i].weight * v * v * 0.5;//todo: not consider measInObjFunc
	}
	return true;
}

bool WLSObjFunction::eval_grad_f(int n, const double* x, bool new_x, double* grad_f, int* measInObjFunc, int offset) {
	for (int i = 0; i < n; i++)
		grad_f[i] = 0;
	for (int i = 0; i < measVector->measNum; i++) {
		double v = x[i + offset];
		grad_f[i + offset] = v * measVector->meas[i].weight;//todo: not consider
	}
	return true;
}

void WLSObjFunction::setMeasVector(MeasureVector * measV){
	this->measVector = measV;	
}

