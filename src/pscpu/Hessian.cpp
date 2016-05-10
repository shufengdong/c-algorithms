#include<math.h>
#include<stdlib.h>
#include"Hessian.h"
#include"MeasureConstants.h"
#include"StateCalculator.h"

using namespace cpscpu;

Hessian::Hessian(MeasureVector *meas, YMatrix *y, const double *state) {
	this->meas = meas;
	this->y = y;
	this->state = state;
}

void Hessian::formHessian_polar() {
	double * tmp = new double[meas->measNum];
	for(int i = 0;i < meas->measNum; i++)
		tmp[i] = 1.0;
	formHessian_polar(tmp);
	delete[] tmp;
}

void Hessian::formHessian_polar(const double * lamda) {
	int n = y->admittance[0].getM();
	for(int i = 0; i < meas->measNum; i++) {
		int num = atoi(meas->meas[i].positionId.c_str());		
		switch(meas->meas[i].measureType) {
			case TYPE_BUS_ANGLE:
				break;
			case TYPE_BUS_VOLTAGE:
				break;
			case TYPE_BUS_ACTIVE_POWER:
				_bus_p_polar(num, n, lamda[i]);
				break;
			case TYPE_BUS_REACTIVE_POWER:
				_bus_q_polar(num, n, lamda[i]);
				break;
			case TYPE_LINE_FROM_ACTIVE:
				_line_from_p_polar(num, n, lamda[i]);
				break;
			case TYPE_LINE_FROM_REACTIVE:
				_line_from_q_polar(num, n, lamda[i]);
				break;
			case TYPE_LINE_TO_ACTIVE:
				_line_to_p_polar(num, n, lamda[i]);
				break;
			case TYPE_LINE_TO_REACTIVE:
				_line_to_q_polar(num, n, lamda[i]);
				break;
			default:
				cout<<"<Hessian>unsupported measure type: "<<(*meas).meas[i].measureType<<endl;
				break;
		}
	}
}

void Hessian::_bus_p_polar(int pos, int n, double w){
	pos = pos - 1;
	MatrixNode * current1 = y->admittance[0].headNode[pos].right;
	MatrixNode * current2 = y->admittance[1].headNode[pos].right;
	while(current1 != y->admittance[0].headNode + pos) {
		int j = current1->triple.col;
		double gij = current1->triple.value;
		double bij = current2->triple.value;
		double thetaIJ = state[pos + n] - state[j + n];
		double sinT = sin(thetaIJ);
		double cosT = cos(thetaIJ);
		if (j == pos) {
			matrix->setValue(pos, pos, 2 * w * y->admittance[0].getValue(pos, pos), true);
			double mij = -state[pos] * state[j] * (gij * cosT + bij * sinT);
			double v = -bij * state[pos] * state[pos];
			v -= StateCalculator::calBusQ_polar(pos + 1, y, state);
			mij += StateCalculator::calBusP_polar(pos + 1, y, state);
			matrix->setValue(pos + n, pos, v * w / state[pos], true);
			matrix->setValue(pos + n, pos + n, -mij * w, true);
		} else {
			double v = state[pos] * (gij * cosT + bij * sinT);
			double v1 = v / state[pos];
			if (j > pos) 
				matrix->setValue(j, pos, v1 * w, true);
			else
				matrix->setValue(pos, j, v1 * w, true);
			double mij = -state[pos] * state[j] * (gij * cosT + bij * sinT);
			v = state[pos] * state[j] * (gij * sinT - bij * cosT);
			v1 = v / state[j];
			matrix->setValue(j + n, j, v1 * w, true);
			matrix->setValue(pos + n, j, -v1 * w, true);
			matrix->setValue(j + n, pos, v * w / state[pos], true);
			matrix->setValue(j + n, j + n, mij * w, true);
			if (pos > j) matrix->setValue(pos + n, j + n, -mij * w, true);
			else matrix->setValue(j + n, pos + n, -mij * w, true);
		}
		current1 = current1->right;
		current2 = current2->right;
	}
}

