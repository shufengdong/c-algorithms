#include <math.h>
#include <stdlib.h>
#include "MeasureConstants.h"
#include "StateCalculator.h"

using namespace std;
using namespace cpscpu;

void StateCalculator::getEstimatedZ_fullState(const MeasureVector *meas, YMatrix * y, const double*  state, double * result) {
	int n = y->admittance[0].getM();
	for(int i = 0; i < meas->measNum; i++) {
		int num = atoi(meas->meas[i].positionId.c_str());		
		switch((*meas).meas[i].measureType) {
			case TYPE_BUS_ANGLE:
				result[i] = state[num - 1 + n];
				break;
			case TYPE_BUS_VOLTAGE:
				result[i] = state[num - 1];
				break;
			case TYPE_BUS_ACTIVE_POWER:
				result[i] = state[num - 1 + 2 * n];
				break;
			case TYPE_BUS_REACTIVE_POWER:
				result[i] = state[num - 1 + 3 * n];
				break;
			case TYPE_LINE_FROM_ACTIVE:
				result[i] = calLinePFrom_polar(num, y, state);
				break;
			case TYPE_LINE_FROM_REACTIVE:
				result[i] = calLineQFrom_polar(num, y, state);
				break;
			case TYPE_LINE_TO_ACTIVE:
				result[i] = calLinePTo_polar(num, y, state);
				break;
			case TYPE_LINE_TO_REACTIVE:
				result[i] = calLineQTo_polar(num, y, state);
				break;
			default:
				cout<<"<StateCalculator>unsupported measure type: "<<meas->meas[i].measureType<<endl;//todo: not good
				break;
		}
	}
}

void StateCalculator::getEstimatedZ_polar(const MeasureVector *meas, YMatrix * y, const double*  state, double * result) {
	int n = y->admittance[0].getM();
	for(int i = 0; i < meas->measNum; i++) {
		int num = atoi(meas->meas[i].positionId.c_str());		
		switch((*meas).meas[i].measureType) {
			case TYPE_BUS_ANGLE:
				result[i] = state[num - 1 + n];
				break;
			case TYPE_BUS_VOLTAGE:
				result[i] = state[num - 1];
				break;
			case TYPE_BUS_ACTIVE_POWER:
				result[i] = calBusP_polar(num, y, state);
				break;
			case TYPE_BUS_REACTIVE_POWER:
				result[i] = calBusQ_polar(num, y, state);
				break;
			case TYPE_LINE_FROM_ACTIVE:
				result[i] = calLinePFrom_polar(num, y, state);
				break;
			case TYPE_LINE_FROM_REACTIVE:
				result[i] = calLineQFrom_polar(num, y, state);
				break;
			case TYPE_LINE_TO_ACTIVE:
				result[i] = calLinePTo_polar(num, y, state);
				break;
			case TYPE_LINE_TO_REACTIVE:
				result[i] = calLineQTo_polar(num, y, state);
				break;
			default:
				cout<<"<StateCalculator>unsupported measure type: "<<meas->meas[i].measureType<<endl;//todo: not good
				break;
		}
	}
}

void StateCalculator::getEstimatedZ_rect(MeasureVector *meas, YMatrix * y, const double*  state, double * result) {
	int n = y->admittance[0].getM();
	for(int i = 0; i < meas->measNum; i++) {
		int num = atoi(meas->meas[i].positionId.c_str());		
		switch(meas->meas[i].measureType) {
			case TYPE_BUS_ANGLE:
				result[i] = atan2(state[num - 1 + n], state[num - 1]);
				break;
			case TYPE_BUS_VOLTAGE:
				result[i] = state[num - 1] * state[num - 1] + state[num - 1 + n] * state[num - 1 + n];
				break;
			case TYPE_BUS_ACTIVE_POWER:
				result[i] = calBusP_rect(num, y, state);
				break;
			case TYPE_BUS_REACTIVE_POWER:
				result[i] = calBusQ_rect(num, y, state);
				break;
			case TYPE_LINE_FROM_ACTIVE:
				result[i] = calLinePFrom_rect(num, y, state);
				break;
			case TYPE_LINE_FROM_REACTIVE:
				result[i] = calLineQFrom_rect(num, y, state);
				break;
			case TYPE_LINE_TO_ACTIVE:
				result[i] = calLinePTo_rect(num, y, state);
				break;
			case TYPE_LINE_TO_REACTIVE:
				result[i] = calLineQTo_rect(num, y, state);
				break;
			case TYPE_BUS_E:
				result[i] = state[num - 1];
                break;
			case TYPE_BUS_F:
				result[i] = state[num + n - 1];
                break;
			default:
				cout<<"<StateCalculator>unsupported measure type: "<<(*meas).meas[i].measureType<<endl;//todo: not good
				break;
		}
	}
}

