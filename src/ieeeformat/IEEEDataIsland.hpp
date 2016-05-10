#ifndef __IEEEDATAISLAND_H__
#define __IEEEDATAISLAND_H__

#include "BusData.hpp"
#include "BranchData.hpp"
#include "TitleData.hpp"
#include "LossZoneData.hpp"
#include "TieLineData.hpp"
#include "InterchangeData.hpp"

using namespace std;

namespace cpscpu {
	class IEEEDataIsland {
	public:	
		TitleData title;
		BusData * buses;
		BranchData* branches;
		LossZoneData * lossZones;
		TieLineData * tieLines;
		InterchangeData * interchanges;

		int busN;
		int branchN;
		int lossZoneN;
		int tieLineN;
		int interchangeN;
		double mvaBase;

	public:
		IEEEDataIsland() {
			busN = -1;
			branchN = -1;
			lossZoneN = -1;
			tieLineN = -1;
			interchangeN = -1;
		};

		~IEEEDataIsland() {
			freeSpace();
		};			

	private:
		void freeSpace() {
			if(busN != -1)
				delete[] buses;
			if(branchN != -1)
				delete[] branches;
			if(lossZoneN != -1)
				delete[] lossZones;
			if(tieLineN != -1)
				delete[] tieLines;
			if(interchangeN != -1)
				delete[] interchanges;
		};
		
	};
}//end of namespace

#endif