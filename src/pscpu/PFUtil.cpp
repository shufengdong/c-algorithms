#include <stdio.h>
#include "PFUtil.h"

using namespace std;
using namespace cpscpu;

void PFUtil::formMeas4PF_polar(IEEEDataIsland *island, MeasureVector * meas){
	meas->measNum = 2 * island->busN;
	meas->meas = new MeasureInfo[2 * island->busN];//todo: dangerous
	int count = 0;
	double baseMVA = island->mvaBase;
	double pm, qm, vm, am;
	for(int i = 0; i < island->busN; i++) {			
		pm = (island->buses[i].generationP - island->buses[i].loadP) / baseMVA;
		qm = (island->buses[i].generationQ - island->buses[i].loadQ) / baseMVA;
		vm = island->buses[i].finalVoltage;
		am = island->buses[i].finalAngle * PI / 180.0;
		switch(island->buses[i].busType) {
		case 0://
		case 1://					
			formMeasure(TYPE_BUS_ACTIVE_POWER, island->buses[i].busNumber, pm, meas->meas + count);
			count++;
			formMeasure(TYPE_BUS_REACTIVE_POWER, island->buses[i].busNumber, qm, meas->meas  + count);
			count++;
			break;
		case 2://
			formMeasure(TYPE_BUS_ACTIVE_POWER, island->buses[i].busNumber, pm, meas->meas  + count);
			count++;
			formMeasure(TYPE_BUS_VOLTAGE, island->buses[i].busNumber, vm, meas->meas  + count);
			count++;
			break;
		case 3://
			formMeasure(TYPE_BUS_VOLTAGE, island->buses[i].busNumber, vm, meas->meas + count);
			count++;
			formMeasure(TYPE_BUS_ANGLE, island->buses[i].busNumber, am, meas->meas + count);
			count++;
			break;
		default://
			cout<<"wrong type in ieee common data format!"<<endl;
			break;
		}
	}	
}

void PFUtil::formMeas4PF_rect(IEEEDataIsland *island, MeasureVector * meas) {
	meas->measNum = 2 * island->busN;
	meas->meas = new MeasureInfo[2 * island->busN];//todo: dangerous
	int count = 0;
	double baseMVA = island->mvaBase;
	double pm, qm, vm, am;
	for(int i = 0; i < island->busN; i++) {			
		pm = (island->buses[i].generationP - island->buses[i].loadP) / baseMVA;
		qm = (island->buses[i].generationQ - island->buses[i].loadQ) / baseMVA;
		vm = island->buses[i].finalVoltage;
		am = island->buses[i].finalAngle * PI / 180.0;
		switch(island->buses[i].busType) {
		case 0://
		case 1://					
			formMeasure(TYPE_BUS_ACTIVE_POWER, island->buses[i].busNumber, pm, meas->meas + count);
			count++;
			formMeasure(TYPE_BUS_REACTIVE_POWER, island->buses[i].busNumber, qm, meas->meas  + count);
			count++;
			break;
		case 2://
			formMeasure(TYPE_BUS_ACTIVE_POWER, island->buses[i].busNumber, pm, meas->meas  + count);
			count++;
			formMeasure(TYPE_BUS_VOLTAGE, island->buses[i].busNumber, vm * vm, meas->meas  + count);
			count++;
			break;
		case 3://
			formMeasure(TYPE_BUS_E, island->buses[i].busNumber, vm * cos(am), meas->meas + count);
			count++;
			formMeasure(TYPE_BUS_F, island->buses[i].busNumber, vm * sin(am), meas->meas + count);
			count++;
			break;
		default://
			cout<<"wrong type in ieee common data format!"<<endl;
			break;
		}
	}
}