double StateCalculator::calBusP_polar(int num, YMatrix *y, const double* state) {
	num = num - 1;
	int n = y->admittance[0].getM();
	double value = 0.0;
	MatrixNode * current1 = y->admittance[0].headNode[num].right;
	MatrixNode * current2 = y->admittance[1].headNode[num].right;
	while(current1 != y->admittance[0].headNode + num){
		int j = current1->triple.col;
		double gij = current1->triple.value;
		double bij = current2->triple.value;
		double thetaIJ = state[num + n] - state[j + n];
		//if(num == 151)
		//	cout<<gij<<"\t"<<bij<<"\t"<<thetaIJ<<"\t"<<state[num]<<"\t"<<state[j]<<"\t"<<j<<endl;

		value += state[num] * state[j] * (gij * cos(thetaIJ) + bij * sin(thetaIJ));
		current1 = current1->right;
		current2 = current2->right;
	}
	return value;
}

double StateCalculator::calBusQ_polar(int num, YMatrix * y, const double* state) {
	num = num - 1;
	int n = y->admittance[0].getM();
	double value = 0.0;
	MatrixNode * current1 = y->admittance[0].headNode[num].right;
	MatrixNode * current2 = y->admittance[1].headNode[num].right;
	while(current1 != y->admittance[0].headNode + num){
		int j = current1->triple.col;
		double gij = current1->triple.value;
		double bij = current2->triple.value;
		double thetaIJ = state[num + n] - state[j + n];

		value += state[num] * state[j] * (gij * sin(thetaIJ) - bij * cos(thetaIJ));
		current1 = current1->right;
		current2 = current2->right;
	}
	return value;
}

double StateCalculator::calLinePFrom_polar(int branchId, YMatrix *y, const double* state) {
	int n = y->admittance[0].getM();
	int i = y->lineFrom[branchId - 1];
	int j = y->lineTo[branchId - 1];
	double* gbg1b1 = y->lineFromAdmittance + 4 * (branchId - 1);
	double thetaIJ = state[i + n] - state[j + n];
	return state[i] * state[i] * (gbg1b1[0] + gbg1b1[2]) - state[i] * state[j] * (gbg1b1[0] * cos(thetaIJ)
		+ gbg1b1[1] * sin(thetaIJ));
}

double StateCalculator::calLinePTo_polar(int branchId, YMatrix *y, const double* state) {
	int n = y->admittance[0].getM();
	int i = y->lineFrom[branchId - 1];
	int j = y->lineTo[branchId - 1];
	double* gbg1b1 = y->lineToAdmittance + 4 * (branchId - 1);
	double thetaIJ = state[j + n] - state[i + n];
	return state[j] * state[j] * (gbg1b1[0] + gbg1b1[2]) - state[i] * state[j] * (gbg1b1[0] * cos(thetaIJ)
		+ gbg1b1[1] * sin(thetaIJ));
}

double StateCalculator::calLineQFrom_polar(int branchId, YMatrix *y, const double* state) {
	int n = y->admittance[0].getM();
	int i = y->lineFrom[branchId - 1];
	int j = y->lineTo[branchId - 1];
	double* gbg1b1 = y->lineFromAdmittance + 4 * (branchId - 1);
	double thetaIJ = state[i + n] - state[j + n];
	return -state[i] * state[i] * (gbg1b1[1] + gbg1b1[3]) - state[i] * state[j] * (gbg1b1[0] * sin(thetaIJ)
		- gbg1b1[1] * cos(thetaIJ));
}

double StateCalculator::calLineQTo_polar(int branchId, YMatrix *y, const double* state) {
	int n = y->admittance[0].getM();
	int i = y->lineFrom[branchId - 1];
	int j = y->lineTo[branchId - 1];
	double* gbg1b1 = y->lineToAdmittance + 4 * (branchId - 1);
	double thetaIJ = state[j + n] - state[i + n];
	return -state[j] * state[j] * (gbg1b1[1] + gbg1b1[3]) - state[i] * state[j] * (gbg1b1[0] * sin(thetaIJ)
		- gbg1b1[1] * cos(thetaIJ));
}


