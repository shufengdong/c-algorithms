#ifndef __cpscpu_BusNumberOpt_H__
#define	__cpscpu_BusNumberOpt_H__

#include"IEEEDataIsland.hpp"
#include<map>
#include<vector>
#include<iostream>

using namespace std;

namespace cpscpu {
	struct Bus2Buses {
		~Bus2Buses(){
			connectedBuses.clear();
		}
		Bus2Buses(int busNumber) {
			this->busNumber = busNumber;
		};

		int busNumber;
		vector<int> connectedBuses;
	};

	class BusNumberOpt {
	public:	
		BusNumberOpt(IEEEDataIsland * island){this->island = island;};

		~BusNumberOpt(){
			bus2buses.clear();
			old2new.clear();
			new2old.clear();
		};

		void formBus2Buses();
		void tinney1();
		void tinney2();
		void trans();
		void recover();
		void trans(map<int, int> * m);
	private:
		void run(Bus2Buses **, int, int);
		void quickSort(Bus2Buses **, int);

	public:
		map<int, Bus2Buses> bus2buses;
		map<int, int> old2new;
		map<int, int> new2old;

	private:
		IEEEDataIsland * island;

	};
}//end of namespace

#endif