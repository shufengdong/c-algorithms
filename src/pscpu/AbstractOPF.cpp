#include"AbstractOPF.h"
#include"PFUtil.h"

using namespace cpscpu;

AbstractOPF::AbstractOPF(OPFModel * model, int mode) {
	initial();
	setMode(mode);
	setModel(model);
}

AbstractOPF::AbstractOPF() {
	initial();	
	setMode(OPF_MODE_PQVTHETA);	
}

AbstractOPF::AbstractOPF(OPFModel * model) {
	isDebug = false;
	isPrintPath = true;
	setMode(OPF_MODE_PQVTHETA);
	setModel(model);			
}

void AbstractOPF::initial(){
	isDebug = false;
	model = NULL;
}

void AbstractOPF::setModel(OPFModel * model) {
	this->model = model;
}

int AbstractOPF::getMode(){
	return mode;
}

void AbstractOPF::setMode(int mode) {
	this->mode = mode;
}


bool AbstractOPF::getIsPrintPath() {
	return isPrintPath;
}

void AbstractOPF::setIsPrintPath(bool b) {
	this->isPrintPath = b;
}

void AbstractOPF::setObjFunc(OPFObjFunc * obj) {
	this->objFunc = obj;
}

OPFObjFunc * AbstractOPF::getObjFunc() {
	return objFunc;
}

bool AbstractOPF::initialModel() {
	if(model == NULL)
		return false;	
	if(model->island == NULL)
		return false;			
	model->freeSpace();
	int n = model->island->busN;		
    model->vTheta = new double[2 * n];
	model->y.formMatrix(model->island);
	if(getMode() ==OPF_MODE_PQVTHETA) {
		model->initialX = new double[4 * n];
		IEEEDataIsland * island = model->island;
		
		for (int i =0; i < n; i++) {			
			model->initialX[island->buses[i].busNumber - 1] =  island->buses[i].finalVoltage; 
			model->initialX[island->buses[i].busNumber - 1 + n] = island->buses[i].finalAngle * PI / 180.0;		
		}		
		for (int i =0; i < n; i++) {			
			int pos = island->buses[i].busNumber;
			model->initialX[pos - 1 + 2 * n] =  StateCalculator::calBusP_polar(pos, &(model->y), model->initialX);
			model->initialX[pos - 1 + 3 * n] = StateCalculator::calBusQ_polar(pos, &(model->y), model->initialX);
		}
	} else if(getMode() == OPF_MODE_UI) {
		model->initialX = new double[4 * n];
		IEEEDataIsland * island = model->island;
		for (int i =0; i < n; i++) {			
			double v =  island->buses[i].finalVoltage; 
			double theta = island->buses[i].finalAngle * PI / 180.0;		
			model->initialX[island->buses[i].busNumber - 1] = v * cos(theta);
			model->initialX[island->buses[i].busNumber - 1 + n] = v * sin(theta);
		}
	}
	PFUtil::formMeas4OPF(model->island, &(model->meas), model->pControllableNum, model->qControllableNum, model->vControllableNum, 
		model->pControllableBus, model->qControllableBus, model->vControllableBus, true);
	return true;
}