void Hessian::_bus_q_polar(int pos, int n, double w) {
	pos = pos - 1;
	MatrixNode * current1 = y->admittance[0].headNode[pos].right;
	MatrixNode * current2 = y->admittance[1].headNode[pos].right;
	while(current1 != y->admittance[0].headNode + pos) {
		int j = current1->triple.col;
		double gij = current1->triple.value;
		double bij = current2->triple.value;
		double thetaIJ = state[pos + n] - state[j + n];
		double sinT = sin(thetaIJ);
		double cosT = cos(thetaIJ);

		if (j == pos) {
			matrix->setValue(pos, pos, -2 * w * y->admittance[1].getValue(pos, pos), true);
			double hij = state[pos] * state[j] * (gij * sinT - bij * cosT);
			double v = -gij * state[pos] * state[pos];
			v += StateCalculator::calBusP_polar(pos + 1, y, state);
			hij -= StateCalculator::calBusQ_polar(pos + 1, y, state);
			matrix->setValue(pos + n, pos, v * w / state[pos], true);
			matrix->setValue(pos + n, pos + n, hij * w, true);
		} else {
			double v = state[pos] * (gij * sinT - bij * cosT);
			double v1 = v / state[pos];
			if (j > pos) 
				matrix->setValue(j, pos, v1 * w, true);
			else
				matrix->setValue(pos, j, v1 * w, true);
			
			double hij = state[pos] * state[j] * (gij * sinT - bij * cosT);
			v = -state[pos] * state[j] * (gij * cosT + bij * sinT);
			v1 = v / state[j];
			matrix->setValue(j + n, j, v1 * w, true);
			matrix->setValue(pos + n, j, -v1 * w, true);
			matrix->setValue(j + n, pos, v * w / state[pos], true);
			matrix->setValue(j + n, j + n, -hij * w, true);
			if (pos > j) matrix->setValue(pos + n, j + n, hij * w, true);
			else matrix->setValue(j + n, pos + n, hij * w, true);
		}
		current1 = current1->right;
		current2 = current2->right;
	}
}

void Hessian::_line_from_p_polar(int branchId, int n, double w) {
	int i = y->lineFrom[branchId - 1];
	int j = y->lineTo[branchId - 1];
	double* gbg1b1 = y->lineFromAdmittance + 4 * (branchId - 1);
	double thetaIJ = state[i + n] - state[j + n];
	double cosT = cos(thetaIJ);
	double sinT = sin(thetaIJ);

	matrix->setValue(i, i, w * 2 * (gbg1b1[0] + gbg1b1[2]), true);
	matrix->setValue(i + n, i, w * state[j] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true);
	matrix->setValue(i + n, j, w * state[i] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true);
	matrix->setValue(i + n, i + n, w * state[i] * state[j] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);
	matrix->setValue(j + n, i, -w * state[j] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true);
	matrix->setValue(j + n, j, -w * state[i] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true);
	matrix->setValue(j + n, j + n, w * state[i] * state[j] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);       

	if(i > j) {
		matrix->setValue(i, j, -w * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);
		matrix->setValue(i + n, j + n, -w * state[i] * state[j] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);
	} else {
		matrix->setValue(j, i, -w * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);
		matrix->setValue(j + n, i + n, -w * state[i] * state[j] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);
	} 
}

void Hessian::_line_from_q_polar(int branchId, int n, double w) {
	int i = y->lineFrom[branchId - 1];
	int j = y->lineTo[branchId - 1];
	double* gbg1b1 = y->lineFromAdmittance + 4 * (branchId - 1);
	double thetaIJ = state[i + n] - state[j + n];
	double cosT = cos(thetaIJ);
	double sinT = sin(thetaIJ);

	matrix->setValue(i, i, -w * 2 * (gbg1b1[1] + gbg1b1[3]), true);
	matrix->setValue(i + n, i, -w * state[j] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);
	matrix->setValue(i + n, j, -w * state[i] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);
	matrix->setValue(i + n, i + n, w * state[i] * state[j] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true);
	matrix->setValue(j + n, i, w * state[j] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);
	matrix->setValue(j + n, j, w * state[i] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);
	matrix->setValue(j + n, j + n, w * state[i] * state[j] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true); 

	if(i > j) {
		matrix->setValue(i, j, -w * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true);
		matrix->setValue(i + n, j + n, -w * state[i] * state[j] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true);
	} else {
		matrix->setValue(j, i, -w * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true);
		matrix->setValue(j + n, i + n, -w * state[i] * state[j] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true);
	}    
}

