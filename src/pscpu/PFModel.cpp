#include"PFModel.h"
#include<math.h>

using namespace cpscpu;

PFModel::~PFModel(){
	freeSpace();
}

void PFModel::freeSpace(){
	if(initialX != NULL)
		delete[] initialX;
	if(vTheta != NULL) 
		delete[] vTheta;
	initialX = NULL;		
	vTheta = NULL;	
	meas.freeSpace();
}

PFModel::PFModel() {
	tolerance = 1e-5;
	maxIter = 50;
	isConverged = false;
	printPath = false;
	initialX = NULL;
	vTheta = NULL;
}

PFModel::PFModel(IEEEDataIsland * island) {
	tolerance = 1e-4  ;
	maxIter = 50;
	isConverged = false;
	printPath = false;
	initialX = NULL;
	vTheta = NULL;
	setIsland(island);
}

void PFModel::setIsland(IEEEDataIsland * island) {
	this->island = island;	
}


