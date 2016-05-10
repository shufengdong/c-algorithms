#include"AbstractSE.h"

using namespace cpscpu;

SEModel::SEModel(){
	maxIter = 500;

	tolerance = 1e-4;
	isConverged = false;
	printPath = true;	
	isSlackBusVoltageFixed = false;		

	slackBusCol = -1;		
	slackBusVoltage = 1.0;
	slackBusAngle = 0.0;
	iterNum = -1;	

	initialState = NULL;
	measVector = NULL;
	island = NULL;
	obj = NULL;
	
	finalVTheta = NULL;
	measInObjFunc = NULL;		
	initialX = NULL;		
	z_est = NULL;
}

void SEModel::freeSpace() {
	if(initialX != NULL)
		delete[] initialX;
	if(measInObjFunc != NULL)
		delete[] measInObjFunc;
	if(finalVTheta != NULL)
		delete[] finalVTheta;	
	if(z_est != NULL)
		delete[] z_est;	
	finalVTheta = NULL;
	measInObjFunc = NULL;		
	initialX = NULL;
	z_est = NULL;
}

void SEModel::setInitialState(double * state) {
	this->initialState = state;
}

void SEModel::setMeasVector(MeasureVector * m){
	this->measVector = m;
}
void SEModel::setIsland(IEEEDataIsland * aIsland){
	this->island = aIsland;
}

void SEModel::setObjFunc(ObjectiveFunction * func) {
	this->obj = func;
}