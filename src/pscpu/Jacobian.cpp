
#include <math.h>
#include <iostream>
#include <cassert>
#include <stdlib.h>

#include "Jacobian.h"
#include "StateCalculator.h"
#include "MeasureConstants.h"

using namespace std;
using namespace cpscpu;

Jacobian::Jacobian(MeasureVector* meas, YMatrix * y, const double * state) {
	this->meas = meas;
	this->y = y;
	this->state = state;
}

void Jacobian::formJacobian_rect() {
	int n = y->admittance[0].getM();
	for(int i = 0; i < meas->measNum; i++) {
		int num = atoi(meas->meas[i].positionId.c_str());		
		switch(meas->meas[i].measureType) {
			case TYPE_BUS_ANGLE:
				_bus_a_rect(num, n, i);
                break;
            case TYPE_BUS_VOLTAGE:
				_bus_v_rect(num, n, i);
                break;
            case TYPE_BUS_ACTIVE_POWER:
				_bus_p_rect(num,n, i);
                break;
            case TYPE_BUS_REACTIVE_POWER:
                _bus_q_rect(num,n,i);
                break;
            case TYPE_LINE_FROM_ACTIVE:
				_line_from_p_rect(num, n, i);
                break;
            case TYPE_LINE_FROM_REACTIVE:
                _line_from_q_rect(num, n, i);
                break;
            case TYPE_LINE_TO_ACTIVE:
				_line_to_p_rect(num, n, i);
                break;
            case TYPE_LINE_TO_REACTIVE:
                _line_to_q_rect(num, n, i);
				break;
			case TYPE_BUS_E:
				_bus_Ux_rect(num, n, i);
                break;
			case TYPE_BUS_F:
				_bus_Uy_rect(num, n, i);
                break;
            default:
                cout<<"<Jacobian>unsupported measure type: "<<meas->meas[i].measureType<<endl;
                break;
		}
	}
}

void Jacobian::_bus_a_rect(int pos, int n, int currentRow){
    //matrix->setValue(currentRow, pos + n - 1, 1);
}

void Jacobian::_bus_Ux_rect(int pos, int n, int currentRow){
    matrix->setValue(currentRow, pos - 1, 1.0);
}

void Jacobian::_bus_Uy_rect(int pos, int n, int currentRow){
    matrix->setValue(currentRow, pos + n - 1, 1.0);
}

void Jacobian::_bus_v_rect(int pos, int n, int currentRow) {
	double e = state[pos - 1];
	double f = state[pos - 1 + n];
    //matrix->setValue(currentRow, pos - 1, e / sqrt(e * e + f *  f));
	//matrix->setValue(currentRow, pos - 1 + n, f / sqrt(e * e + f *  f));
	matrix->setValue(currentRow, pos - 1, 2.0 * e);
	matrix->setValue(currentRow, pos - 1 + n, 2.0 * f);
}

void Jacobian::_bus_p_rect(int pos, int n, int index){
	int num = pos - 1;//num starts from 0		

	double tmp1 = 0;
	double tmp2 = 0;
	MatrixNode * current1 = y->admittance[0].headNode[num].right;
	MatrixNode * current2 = y->admittance[1].headNode[num].right;
	double ei = state[num];
	double fi = state[num+n];
	while(current1 != y->admittance[0].headNode + num){
		int j = current1->triple.col;	
		double ej = state[j];
		double fj = state[j+n];
		double gij = current1->triple.value;
		double bij = current2->triple.value;
		if (num == j) {
			tmp1 += gij * ei + bij * fi;
			tmp2 += -bij * ei + gij * fi;
		}
		else{
			matrix->setValue(index, j, gij * ei + bij * fi);
			matrix->setValue(index, j + n, -bij * ei + gij * fi);
		}
		tmp1 += gij * ej - bij * fj;
		tmp2 += gij * fj + bij * ej;
		current1 = current1->right;
		current2 = current2->right;
	}
	matrix->setValue(index, num, tmp1);
	matrix->setValue(index, num + n, tmp2);
}

