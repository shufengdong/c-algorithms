#include <string.h>
#include <iostream>
#include <fstream>
#include "DefaultIEEEFormatRW.hpp"

using namespace std;
using namespace cpscpu;

int getItemN(string str, int start) {
	int b;
	b = str.find(ITEMS);
	sscanf(str.substr((start), (b - start)).data(),"%d",&b);//todo: may wrong!
	return b;
}

DefaultIEEEFormatRW::DefaultIEEEFormatRW(IEEEDataIsland * island) {
	this->island = island;
}

void DefaultIEEEFormatRW::parse(char * filePath, IEEEDataIsland * island) {
	this->island = island;
	parse(filePath);
}

void DefaultIEEEFormatRW::parse(char * filePath) { 
	ifstream ieeefile(filePath, ios::in);
	size_t found;
	if(!ieeefile) { //if file is null, return;
		cout<<"can't open file: "<<filePath<<endl;  
		return;
	}

	string templine;
	string sub;
	float f;

	//title card is considered as the first nonempty line
	do {			
		getline(ieeefile,templine);
	} while(templine.empty());
	island->title.parseString(templine);
	island->mvaBase = island->title.mvaBase;
	cout<<"the MVA base is "<<island->mvaBase<<" MVA."<<endl;

	//looking for bus card
	do{
		getline(ieeefile,templine);
		found = templine.find(BUS_CARD_BEGIN);		
	} while(found == string::npos);

	
	int a = getItemN(templine, found + BUS_CARD_BEGIN.length());
	cout<< "There are " << a << " buses." << endl;

	island->buses = new BusData[a];
	island->busN = a;				
	for(int i = 0; i < a; i++) {
		getline(ieeefile, templine);
#ifdef IEEE_FORMAT_DEBUG
		cout<<templine<<endl;
#endif
		island->buses[i].parseString(templine);
	}

	//looking for branch card
	do{
		getline(ieeefile,templine);
		found = templine.find(BRANCH_CARD_BEGIN);	
	} while(found == string::npos);

	a = getItemN(templine, found + BRANCH_CARD_BEGIN.length());
	cout<< "There are " << a << " branches." << endl;

	island->branchN = a;		
	island->branches = new BranchData[a];
	for(int i = 0; i < a; i++) { 
		getline(ieeefile, templine);
#ifdef IEEE_FORMAT_DEBUG
		cout<<templine<<endl;
#endif
		island->branches[i].parseString(templine);
	}

	
	//looking for loss zone card
	do{
		getline(ieeefile,templine);
		found = templine.find(LOSS_ZONE_CARD_BEGIN);	
	} while(found == string::npos);

	a = getItemN(templine, found + LOSS_ZONE_CARD_BEGIN.length());
	cout<< "There are " << a << " loss zones." << endl;

	island->lossZoneN = a;
	island->lossZones = new LossZoneData[a];
	for(int i = 0; i < a; i++) { 
		getline(ieeefile, templine);
#ifdef IEEE_FORMAT_DEBUG
		cout<<templine<<endl;
#endif
		island->lossZones[i].parseString(templine);			
	}

	//looking for interchange card
	do{
		getline(ieeefile,templine);
		found = templine.find(INTERCHANGE_CARD_BEGIN);	
	} while(found == string::npos);

	a = getItemN(templine, found + INTERCHANGE_CARD_BEGIN.length());
	cout<< "There are " << a << " interchanges." << endl;

	island->interchangeN = a;		
	island->interchanges = new InterchangeData[a];
	for(int i = 0; i < a; i++) { 
		getline(ieeefile, templine);
#ifdef IEEE_FORMAT_DEBUG
		cout<<templine<<endl;
#endif
		island->interchanges[i].parseString(templine);			
	}

	//looking for tie line card
	do{
		getline(ieeefile,templine);
		found = templine.find(TIE_LINE_CARD_BEGIN);	
	} while(found == string::npos);

	a = getItemN(templine, found + TIE_LINE_CARD_BEGIN.length());
	cout<< "There are " << a << " tie lines." << endl;

	island->tieLineN = a;		
	island->tieLines = new TieLineData[a];
	for(int i = 0; i < a; i++) { 
		getline(ieeefile, templine);
#ifdef IEEE_FORMAT_DEBUG
		cout<<templine<<endl;
#endif
		island->tieLines[i].parseString(templine);			
	}

	ieeefile.close();
}