double StateCalculator::calBusP_rect(int num, YMatrix *y, const double* state) {
	num = num - 1;
	int n = y->admittance[0].getM();
	double value = 0.0;
	MatrixNode * current1 = y->admittance[0].headNode[num].right;
	MatrixNode * current2 = y->admittance[1].headNode[num].right;
	while(current1 != y->admittance[0].headNode + num){
		int j = current1->triple.col;
		double gij = current1->triple.value;
		double bij = current2->triple.value;		
		value += state[num] * (gij * state[j] - bij * state[j + n]) + state[num + n] * (gij * state[j+n] + bij*state[j]);
		current1 = (*current1).right;
		current2 = (*current2).right;
	}
	return value;
}

double StateCalculator::calBusQ_rect(int num, YMatrix * y, const double* state) {
	num = num - 1;
	int n = y->admittance[0].getM();
	double value = 0.0;
	MatrixNode * current1 = y->admittance[0].headNode[num].right;
	MatrixNode * current2 = y->admittance[1].headNode[num].right;
	while(current1 != y->admittance[0].headNode + num){
		int j = current1->triple.col;
		double gij = current1->triple.value;
		double bij = current2->triple.value;

		value += state[num + n] * (gij * state[j] - bij * state[j + n]) - state[num] * (gij * state[j + n] + bij * state[j]);
		current1 = current1->right;
		current2 = current2->right;
	}
	return value;
}

double StateCalculator::calLinePFrom_rect(int branchId, YMatrix *y, const double* state) {
	int n = y->admittance[0].getM();
	int i = y->lineFrom[branchId - 1];
	int j = y->lineTo[branchId - 1];
	double* gbg1b1 = y->lineFromAdmittance + 4 * (branchId - 1);

	return (gbg1b1[0] + gbg1b1[2]) * (state[i] * state[i] + state[i+n] * state[i+n])  
		+ gbg1b1[1] * state[j+n] * state[i]
	- gbg1b1[0] * state[j] * state[i]
	- gbg1b1[1] * state[j] * state[i+n]
	- gbg1b1[0] * state[j+n] * state[i+n];
}

double StateCalculator::calLinePTo_rect(int branchId, YMatrix *y, const double* state) {
	int n = y->admittance[0].getM();
	int j = y->lineFrom[branchId - 1];
	int i = y->lineTo[branchId - 1];
	double* gbg1b1 = y->lineToAdmittance + 4 * (branchId - 1);

	return (gbg1b1[0] + gbg1b1[2]) * (state[i] * state[i] + state[i+n] * state[i+n])  
	+ gbg1b1[1] * state[j+n] * state[i]
	- gbg1b1[0] * state[j] * state[i]
	- gbg1b1[1] * state[j] * state[i+n]
	- gbg1b1[0] * state[j+n] * state[i+n];
}

double StateCalculator::calLineQFrom_rect(int branchId, YMatrix *y, const double* state) {
	int n =  y->admittance[0].getM();
	int i =  y->lineFrom[branchId - 1];
	int j =  y->lineTo[branchId - 1];
	double* gbg1b1 =  y->lineFromAdmittance + 4 * (branchId - 1);

	return -(gbg1b1[1] + gbg1b1[3]) * (state[i] * state[i] + state[i+n] * state[i+n])  
		+ gbg1b1[1] * state[j] * state[i]
	+ gbg1b1[0] * state[j+n] * state[i]
	- gbg1b1[0] * state[j] * state[i+n]
	+ gbg1b1[1] * state[j+n] * state[i+n];
}

double StateCalculator::calLineQTo_rect(int branchId, YMatrix *y, const double* state) {
	int n = y->admittance[0].getM();
	int j = y->lineFrom[branchId - 1];
	int i = y->lineTo[branchId - 1];
	double* gbg1b1 = y->lineToAdmittance + 4 * (branchId - 1);
	return -(gbg1b1[1] + gbg1b1[3]) * (state[i] * state[i] + state[i+n] * state[i+n])  
		+ gbg1b1[1] * state[j] * state[i]
	+ gbg1b1[0] * state[j+n] * state[i]
	- gbg1b1[0] * state[j] * state[i+n]
	+ gbg1b1[1] * state[j+n] * state[i+n];
}

