#ifndef _CPSCPU_DEFAULT_IEEE_FORMAT_PARSER_H_
#define _CPSCPU_DEFAULT_IEEE_FORMAT_PARSER_H_

#include "IEEEDataIsland.hpp"
#include "BusData.hpp"
#include "BranchData.hpp"

#define BUS_CARD_BEGIN	string("BUS DATA FOLLOWS")	
#define BUS_CARD_END	string("-999")	

#define BRANCH_CARD_BEGIN	string("BRANCH DATA FOLLOWS")
#define BRANCH_CARD_END		string("-999")

#define LOSS_ZONE_CARD_BEGIN string("LOSS ZONES FOLLOWS")
#define LOSS_ZONE_CARD_END string("-99")

#define INTERCHANGE_CARD_BEGIN string("INTERCHANGE DATA FOLLOWS")
#define INTERCHANGE_CARD_END string("-9")

#define TIE_LINE_CARD_BEGIN string("TIE LINES FOLLOWS")
#define TIE_LINE_CARD_END string("-999")

#define ITEMS           string("ITEMS")

namespace cpscpu {
	class DefaultIEEEFormatRW {
	public:
		DefaultIEEEFormatRW(){};

		DefaultIEEEFormatRW(IEEEDataIsland * island);
		
		void parse(char * filePath);
		
		void parse(char * filePath, IEEEDataIsland * island);
		
		void parseFromContent(char * ieeeContent);
	
		bool write(char * filePath);

		bool write(char * filePath, IEEEDataIsland * island);
	private:
		IEEEDataIsland * island;		
	};
}

#endif