void DefaultIEEEFormatRW::parseFromContent(char* ieeeContent) {
	const char *d = "\n";
	char *p;
	string templine;
    size_t found;
	p = strtok(ieeeContent, d);
	templine.assign(p);
    /*
	while(p) {
        printf("%s\n",p);
        p = strtok(NULL,d);
    }	
	return;	
	*/
	//title card is considered as the first nonempty line
	while(templine.empty()) {
		p = strtok(NULL, d);
		templine.assign(p);
	}
	island->title.parseString(templine);
	island->mvaBase = island->title.mvaBase;
	cout<<"the MVA base is "<<island->mvaBase<<" MVA."<<endl;

	//looking for bus card
	do {
		p = strtok(NULL, d);
		templine.assign(p);
		found = templine.find(BUS_CARD_BEGIN);		
	} while(found == string::npos);

	
	int a = getItemN(templine, found + BUS_CARD_BEGIN.length());
	cout<< "There are " << a << " buses." << endl;

	island->buses = new BusData[a];
	island->busN = a;				
	for(int i = 0; i < a; i++) {
		p = strtok(NULL, d);
		templine.assign(p);
#ifdef IEEE_FORMAT_DEBUG
		cout<<templine<<endl;
#endif
		island->buses[i].parseString(templine);
	}

	//looking for branch card
	do{
		p = strtok(NULL, d);
		templine.assign(p);
		found = templine.find(BRANCH_CARD_BEGIN);	
	} while(found == string::npos);

	a = getItemN(templine, found + BRANCH_CARD_BEGIN.length());
	cout<< "There are " << a << " branches." << endl;

	island->branchN = a;		
	island->branches = new BranchData[a];
	for(int i = 0; i < a; i++) { 
		p = strtok(NULL, d);
		templine.assign(p);
#ifdef IEEE_FORMAT_DEBUG
		cout<<templine<<endl;
#endif
		island->branches[i].parseString(templine);
	}

	
	//looking for loss zone card
	do{
		p = strtok(NULL, d);
		templine.assign(p);
		found = templine.find(LOSS_ZONE_CARD_BEGIN);	
	} while(found == string::npos);

	a = getItemN(templine, found + LOSS_ZONE_CARD_BEGIN.length());
	cout<< "There are " << a << " loss zones." << endl;

	island->lossZoneN = a;
	island->lossZones = new LossZoneData[a];
	for(int i = 0; i < a; i++) { 
		p = strtok(NULL, d);
		templine.assign(p);
#ifdef IEEE_FORMAT_DEBUG
		cout<<templine<<endl;
#endif
		island->lossZones[i].parseString(templine);			
	}

	//looking for interchange card
	do{
		p = strtok(NULL, d);
		templine.assign(p);
		found = templine.find(INTERCHANGE_CARD_BEGIN);	
	} while(found == string::npos);

	a = getItemN(templine, found + INTERCHANGE_CARD_BEGIN.length());
	cout<< "There are " << a << " interchanges." << endl;

	island->interchangeN = a;		
	island->interchanges = new InterchangeData[a];
	for(int i = 0; i < a; i++) { 
		p = strtok(NULL, d);
		templine.assign(p);
#ifdef IEEE_FORMAT_DEBUG
		cout<<templine<<endl;
#endif
		island->interchanges[i].parseString(templine);			
	}

	//looking for tie line card
	do {
		p = strtok(NULL, d);
		templine.assign(p);
		found = templine.find(TIE_LINE_CARD_BEGIN);	
	} while(found == string::npos);

	a = getItemN(templine, found + TIE_LINE_CARD_BEGIN.length());
	cout<< "There are " << a << " tie lines." << endl;

	island->tieLineN = a;		
	island->tieLines = new TieLineData[a];
	for(int i = 0; i < a; i++) { 
		p = strtok(NULL, d);
		templine.assign(p);
#ifdef IEEE_FORMAT_DEBUG
		cout<<templine<<endl;
#endif
		island->tieLines[i].parseString(templine);			
	}
}

bool DefaultIEEEFormatRW::write(char * filePath, IEEEDataIsland * island) {
	ofstream fout(filePath);
	if(!fout) {
		cout<<"Unable to open " << filePath<< " for writing ieee data."<<endl;
		return false;
	}
	char buffer[73];
	island->title.toString(buffer);
	fout<<buffer<<endl;

	fout<<"BUS DATA FOLLOWS                    "<<island->busN<<" ITEMS"<<endl;
	for(int i = 0; i < island->busN; i++) {
		char buffer[127];
		island->buses[i].toString(buffer);
		fout<<buffer<<endl;
	}
	fout<<BUS_CARD_END<<endl;


	fout<<"BRANCH DATA FOLLOWS                    "<<island->branchN<<" ITEMS"<<endl;
	for(int i = 0; i < island->branchN; i++) {
		char buffer[126];
		island->branches[i].toString(buffer);
		fout<<buffer<<endl;
	}
	fout<<BRANCH_CARD_END<<endl;

	fout<<"LOSS ZONES FOLLOWS   "<<island->lossZoneN<<" ITEMS"<<endl;
	for(int i = 0; i < island->lossZoneN; i++) {
		char buffer[16];	
		island->lossZones[i].toString(buffer);
		fout<<buffer<<endl;
	}
	fout<<LOSS_ZONE_CARD_END<<endl;

	fout<<"INTERCHANGE DATA FOLLOWS      "<<island->interchangeN<<" ITEMS"<<endl;
	for(int i = 0; i < island->interchangeN; i++) {
		char buffer[75];	
		island->interchanges[i].toString(buffer);
		fout<<buffer<<endl;
	}
	fout<<INTERCHANGE_CARD_END<<endl;

	fout<<"TIE LINES FOLLOWS             "<<island->tieLineN<<" ITEMS"<<endl;
	for(int i = 0; i < island->tieLineN; i++) {
		char buffer[21];
		island->tieLines[i].toString(buffer);
		fout<<buffer<<endl;
	}
	fout<<TIE_LINE_CARD_END<<endl;

	fout<<"END OF DATA"<<endl;
	fout.close();	
	return true;
}

bool DefaultIEEEFormatRW::write(char * filePath) {
	return write(filePath, this->island);
}