void Hessian::_line_to_p_polar(int branchId, int n, double w) {
	int j = y->lineFrom[branchId - 1];
	int i = y->lineTo[branchId - 1];
	double* gbg1b1 = y->lineToAdmittance + 4 * (branchId - 1);
	double thetaIJ = state[i + n] - state[j + n];
	double cosT = cos(thetaIJ);
	double sinT = sin(thetaIJ);

	matrix->setValue(i, i, w * 2 * (gbg1b1[0] + gbg1b1[2]), true);
	matrix->setValue(i + n, i, w * state[j] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true);
	matrix->setValue(i + n, j, w * state[i] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true);
	matrix->setValue(i + n, i + n, w * state[i] * state[j] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);
	matrix->setValue(j + n, i, -w * state[j] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true);
	matrix->setValue(j + n, j, -w * state[i] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true);
	matrix->setValue(j + n, j + n, w * state[i] * state[j] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);       

	if(i > j) {
		matrix->setValue(i, j, - w * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);
		matrix->setValue(i + n, j + n, -w * state[i] * state[j] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);
	} else {
		matrix->setValue(j, i, -w * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);
		matrix->setValue(j + n, i + n, -w *state[i] * state[j] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);
	} 
}

void Hessian::_line_to_q_polar(int branchId, int n, double w) {
	int j = y->lineFrom[branchId - 1];
	int i = y->lineTo[branchId - 1];
	double* gbg1b1 = y->lineToAdmittance + 4 * (branchId - 1);
	double thetaIJ = state[i + n] - state[j + n];
	double cosT = cos(thetaIJ);
	double sinT = sin(thetaIJ);

	matrix->setValue(i, i, -w * 2 * (gbg1b1[1] + gbg1b1[3]), true);
	matrix->setValue(i + n, i, -w * state[j] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);
	matrix->setValue(i + n, j, -w * state[i] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);
	matrix->setValue(i + n, i + n, w * state[i] * state[j] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true);
	matrix->setValue(j + n, i, w * state[j] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);
	matrix->setValue(j + n, j, w * state[i] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT), true);
	matrix->setValue(j + n, j + n, w * state[i] * state[j] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true); 

	if(i > j) {
		matrix->setValue(i, j, -w * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true);
		matrix->setValue(i + n, j + n, -w * state[i] * state[j] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true);
	} else {
		matrix->setValue(j, i, -w * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true);
		matrix->setValue(j + n, i + n, -w * state[i] * state[j] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT), true);
	}    
}

void Hessian::formHessian_fullState() {
	double * tmp = new double[meas->measNum];
	for(int i = 0;i < meas->measNum; i++)
		tmp[i] = 1.0;
	formHessian_fullState(tmp);
	delete[] tmp;
}

void Hessian::formHessian_fullState(const double * lamda) {	
	int n = y->admittance[0].getM();
	for(int i = 0; i < meas->measNum; i++) {
		int num = atoi(meas->meas[i].positionId.c_str());		
		switch(meas->meas[i].measureType) {
			case TYPE_BUS_ANGLE:
				break;
			case TYPE_BUS_VOLTAGE:
				break;
			case TYPE_BUS_ACTIVE_POWER:
				break;
			case TYPE_BUS_REACTIVE_POWER:
				break;
			case TYPE_LINE_FROM_ACTIVE:
				_line_from_p_polar(num, n, lamda[i]);
				break;
			case TYPE_LINE_FROM_REACTIVE:
				_line_from_q_polar(num, n, lamda[i]);
				break;
			case TYPE_LINE_TO_ACTIVE:
				_line_to_p_polar(num, n, lamda[i]);
				break;
			case TYPE_LINE_TO_REACTIVE:
				_line_to_q_polar(num, n, lamda[i]);
				break;
			default:
				cout<<"<Hessian>unsupported measure type: "<<meas->meas[i].measureType<<endl;
				break;
		}
	}
}