void Jacobian::_bus_q_rect(int pos, int n, int index){
	int num = pos - 1;//num starts from 0
	double tmp1 = 0;
	double tmp2 = 0;
	MatrixNode * current1 = y->admittance[0].headNode[num].right;
	MatrixNode * current2 = y->admittance[1].headNode[num].right;
	double ei = state[num];
	double fi = state[num+n];
	while(current1 != y->admittance[0].headNode + num){
		int j = current1->triple.col;		
		double ej = state[j];
		double fj = state[j+n];
		double gij = current1->triple.value;
		double bij = current2->triple.value;
		if (num == j) {
			tmp1 += -bij * ei + gij * fi;
			tmp2 += -gij * ej - bij * fj;
		}
		else{
			matrix->setValue(index, j, -bij * ei + gij * fi);
			matrix->setValue(index, j + n, -gij * ei - bij * fi);
		}
		tmp1 += -gij * fj - bij * ej;
		tmp2 += gij * ej - bij * fj;
		current1 = current1->right;
		current2 = current2->right;
	}
	matrix->setValue(index, num, tmp1);
	matrix->setValue(index, num + n, tmp2);
}

void Jacobian::_line_from_p_rect(int pos, int n, int currentRow){
	int i = y->lineFrom[pos - 1];
	int j = y->lineTo[pos - 1];
	double* gbg1b1 = y->lineFromAdmittance + 4 * (pos - 1);
	double Uxi= state[i];
	double Uxj= state[j];
	double Uyi= state[i + n];
	double Uyj= state[j + n];

	matrix->setValue(currentRow, i, 2 * (gbg1b1[0] + gbg1b1[2]) * Uxi + gbg1b1[1] * Uyj - gbg1b1[0] * Uxj);
	matrix->setValue(currentRow, i + n, 2 * (gbg1b1[0] + gbg1b1[2]) * Uyi - gbg1b1[1] * Uxj - gbg1b1[0] * Uyj);
	matrix->setValue(currentRow, j, -gbg1b1[0] * Uxi - gbg1b1[1] * Uyi);
	matrix->setValue(currentRow, j + n, gbg1b1[1] * Uxi - gbg1b1[0] * Uyi);
}

void Jacobian::_line_from_q_rect(int pos, int n, int currentRow){
	int i = y->lineFrom[pos - 1];
	int j = y->lineTo[pos - 1];
	double* gbg1b1 = y->lineFromAdmittance + 4 * (pos - 1);

	double Uxi= state[i];
	double Uxj= state[j];
	double Uyi= state[i + n];
	double Uyj= state[j + n];

	matrix->setValue(currentRow, i, -2 * (gbg1b1[1] + gbg1b1[3]) * Uxi + gbg1b1[1] * Uxj + gbg1b1[0] * Uyj);
	matrix->setValue(currentRow, i + n, -2 * (gbg1b1[1] + gbg1b1[3]) * Uyi + gbg1b1[1] * Uyj - gbg1b1[0] * Uxj);
	matrix->setValue(currentRow, j, gbg1b1[1] * Uxi - gbg1b1[0] * Uyi);
	matrix->setValue(currentRow, j + n, gbg1b1[0] * Uxi + gbg1b1[1] * Uyi);
}

void Jacobian::_line_to_p_rect(int pos, int n,int currentRow){
	int j = y->lineFrom[pos - 1];
	int i = y->lineTo[pos - 1];
	double* gbg1b1 = y->lineToAdmittance + 4 * (pos - 1);

	double Uxi= state[i];
	double Uxj= state[j];
	double Uyi= state[i + n];
	double Uyj= state[j + n];

	matrix->setValue(currentRow,i, 2 * (gbg1b1[0] + gbg1b1[2]) * Uxi + gbg1b1[1] * Uyj - gbg1b1[0] * Uxj);
	matrix->setValue(currentRow,i + n, 2 * (gbg1b1[0] + gbg1b1[2]) * Uyi - gbg1b1[1] * Uxj - gbg1b1[0] * Uyj);
	matrix->setValue(currentRow, j, -gbg1b1[0] * Uxi - gbg1b1[1] * Uyi);
	matrix->setValue(currentRow,j + n, gbg1b1[1] * Uxi - gbg1b1[0] * Uyi);
}