void PFUtil::formMeas4OPF(IEEEDataIsland *island, MeasureVector * meas, int pControllableNum, int qControllableNum, int vControllableNum, 
	int* pControllableBus, int* qControllableBus, int* vControllableBus, bool isSlackBusVFixed) {			
	
	bool * flagP = new bool[island->busN];
	bool * flagQ = new bool[island->busN];
	bool * flagV = new bool[island->busN];
	for(int i = 0; i < island->busN; i++) {
		flagP[i] = false;
		flagQ[i] = false;
		flagV[i] = false;
	}
	for(int i = 0; i < pControllableNum; i++)
		flagP[pControllableBus[i] - 1] = true;
	for(int i = 0; i < qControllableNum; i++)
		flagQ[qControllableBus[i] - 1] = true;
	for(int i = 0; i < vControllableNum; i++)
		flagV[vControllableBus[i] - 1] = true;

	double baseMVA = island->mvaBase;
	int count = 0, pqCount = 0, pvCount = 0;
	for(int i = 0; i < island->busN; i++) {
		switch(island->buses[i].busType) {
		case 0://
		case 1://
			if(!flagP[island->buses[i].busNumber -1]) count++;
			if(!flagQ[island->buses[i].busNumber -1]) count++;
			pqCount++;
			break;
		case 2://
			if(!flagP[island->buses[i].busNumber -1]) count++;
			if(!flagV[island->buses[i].busNumber -1]) count++;	
			pvCount++;
			break;
		case 3://					
			if(!flagV[island->buses[i].busNumber -1] && isSlackBusVFixed) count++;	
			count++;
			break;
		default://
			cout<<"wrong type in ieee common data format!"<<endl;
			break;
		}
	}
	meas->measNum = count;
	meas->meas = new MeasureInfo[meas->measNum];//todo: dangerous
	count = 0;
	double pm, qm, vm, am;
	for(int i = 0; i < island->busN; i++) {			
		pm = (island->buses[i].generationP - island->buses[i].loadP) / baseMVA;
		qm = (island->buses[i].generationQ - island->buses[i].loadQ) / baseMVA;
		vm = island->buses[i].finalVoltage;
		am = island->buses[i].finalAngle * PI / 180.0;
		switch(island->buses[i].busType) {
		case 0://
		case 1://					
			if(!flagP[island->buses[i].busNumber -1]) {
				formMeasure(TYPE_BUS_ACTIVE_POWER, island->buses[i].busNumber, pm, meas->meas + count);
				count++;
			}
			if(!flagQ[island->buses[i].busNumber -1]) {
				formMeasure(TYPE_BUS_REACTIVE_POWER, island->buses[i].busNumber, qm, meas->meas  + count);
				count++;
			}
			break;
		case 2://
			if(!flagP[island->buses[i].busNumber -1]) {
				formMeasure(TYPE_BUS_ACTIVE_POWER, island->buses[i].busNumber, pm, meas->meas  + count);
				count++;
			}
			if(!flagV[island->buses[i].busNumber -1])  {
				formMeasure(TYPE_BUS_VOLTAGE, island->buses[i].busNumber, vm, meas->meas  + count);
				count++;
			}
			break;
		case 3://
			if(!flagV[island->buses[i].busNumber -1] && isSlackBusVFixed) {						
				formMeasure(TYPE_BUS_VOLTAGE, island->buses[i].busNumber, vm, meas->meas + count);
				count++;
			}					
			formMeasure(TYPE_BUS_ANGLE, island->buses[i].busNumber, am, meas->meas + count);
			count++;
			break;
		default://
			cout<<"wrong type in ieee common data format!"<<endl;
			break;
		}
	}
	delete[] flagP;
	delete[] flagQ;
	delete[] flagV;
	cout<<"There are "<<pqCount<<" PQ buses and "<<pvCount<<" PV buses."<<endl;
}
	
void PFUtil::formMeas4PF_CI(IEEEDataIsland *island, MeasureVector * meas) {			
	//double baseMVA = 100;//todo: not right
	double baseMVA = island->mvaBase;
	int count = 0;
	count = 4 * island->busN;
	meas->measNum = count;
	meas->meas = new MeasureInfo[meas->measNum];
	count = 2* island->busN;	//count for non_net measures
	int netcount = 0; // count for net measure
	int pqcount = 0, pvcount = 0, swcount=0, cncount=0;
	//arrange the first 2N measures as NetMeasures,freal=0 and fimage=0;
	double pm, qm, vm, am;
	for(int i = 0; i < island->busN; i++) {
		//f(real) =0, f(image) = 0;
		formMeasure(TYPE_BUS_NetReal, island->buses[i].busNumber, 0, meas->meas + netcount);
		netcount++;
		formMeasure(TYPE_BUS_NetImage, island->buses[i].busNumber, 0, meas->meas + netcount);
		netcount++;
		pm = (island->buses[i].generationP - island->buses[i].loadP) / baseMVA;
		qm = (island->buses[i].generationQ - island->buses[i].loadQ) / baseMVA;
		vm = island->buses[i].finalVoltage;
		am = island->buses[i].finalAngle * PI / 180.0;
		switch(island->buses[i].busType) {
			case 0://
			case 1://					
				formMeasure(TYPE_BUS_ACTIVE_POWER, island->buses[i].busNumber, pm, meas->meas + count);
				count++;
				formMeasure(TYPE_BUS_REACTIVE_POWER, island->buses[i].busNumber, qm, meas->meas  + count);
				count++;
				pqcount++;
				break;
			case 2://
				formMeasure(TYPE_BUS_ACTIVE_POWER, island->buses[i].busNumber, pm, meas->meas  + count);
				count++;
				formMeasure(TYPE_BUS_VOLTAGE, island->buses[i].busNumber, vm*vm, meas->meas  + count);//rect: V*V
				count++;
				pvcount++;
				break;
			case 3://
				formMeasure(TYPE_BUS_E, island->buses[i].busNumber, vm*cos(am), meas->meas + count);
				count++;
				formMeasure(TYPE_BUS_F, island->buses[i].busNumber, vm*sin(am), meas->meas + count);
				count++;
				swcount++;
				break;
			case 4://
				formMeasure(TYPE_BUS_Ix, island->buses[i].busNumber, 0, meas->meas + count);
				count++;
				formMeasure(TYPE_BUS_Iy, island->buses[i].busNumber, 0, meas->meas + count);
				count++;
				cncount++;
				break;
			default://
				cout<<"wrong type in ieee common data format!"<<endl;
				break;
		}
	}
	cout<<"There are "<<pqcount<<" PQ buses ,"<<pvcount<<" PV buses, "<<swcount<<" SW buses and "<<cncount<<" CN buses."<<endl;
}

void PFUtil::formMeasure(int type, int positionId, double value, MeasureInfo* info) {
	info->value = value;
	char tmp[10];
	sprintf(tmp, "%d", positionId);
	string s(tmp);
	info->positionId = s;
	info->measureType = type;
}

