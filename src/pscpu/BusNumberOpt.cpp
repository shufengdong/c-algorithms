#include"BusNumberOpt.h"

using namespace std;
using namespace cpscpu;

void BusNumberOpt::tinney1() {	
	if(bus2buses.empty())
		formBus2Buses();
	Bus2Buses** toSort = new Bus2Buses*[bus2buses.size()];
	map<int, Bus2Buses>::iterator iter;
	int count = 0;
	for (iter = bus2buses.begin(); iter != bus2buses.end(); iter++) {
		toSort[count++] = &iter->second;
	}
	quickSort(toSort, count);
	old2new.clear();
	new2old.clear();	
	for(int i = 0; i < island->busN; i++) {
		int old = (*toSort[i]).busNumber;
		old2new.insert(pair<int, int>(old, i + 1));
		new2old.insert(pair<int, int>(i + 1, old));
	}
	delete[] toSort;
}

void BusNumberOpt::tinney2() {	
	if(bus2buses.empty())
		formBus2Buses();	
	//todo: not finished.
}

void BusNumberOpt::trans() {
	trans(&old2new);
}

void BusNumberOpt::trans(map<int, int> * m) {
	for(int i = 0; i < island->busN; i++) {
		int newNum = m->find(island->buses[i].busNumber)->second;		
		//cout<<"old->new"<<island->buses[i].busNumber<<"\t"<<newNum<<endl;
		island->buses[i].busNumber = newNum;
	}
    for(int i = 0; i < island->branchN; i++) {
        int head = island->branches[i].tapBusNumber;
		int tail = island->branches[i].zBusNumber;
		island->branches[i].tapBusNumber = m->find(head)->second;
        island->branches[i].zBusNumber = m->find(tail)->second;
    }
}

void BusNumberOpt::recover() {
	trans(&new2old);
}

void BusNumberOpt::formBus2Buses() {
	for(int i = 0; i < island->branchN; i++) {
		int head = island->branches[i].tapBusNumber;
		int tail = island->branches[i].zBusNumber;
		if(bus2buses.find(head) == bus2buses.end()) {				
			bus2buses.insert(pair<int, Bus2Buses>(head, Bus2Buses(head)));
		}
		if(bus2buses.find(tail) == bus2buses.end()) {				
			bus2buses.insert(pair<int, Bus2Buses>(tail, Bus2Buses(tail)));
		}
		bool flag = true;
		int size = bus2buses.find(head)->second.connectedBuses.size();
		for(int i = 0; i < size; i++) {
			if(bus2buses.find(head)->second.connectedBuses.at(i) == tail) {
				flag = false;
				break;
			}
		}
		if(flag) {
			bus2buses.find(head)->second.connectedBuses.push_back(tail);
			bus2buses.find(tail)->second.connectedBuses.push_back(head);
		}
	}
}


void BusNumberOpt::run(Bus2Buses** pData,int left,int right) { 
	int i,j; 
	int middle;
	Bus2Buses* iTemp;
	i = left; 
	j = right; 
	middle = (*pData[(left+right)/2]).connectedBuses.size(); //求中间值 
	do{ 
		while(((*pData[i]).connectedBuses.size()<middle) && (i<right))//从左扫描大于中值的数 
			i++; 
		while(((*pData[i]).connectedBuses.size()>middle) && (j>left))//从右扫描大于中值的数 
			j--; 
		if(i<=j){//找到了一对值
			//交换 
			iTemp = pData[i]; 
			pData[i] = pData[j]; 
			pData[j] = iTemp; 
			i++; 
			j--; 
		} 
	}while(i<=j);//如果两边扫描的下标交错，就停止（完成一次） 

	//当左边部分有值(left<j)，递归左半边 
	if(left<j) 
		run(pData,left,j); 
	//当右边部分有值(right>i)，递归右半边 
	if(right>i) 
		run(pData,i,right); 
} 


void BusNumberOpt::quickSort(Bus2Buses** pData,int count) { 
	Bus2Buses*  iTemp; 
	int iPos; 
	for(int i= 0; i<count-1; i++) { 
		iTemp = pData[i]; 
        iPos = i; 
        for(int j=i+1;j<count;j++) { 
			if((*pData[j]).connectedBuses.size()<(*iTemp).connectedBuses.size()) { 
				iTemp = pData[j]; 
                iPos = j; 
            } 
        } 
        pData[iPos] = pData[i]; 
        pData[i] = iTemp; 
	} 
} 