void Jacobian::_line_to_q_rect(int pos, int n, int currentRow){
	int j = y->lineFrom[pos - 1];
	int i = y->lineTo[pos - 1];
	double* gbg1b1 = y->lineToAdmittance + 4 * (pos - 1);

	double Uxi= state[i];
	double Uxj= state[j];
	double Uyi= state[i + n];
	double Uyj= state[j + n];

	matrix->setValue(currentRow,i, -2 * (gbg1b1[1] + gbg1b1[3]) * Uxi + gbg1b1[1] * Uxj + gbg1b1[0] * Uyj);
	matrix->setValue(currentRow, i + n, -2 * (gbg1b1[1] + gbg1b1[3]) * Uyi + gbg1b1[1] * Uyj - gbg1b1[0] * Uxj);
	matrix->setValue(currentRow,j, gbg1b1[1] * Uxi - gbg1b1[0] * Uyi);
	matrix->setValue(currentRow, j + n, gbg1b1[0] * Uxi + gbg1b1[1] * Uyi);

}

void Jacobian::formJacobian_polar() {
	int n = y->admittance[0].getM();
	for(int i = 0; i < meas->measNum; i++) {
		int num = atoi(meas->meas[i].positionId.c_str());		
		switch(meas->meas[i].measureType) {
			case TYPE_BUS_ANGLE:
				_bus_a_polar(num, n, i);
                break;
            case TYPE_BUS_VOLTAGE:
				_bus_v_polar(num, i);
                break;
            case TYPE_BUS_ACTIVE_POWER:
				_bus_p_polar(num, n, i);
                break;
            case TYPE_BUS_REACTIVE_POWER:
                _bus_q_polar(num, n, i);
                break;
            case TYPE_LINE_FROM_ACTIVE:
				_line_from_p_polar(num, n, i);
                break;
            case TYPE_LINE_FROM_REACTIVE:
                _line_from_q_polar(num, n, i);
                break;
            case TYPE_LINE_TO_ACTIVE:
				_line_to_p_polar(num, n, i);
                break;
            case TYPE_LINE_TO_REACTIVE:
                _line_to_q_polar(num, n, i);
                break;
            default:
                cout<<"<Jacobian>unsupported measure type: "<<(*meas).meas[i].measureType<<endl;
                break;
		}
	}
}

void Jacobian::_bus_a_polar(int pos, int n, int currentRow){
    matrix->setValue(currentRow, pos + n - 1, 1);
}

void Jacobian::_bus_v_polar(int pos,int currentRow) {
	matrix->setValue(currentRow, pos - 1, 1);
}

void Jacobian::_bus_p_polar(int pos, int n, int index){
    int num = pos - 1;//num starts from 0		
	double qi = 0;
	MatrixNode * current1 = y->admittance[0].headNode[num].right;
	MatrixNode * current2 = y->admittance[1].headNode[num].right;
	while(current1 != y->admittance[0].headNode + num){
		int j = current1->triple.col;
		double thetaIJ = state[num + n] - state[j + n];
		double sinT = sin(thetaIJ);
		double cosT = cos(thetaIJ);
		double gij = current1->triple.value;
		double bij = current2->triple.value;
		qi += state[num] * state[j] * (gij * sinT - bij * cosT);
		if (num == j) {
			matrix->setValue(index, num, (gij * state[num] * state[num] + StateCalculator::calBusP_polar(pos, y, state)) / state[num]);
			matrix->setValue(index, num + n, -bij * state[num] * state[num]);
			current1 = current1->right;
			current2 = current2->right;
			continue;
		}
		matrix->setValue(index, j, state[num] * (gij * cosT + bij * sinT));
		matrix->setValue(index, j + n, state[num] * state[j] * (gij * sinT - bij * cosT));
		current1 = current1->right;
		current2 = current2->right;
	}
	matrix->setValue(index, num + n, -qi, true);
}