void Hessian::formHessian_rect() {
	double * tmp = new double[meas->measNum];
	for(int i = 0;i < meas->measNum; i++)
		tmp[i] = 1.0;
	formHessian_rect(tmp);
	delete[] tmp;
}

void Hessian::formHessian_rect(const double * lamda){
	int n = y->admittance[0].getM();
	for(int i = 0; i < meas->measNum; i++) {
		int num = atoi(meas->meas[i].positionId.c_str());		
		switch(meas->meas[i].measureType) {
			case TYPE_BUS_ANGLE:
				break;
			case TYPE_BUS_VOLTAGE:
				_bus_v_rect(num, n, lamda[i]);
				break;
			case TYPE_BUS_ACTIVE_POWER:
				_bus_p_rect(num, n, lamda[i]);
				break;
			case TYPE_BUS_REACTIVE_POWER:
				_bus_q_rect(num, n, lamda[i]);
				break;
			case TYPE_LINE_FROM_ACTIVE:
				_line_from_p_rect(num, n, lamda[i]);
				break;
			case TYPE_LINE_FROM_REACTIVE:
				_line_from_q_rect(num, n, lamda[i]);
				break;
			case TYPE_LINE_TO_ACTIVE:
				_line_to_p_rect(num, n, lamda[i]);
				break;
			case TYPE_LINE_TO_REACTIVE:
				_line_to_q_rect(num, n, lamda[i]);
				break;
			case TYPE_BUS_E:
				break;
			case TYPE_BUS_F:
				break;
			default:
				cout<<"<Hessian>unsupported measure type: "<<(*meas).meas[i].measureType<<endl;
				break;
		}
	}
}

void Hessian::_bus_p_rect(int pos, int n, double w){
	pos = pos - 1;
	MatrixNode * current1 = y->admittance[0].headNode[pos].right;
	MatrixNode * current2 = y->admittance[1].headNode[pos].right;
	while(current1 != y->admittance[0].headNode + pos) {
		int j = current1->triple.col;
		double gij = current1->triple.value;
		double bij = current2->triple.value;
		if (pos == j) {
			matrix->setValue(pos, pos, 2 * gij * w, true);		
			matrix->setValue(pos + n, pos + n, 2 * gij * w, true);		
		} else if(j > pos) {
			matrix->setValue(j, pos, gij * w, true);
			matrix->setValue(j + n, pos + n, gij * w, true);
			matrix->setValue(pos + n, j, bij * w, true);
			matrix->setValue(j + n, pos, -bij * w, true);
		} else{
			matrix->setValue(pos, j, gij * w, true);
			matrix->setValue(pos + n, j + n, gij * w, true);
			matrix->setValue(pos + n, j, bij * w, true);
			matrix->setValue(j + n, pos, -bij * w, true);
		}
		current1 = current1->right;
		current2 = current2->right;
	}
}

void Hessian::_bus_q_rect(int pos, int n, double w){
	pos = pos - 1;
	MatrixNode * current1 = y->admittance[0].headNode[pos].right;
	MatrixNode * current2 = y->admittance[1].headNode[pos].right;
	while(current1 != y->admittance[0].headNode + pos) {
		int j = current1->triple.col;
		double gij = current1->triple.value;
		double bij = current2->triple.value;
		if (pos == j) {
			matrix->setValue(pos, pos, -2 * bij * w, true);		
			matrix->setValue(pos + n, pos + n, -2 * bij * w, true);		
		} else if(j > pos) {
			matrix->setValue(j, pos, -bij * w, true);
			matrix->setValue(j + n, pos + n, -bij * w, true);
			matrix->setValue(pos + n, j, gij * w, true);
			matrix->setValue(j + n, pos, -gij * w, true);
		} else{
			matrix->setValue(pos, j, -bij * w, true);
			matrix->setValue(pos + n, j + n, -bij * w, true);
			matrix->setValue(pos + n, j, gij * w, true);
			matrix->setValue(j + n, pos, -gij * w, true);
		} 
		current1 = current1->right;
		current2 = current2->right;
	}
}

