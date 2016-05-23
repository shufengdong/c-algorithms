#include <string.h>
#include "NewtonPF.h"
#include "AbstractSM.h"
#include "StateCalculator.h"
#include "NewtonAlg.h"
#include "PFUtil.h"
#include "Jacobian.h"

using namespace cpscpu;

NewtonPF::NewtonPF(){	
	result = NULL;
}

NewtonPF::NewtonPF(PFModel *model) : AbstractPF(model){
	result = NULL;
}

void NewtonPF::doPF() {
	cout<<"Initial model..."<<endl;
	bool b = initialModel();
	cout<<"Initial model finished."<<endl;
	if(!b) {
		cout<<"Do power flow failed because of wrong se model."<<endl;
		model->isConverged = false;
		return;
	}
	if(result != NULL)
		delete[] result;
	result = new double[getDimension()];
	cout<<"Start newton solve ..."<<endl;
	b = NewtonAlg::cal(this, result);
    cout<<"Newton solve finished."<<endl;
	if(b) {
		int n = model->island->busN;	
		model->isConverged = true;
		if(getMode() == PF_MODE_VTHETA) {
			for(int i = 0; i < 2 * n; i++) 
				model->vTheta[i] = result[i];
		} else if(getMode() == PF_MODE_RECT){
			for(int i = 0; i < n; i++) {
				model->vTheta[i] = sqrt(result[i] * result[i] + result[i + n] * result[i + n]);
				model->vTheta[i + n] = atan2(result[i + n], result[i]);
				cout<<result[i]<<"\t"<<result[i + n] * 180 / PI<<endl;
			}
		}
	} else
		model->isConverged = false;   
}

void NewtonPF::getResult(double * r) {
	if(result != NULL)
		memcpy(r, result, sizeof(double) * getDimension());
}

int NewtonPF::getDimension(){
	return model->island->busN * 2;
}

int NewtonPF::getMaxIter() {
	return 50;
}

double NewtonPF::getTolerance() {
	return 1e-5;
}

void NewtonPF::getInitial(double* x) {
	memcpy(x, model->initialX, sizeof(double) * getDimension());
}

void NewtonPF::getJocobian(double *state, AbstractSM * matrix) {
	Jacobian * h = new Jacobian(&(model->meas),&(model->y), state);
	h->setMatrix(matrix);

	if(getMode() == PF_MODE_VTHETA)
		h->formJacobian_polar();
	else if(getMode() == PF_MODE_RECT)
		h->formJacobian_rect();
}

void NewtonPF::getZ(double* z) {
	for(int i = 0; i < model->meas.measNum; i++)
		z[i] = model->meas.meas[i].value;
}

void NewtonPF::calZ(double* state, double* z_est) {
	if(getMode() == PF_MODE_VTHETA)
		StateCalculator::getEstimatedZ_polar(&(model->meas), &(model->y), state, z_est);
	else if(getMode() == PF_MODE_RECT)
		StateCalculator::getEstimatedZ_rect(&(model->meas), &(model->y), state, z_est);
}
