#include"AbstractSE.h"

using namespace cpscpu;

AbstractSE::AbstractSE(SEModel * model, int mode) {
	initial();
	this->model = model;			
	this->mode = mode;
}

AbstractSE::AbstractSE(SEModel * model) {
	initial();
	this->model = model;			
}

AbstractSE::AbstractSE() {
	initial();
}

void AbstractSE::initial() {
	isDebug = false;
	isPrintPath = true;
	mode = SE_MODE_VTHETA;
}

int AbstractSE::getMode() {
	return mode;
}

void AbstractSE::setMode(int mode) {
	this->mode = mode;
}

bool AbstractSE::getIsPrintPath() {
	return isPrintPath;
}

void AbstractSE::setIsPrintPath(bool b) {
	this->isPrintPath = b;
}

bool AbstractSE::initialModel() {
	if(model->island == NULL) {
		cout<<"Initial SE model failed because island is NULL"<<endl;
		return false;
	}
	if(model->measVector == NULL) {
		cout<<"Initial SE model failed because measure vector is NULL"<<endl;
		return false;
	}
	if(model->obj == NULL) {
		cout<<"Initial SE model failed because objective function is NULL"<<endl;
		return false;	
	}

	//free space first
	model->freeSpace();
	//intial y of island, free space will be runned in YMatrix class
	model->y.formMatrix(model->island);

	//todo: the following lines is wrong!
	model->measInObjFunc = new int[model->measVector->measNum];
	for(int i = 0; i < model->measVector->measNum; i++)
		model->measInObjFunc[i] = i;
	    
	return true;
}