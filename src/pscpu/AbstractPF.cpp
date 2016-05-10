#include"AbstractPF.h"
#include"PFUtil.h"

using namespace cpscpu;

AbstractPF::AbstractPF(PFModel * model, int mode) {
	initial();
	setMode(mode);
	setModel(model);
}

AbstractPF::AbstractPF() {
	initial();	
	setMode(PF_MODE_VTHETA);	
}

AbstractPF::AbstractPF(PFModel * model) {
	isDebug = false;
	isPrintPath = true;
	setMode(PF_MODE_VTHETA);
	setModel(model);			
}

void AbstractPF::initial(){
	isDebug = false;
	model = NULL;
}

void AbstractPF::setModel(PFModel * model) {
	this->model = model;
}

int AbstractPF::getMode(){
	return mode;
}

void AbstractPF::setMode(int mode) {
	this->mode = mode;
}


bool AbstractPF::getIsPrintPath() {
	return isPrintPath;
}

void AbstractPF::setIsPrintPath(bool b) {
	this->isPrintPath = b;
}

int AbstractPF::getIterTimes() {
	return iterTimes;
}

PFModel * AbstractPF::getModel(){
	return model;
}

bool AbstractPF::initialModel() {
	if(model == NULL)
		return false;	
	if(model->island == NULL)
		return false;			
	model->freeSpace();
	int n = model->island->busN;		
    model->vTheta = new double[2 * n];
	model->y.formMatrix(model->island);
	if(getMode() == PF_MODE_VTHETA || getMode() == PF_MODE_RECT) {
		model->initialX = new double[2 * n];
		IEEEDataIsland * island = model->island;
		for (int i =0; i < n; i++) {			
			if (island->buses[i].busType == 3) {
				model->initialX[island->buses[i].busNumber - 1] =  island->buses[i].finalVoltage; 
				model->initialX[island->buses[i].busNumber - 1 + n] = island->buses[i].finalAngle * PI / 180.0;
			} else if(island->buses[i].busType == 2){//it's pv node
				model->initialX[island->buses[i].busNumber - 1] =  island->buses[i].finalVoltage;  
				model->initialX[island->buses[i].busNumber - 1 + n] =  0.0;
			} else {
				model->initialX[island->buses[i].busNumber - 1] =  1.0; 
				model->initialX[island->buses[i].busNumber - 1 + n] =  0.0;  
			}
		}		
	} 
	if(getMode() == PF_MODE_VTHETA) {		
		PFUtil::formMeas4PF_polar(model->island, &(model->meas));
	} else if(getMode() == PF_MODE_RECT) {
		PFUtil::formMeas4PF_rect(model->island, &(model->meas));
	}
	return true;
}