void Hessian::_bus_v_rect(int pos, int n, double w){
	matrix->setValue(pos - 1, pos - 1, 2 * w, true);		
	matrix->setValue(pos - 1 + n, pos - 1 + n, 2 * w, true);		
}

void Hessian::_line_from_p_rect(int branchId, int n, double w) {
	int i = y->lineFrom[branchId - 1];
	int j = y->lineTo[branchId - 1];
	double* gbg1b1 = y->lineFromAdmittance + 4 * (branchId - 1);
	matrix->setValue(i, i, w * 2 * (gbg1b1[0] + gbg1b1[2]), true);
	matrix->setValue(i + n, i + n, w * 2 * (gbg1b1[0] + gbg1b1[2]), true);
	matrix->setValue(i + n, j, -w * gbg1b1[1], true);
	matrix->setValue(j + n, i, w * gbg1b1[1], true);
	if (i > j) {
		matrix->setValue(i, j, -w * gbg1b1[0], true);
		matrix->setValue(i + n, j + n, -w * gbg1b1[0], true);
	} else {
		matrix->setValue(j, i, -w * gbg1b1[0], true);
		matrix->setValue(j + n, i + n, -w * gbg1b1[0], true);
	}
}

void Hessian::_line_from_q_rect(int branchId, int n, double w) {
	int i = y->lineFrom[branchId - 1];
	int j = y->lineTo[branchId - 1];
	double* gbg1b1 = y->lineFromAdmittance + 4 * (branchId - 1);
	matrix->setValue(i, i, -2 * w * (gbg1b1[1] + gbg1b1[3]), true);
	matrix->setValue(i + n, i + n, -2 * w * (gbg1b1[1] + gbg1b1[3]), true);
	matrix->setValue(i + n, j, -w * gbg1b1[0], true);
	matrix->setValue(j + n, i, w * gbg1b1[0], true);
	if (i > j) {
		matrix->setValue(i, j, w * gbg1b1[1], true);
		matrix->setValue(i + n, j + n, w * gbg1b1[1], true);
	} else {
		matrix->setValue(j, i, w * gbg1b1[1], true);
		matrix->setValue(j + n, i + n, w * gbg1b1[1], true);
	}
}

void Hessian::_line_to_p_rect(int branchId, int n, double w) {
	int j = y->lineFrom[branchId - 1];
	int i = y->lineTo[branchId - 1];
	double* gbg1b1 = y->lineToAdmittance + 4 * (branchId - 1);

	matrix->setValue(i, i, 2 * w * (gbg1b1[0] + gbg1b1[2]), true);
	matrix->setValue(i + n, i + n, 2 * w * (gbg1b1[0] + gbg1b1[2]), true);
	matrix->setValue(i + n, j, -w * gbg1b1[1], true);
	matrix->setValue(j + n, i, w * gbg1b1[1], true);
	if (i > j) {
		matrix->setValue(i, j, -w * gbg1b1[0], true);
		matrix->setValue(i + n, j + n, -w * gbg1b1[0], true);
	} else {
		matrix->setValue(j, i, -w * gbg1b1[0], true);
		matrix->setValue(j + n, i + n, -w * gbg1b1[0], true);
	}
}

void Hessian::_line_to_q_rect(int branchId, int n, double w) {
	int j = y->lineFrom[branchId - 1];
	int i = y->lineTo[branchId - 1];
	double* gbg1b1 = y->lineToAdmittance + 4 * (branchId - 1);

	matrix->setValue(i, i, -2 * w * (gbg1b1[1] + gbg1b1[3]), true);
	matrix->setValue(i + n, i + n, -2 * w * (gbg1b1[1] + gbg1b1[3]), true);
	matrix->setValue(i + n, j, -w * gbg1b1[0], true);
	matrix->setValue(j + n, i, w * gbg1b1[0], true);
	if (i > j) {
		matrix->setValue(i, j, w * gbg1b1[1], true);
		matrix->setValue(i + n, j + n, w * gbg1b1[1], true);
	} else {
		matrix->setValue(j, i, w * gbg1b1[1], true);
		matrix->setValue(j + n, i + n, w * gbg1b1[1], true);
	}
}