void Jacobian::_bus_q_polar(int pos, int n, int index){
    int num = pos - 1;//num starts from 0
	double pi = 0;
	MatrixNode * current1 = y->admittance[0].headNode[num].right;
	MatrixNode * current2 = y->admittance[1].headNode[num].right;
	while(current1 != y->admittance[0].headNode + num){
		int j = current1->triple.col;
		double thetaIJ = state[num + n] - state[j + n];
		double sinT = sin(thetaIJ);
		double cosT = cos(thetaIJ);
		double gij = current1->triple.value;
		double bij = current2->triple.value;
		pi += state[num] * state[j] * (gij * cosT + bij * sinT);
		if (num == j) {
			matrix->setValue(index, num, (-bij * state[num] * state[num] + StateCalculator::calBusQ_polar(pos, y, state)) / state[num]);
 			matrix->setValue(index, num + n, -gij * state[num] * state[num]);
			current1 = current1->right;
			current2 = current2->right;
			continue;
		}
		matrix->setValue(index, j, state[num] * (gij * sinT - bij * cosT));
		matrix->setValue(index, j + n, -state[num] * state[j] * (gij * cosT + bij * sinT));
		current1 = current1->right;
		current2 = current2->right;
	}
	matrix->setValue(index, num + n, pi, true);
}

void Jacobian::_line_from_p_polar(int branchId, int n, int currentRow) {
	int i = y->lineFrom[branchId - 1];
	int j = y->lineTo[branchId - 1];
	double* gbg1b1 = y->lineFromAdmittance + 4 * (branchId - 1);
	double thetaIJ = state[i + n] - state[j + n];
	double cosT = cos(thetaIJ);
	double sinT = sin(thetaIJ);
	matrix->setValue(currentRow, i, 2 * state[i] * (gbg1b1[0] + gbg1b1[2]) - state[j] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT));
	matrix->setValue(currentRow, j, -state[i] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT));
	double tmp = state[i] * state[j] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT);
	matrix->setValue(currentRow, i + n, tmp);
	matrix->setValue(currentRow, j + n, -tmp);
}

void Jacobian::_line_from_q_polar(int branchId, int n, int currentRow) {
	int i = y->lineFrom[branchId - 1];
	int j = y->lineTo[branchId - 1];
	double* gbg1b1 = y->lineFromAdmittance + 4 * (branchId - 1);
	double thetaIJ = state[i + n] - state[j + n];
	double cosT = cos(thetaIJ);
	double sinT = sin(thetaIJ);
	matrix->setValue(currentRow, i, -2 * state[i] * (gbg1b1[1] + gbg1b1[3]) - state[j] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT));
	matrix->setValue(currentRow, j, -state[i] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT));
	double tmp = -state[i] * state[j] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT);
	matrix->setValue(currentRow, i + n, tmp);
	matrix->setValue(currentRow, j + n, -tmp);
}


void Jacobian::_line_to_p_polar(int branchId, int n, int currentRow) {
	int j = y->lineFrom[branchId - 1];
	int i = y->lineTo[branchId - 1];
	double* gbg1b1 = y->lineToAdmittance + 4 * (branchId - 1);
	double thetaIJ = state[i + n] - state[j + n];
	double cosT = cos(thetaIJ);
	double sinT = sin(thetaIJ);
	matrix->setValue(currentRow, i, 2 * state[i] * (gbg1b1[0] + gbg1b1[2]) - state[j] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT));
	matrix->setValue(currentRow, j, -state[i] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT));
	double tmp = state[i] * state[j] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT);
	matrix->setValue(currentRow, i + n, tmp);
	matrix->setValue(currentRow, j + n, -tmp);
}