//for current injection with hession modify model
double* StateCalculator::getEstimatedZ_CIHM_rect(const MeasureVector *meas, YMatrix * y, const double*  state) {
	double * result = new double[meas->measNum];	//todo : not new space here
	int n = y->admittance[0].getN();
	for(int i = 0; i < meas->measNum; i++) {
		int num = atoi(meas->meas[i].positionId.c_str());	//busNum	
		switch((*meas).meas[i].measureType) {
			case TYPE_BUS_NetReal:
				result[i] = calBusNetReal_CIHM_rect(num, y, state);
				break;
			case TYPE_BUS_NetImage:
				result[i] = calBusNetImage_CIHM_rect(num, y, state);
				break;
			case TYPE_BUS_VOLTAGE :
				result[i] = calBusV2_CIHM_rect(num, n, state);
				break;
			case TYPE_BUS_ACTIVE_POWER :
				result[i] = calBusP_CIHM_rect(num, n, state);
				break;
			case TYPE_BUS_REACTIVE_POWER :
				result[i] = calBusQ_CIHM_rect(num, n, state);
				break;
			case TYPE_BUS_E:
				result[i] = state[num - 1];
				break;
			case TYPE_BUS_F:
				result[i] = state[num - 1 + n ];
				break;
			case TYPE_BUS_Ix:
				result[i] = state[num - 1 + 2*n ];
				break;
			case TYPE_BUS_Iy:
				result[i] = state[num - 1 + 3*n ];
				break;
			default:
				cout<<"<StateCalculator>unsupported measure type: "<<(*meas).meas[i].measureType<<endl;
				break;
		}
	}
	return result;
}

double StateCalculator::calBusNetReal_CIHM_rect(int num, YMatrix *y, const double* state){
	int n = y->admittance[0].getN();
	int i = num - 1; // busNum - 1
	int j = 0;
	//-9999  matrix.getValue
	double freal = 0.0;
	double g,b;
	MatrixNode * currentG = y->admittance[0].headNode[i].right;
	while(currentG != y->admittance[0].headNode + i) {
		g = currentG->triple.value;
		j = currentG->triple.col;	//zero_based index
		freal += g * state[ j ];	//ej
		currentG = currentG->right;
	}
	MatrixNode * currentB = y->admittance[1].headNode[i].right;
	while(currentB != y->admittance[1].headNode + i) {
		b = currentB->triple.value;
		j = currentB->triple.col;	//zero_based index
		freal -= b * state[ j + n ];	//fj
		currentB = currentB->right;
	}
	freal -= state[ i + 2*n ];	//Ix_i

	return freal;
}

double StateCalculator::calBusNetImage_CIHM_rect(int num, YMatrix *y, const double* state){
	int n = y->admittance[0].getN();
	int i = num - 1; // busNum - 1
	int j = 0;
	//-9999  matrix.getValue
	double fimage = 0.0;
	double g,b;
	MatrixNode * currentG = y->admittance[0].headNode[i].right;
	while(currentG != y->admittance[0].headNode + i) {
		g = currentG->triple.value;
		j = currentG->triple.col;	//zero_based index
		fimage += g * state[ j + n ];	//fj
		currentG = currentG->right;
	}
	MatrixNode * currentB = y->admittance[1].headNode[i].right;
	while(currentB != y->admittance[1].headNode + i) {
		b = currentB->triple.value;
		j = currentB->triple.col;	//zero_based index
		fimage += b * state[ j ];	//ej
		currentB = currentB->right;
	}
	fimage -= state[ i + 3*n ];	//Iy_i

	return fimage;
}

double StateCalculator::calBusP_CIHM_rect(int num, int n, const double* state){
	int i = num - 1; //busNum - 1;
	double e,f,ix,iy;
	e = state[ i ];	
	f = state[ i + n ];
	ix = state[ i + 2*n ];
	iy = state[ i + 3*n ];
	return e*ix + f*iy ;
}

double StateCalculator::calBusQ_CIHM_rect(int num, int n, const double* state){
	int i = num - 1; //busNum - 1;
	double e,f,ix,iy;
	e = state[ i ];	
	f = state[ i + n ];
	ix = state[ i + 2*n ];
	iy = state[ i + 3*n ];
	return - e*iy + f*ix ;
}

double StateCalculator::calBusV2_CIHM_rect(int num, int n, const double* state){
	int i = num - 1; //busNum - 1;
	double e,f;
	e = state[ i ];	
	f = state[ i + n ];
	return e*e + f*f ;
}
