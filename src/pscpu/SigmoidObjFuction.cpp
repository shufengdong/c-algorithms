#include"ObjectiveFunction.h"
#include<math.h>

using namespace cpscpu;

SigmoidObjFunction::SigmoidObjFunction() {
	a = NULL;
	b = NULL;
	shortenRate = 1.0;
}

void SigmoidObjFunction::setA(double * a) {
	this->a = a;
}

void SigmoidObjFunction::setB(double * b) {
	this->b = b;
}

void SigmoidObjFunction::setShortenRate(double d) {
	this->shortenRate = d;
}

void SigmoidObjFunction::eval_hessian_obj(int n, const double* x, int* measInObjFunc, int offset, double factor, double* values) {
	for (int i = 0; i < n - offset; i++) {
		double v = x[i + offset];
		double y1 = 1.0 / (1.0 + exp(-b[i] * (v - a[i])));
		double y2 = 1.0 / (1.0 + exp(b[i] * (v + a[i])));
        values[i] = factor * shortenRate * b[i] * b[i] * (y1 * (1.0 - y1) * (1.0 - 2.0 * y1) + y2 * (1.0 - y2) * (1.0 - 2.0 * y2));
	}
}

bool SigmoidObjFunction::eval_f(int n, const double* x, bool new_x, double* obj_value, int* measInObjFunc, int offset) {
	obj_value[0] = 0;
	for (int i = 0; i < n - offset; i++) {
		double v = x[i + offset];
		double tmp1 = 1.0 / (1.0 + exp(-b[i] * (v - a[i])));
		double tmp2 = 1.0 / (1.0 + exp(b[i] * (v + a[i])));
        obj_value[0] += shortenRate * (tmp1 + tmp2);
	}
	return true;
}

bool SigmoidObjFunction::eval_grad_f(int n, const double* x, bool new_x, double* grad_f, int* measInObjFunc, int offset) {
	for (int i = 0; i < n; i++)
		grad_f[i] = 0;
	for (int i = 0; i < n - offset; i++) {
		double v = x[i + offset];
		double tmp1 = 1.0 / (1.0 + exp(-b[i] * (v - a[i])));
		double tmp2 = 1.0 / (1.0 + exp(b[i] * (v + a[i])));
		grad_f[i + offset] = shortenRate *  b[i] * (tmp1 * (1.0 - tmp1) - tmp2 * (1.0 - tmp2));
	}
	return true;
}