void Jacobian::_line_to_q_polar(int branchId, int n,int currentRow) {
	int j = y->lineFrom[branchId - 1];
	int i = y->lineTo[branchId - 1];
	double* gbg1b1 = y->lineToAdmittance + 4 * (branchId - 1);
	double thetaIJ = state[i + n] - state[j + n];
	double cosT = cos(thetaIJ);
	double sinT = sin(thetaIJ);
	matrix->setValue(currentRow, i, -2 * state[i] * (gbg1b1[1] + gbg1b1[3]) - state[j] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT));
	matrix->setValue(currentRow, j, -state[i] * (gbg1b1[0] * sinT - gbg1b1[1] * cosT));
	double tmp = -state[i] * state[j] * (gbg1b1[0] * cosT + gbg1b1[1] * sinT);
	matrix->setValue(currentRow, i + n, tmp);
	matrix->setValue(currentRow, j + n, -tmp);
}

void Jacobian::formJacobian_fullState(int startRow) {
	int n = y->admittance[0].getM();
	for(int i = 0; i < meas->measNum; i++) {
		int num = atoi(meas->meas[i].positionId.c_str());	
		switch(meas->meas[i].measureType) {
			case TYPE_BUS_ANGLE:
				matrix->setValue(i + startRow, num - 1 + n, 1);
				break;
			case TYPE_BUS_VOLTAGE:
				matrix->setValue(i + startRow, num - 1, 1);
				break;
			case TYPE_BUS_ACTIVE_POWER:
				matrix->setValue(i + startRow, num - 1 + 2 * n, 1);
				break;
			case TYPE_BUS_REACTIVE_POWER:
				matrix->setValue(i + startRow, num - 1 + 3 * n, 1);
				break;
			case TYPE_LINE_FROM_ACTIVE:
				_line_from_p_polar(num, n, i + startRow);
				break;
			case TYPE_LINE_FROM_REACTIVE:
				_line_from_q_polar(num, n, i + startRow);
				break;
			case TYPE_LINE_TO_ACTIVE:
				_line_to_p_polar(num, n, i + startRow);
				break;
			case TYPE_LINE_TO_REACTIVE:
				_line_to_q_polar(num, n, i + startRow);
				break;
			default:
				cout<<"<Jacobian>unsupported measure type: "<<meas->meas[i].measureType<<endl;
				break;
		}
	}	
}

void Jacobian::formJacobian_CIHM_rect_const(){
	//cout<<"Jacobian Const forming..."<<endl;
	int n = y->admittance[0].getM();
	matrix->initial(meas->measNum, 4 * n);
	assert(meas->measNum == 4*n);
	for(int i = 0; i < meas->measNum; i++) {
		int num = atoi(meas->meas[i].positionId.c_str());		
		switch((*meas).meas[i].measureType) {
			case TYPE_BUS_NetReal:
				_bus_freal_CI_rect(num, n, i);
				break;
			case TYPE_BUS_NetImage:
 				_bus_fimage_CI_rect(num, n, i);
				break;
			case TYPE_BUS_VOLTAGE :
			case TYPE_BUS_ACTIVE_POWER :
			case TYPE_BUS_REACTIVE_POWER :
			case TYPE_BUS_E:
			case TYPE_BUS_F:
			case TYPE_BUS_Ix:
			case TYPE_BUS_Iy:
				break;
			default:
                cout<<"<Jacobian>unsupported measure type: "<<(*meas).meas[i].measureType<<endl;
				break;
		}
	}
}

void Jacobian::formJacobian_CIHM_rect_vary(double* state){
	//cout<<"Jacobian Vary forming ... ..."<<endl;
	this->state = state;
	int n = y->admittance[0].getM();
	//each value must fill in, to update, especially 0
	for(int i = 0; i < meas->measNum; i++) {
		int num = atoi(meas->meas[i].positionId.c_str());		
		switch((*meas).meas[i].measureType) {
			case TYPE_BUS_VOLTAGE :
				_bus_v_CI_rect(num, n, i);
				break;
			case TYPE_BUS_ACTIVE_POWER :
				_bus_p_CI_rect(num, n, i);
				break;
			case TYPE_BUS_REACTIVE_POWER :
				_bus_q_CI_rect(num, n, i);
				break;
			case TYPE_BUS_E:
				_bus_e_CI_rect(num, n, i);
				break;
			case TYPE_BUS_F:
				_bus_f_CI_rect(num, n, i);
				break;
			case TYPE_BUS_Ix:
				_bus_ix_CI_rect(num, n, i);
				break;
			case TYPE_BUS_Iy:
				_bus_iy_CI_rect(num, n, i);
				break;
			case TYPE_BUS_NetReal:
			case TYPE_BUS_NetImage:
				break;
           default:
                cout<<"<Jacobian>unsupported measure type: "<<(*meas).meas[i].measureType<<endl;
                break;
		}
	}
}

//pos: busNum , currentRow ; in JacobyMatrix
void Jacobian::_bus_freal_CI_rect(int pos, int n, int currentRow){
	int i = pos - 1;
	double d;
	for(int j=0;j < n ;j++){
		d = y->admittance[0].getValue(i,j);
		if(d != -9999)	matrix->setValue( currentRow, j, d);
		d = y->admittance[1].getValue(i,j);
		if(d != -9999)	matrix->setValue( currentRow, j+n, -d );
		if(j == i ) { matrix->setValue( currentRow, j + 2*n, - 1.0);}
	}
}

void Jacobian::_bus_fimage_CI_rect(int pos, int n, int currentRow){
	int i = pos - 1;
	double d;
	for(int j=0;j < n ;j++){
		d = y->admittance[1].getValue(i,j); 
		if( d != -9999 )	matrix->setValue( currentRow, j, d );
		d = y->admittance[0].getValue(i,j);
		if( d != -9999 )	matrix->setValue( currentRow, j+n, d);
		if(j == i ) { matrix->setValue( currentRow, j + 3*n, - 1.0);}
	}
}
	
void Jacobian::_bus_v_CI_rect(int pos, int n, int currentRow){
	int busNum_1 = pos - 1;
	matrix->setValue(currentRow, busNum_1, 2*state[ busNum_1 ]);	//2e
	matrix->setValue(currentRow, busNum_1+n, 2*state[ busNum_1+n ]);	//2f
}
	
void Jacobian::_bus_p_CI_rect(int pos, int n, int currentRow){
	int busNum_1 = pos - 1;
	matrix->setValue(currentRow, busNum_1       , state[ busNum_1 + 2*n ]);	//e:ix
	matrix->setValue(currentRow, busNum_1 + n   , state[ busNum_1 + 3*n ]);	//f:iy
	matrix->setValue(currentRow, busNum_1 + 2*n , state[ busNum_1	   ]);	//ix:e
	matrix->setValue(currentRow, busNum_1 + 3*n , state[ busNum_1 +   n ]);	//iy:f
}

void Jacobian::_bus_q_CI_rect(int pos, int n, int currentRow){
	int busNum_1 = pos - 1;
	matrix->setValue(currentRow, busNum_1       , - state[ busNum_1 + 3*n ]);	//e:-iy
	matrix->setValue(currentRow, busNum_1 + n   ,   state[ busNum_1 + 2*n ]);	//f:ix
	matrix->setValue(currentRow, busNum_1 + 2*n ,   state[ busNum_1 +   n ]);	//ix:f
	matrix->setValue(currentRow, busNum_1 + 3*n , - state[ busNum_1       ]);	//iy:-e
}
	
void Jacobian::_bus_e_CI_rect(int pos, int n, int currentRow){
	int i = pos - 1;	//busNumber - 1
	matrix->setValue(currentRow, i, 1.0);
}
	
void Jacobian::_bus_f_CI_rect(int pos, int n, int currentRow){
	int i = pos - 1;	//busNumber - 1
	matrix->setValue(currentRow, i+n, 1.0);
}
	
void Jacobian::_bus_ix_CI_rect(int pos, int n, int currentRow){
	int i = pos - 1;	//busNumber - 1
	matrix->setValue(currentRow, i+2*n, 1.0);
}
	
void Jacobian::_bus_iy_CI_rect(int pos, int n, int currentRow){
	int i = pos - 1;	//busNumber - 1
	matrix->setValue(currentRow, i+3*n, 1.0);
}
