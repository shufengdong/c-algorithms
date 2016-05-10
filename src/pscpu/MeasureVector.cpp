#include "MeasureVector.h"
#include <sstream>
#include <fstream>
#include <iomanip>

#include <iostream>
#include <string>

using namespace cpscpu;
void MeasureVector::parseFile(string filePath) {
	freeSpace();//free space first

	string templine;
	string tmpStr;
	stringstream lineStream;
	ifstream infile(filePath.c_str(),ios::in);
	getline(infile, templine, '\n');
	if(infile.fail()) {
		cout<<"MeasureVector file not exist"<<endl;
	} else{
		lineStream<<templine;
		lineStream>>tmpStr>>tmpStr>>tmpStr>>measNum;

		meas = new MeasureInfo[measNum];
		for(int i = 0; i < measNum; i++) {
			getline(infile, templine, '\n');
			meas[i].parseString(templine);
			//meas[i].weight = 1.0;
			//meas[i].weight = meas[i].weight*1e6;
			//if(abs(meas[i].weight * 1e6 - 1.0 / (meas[i].sigma * meas[i].sigma) ) > 100)
			//	cout<<i<<"\t"<<meas[i].weight * 1e6<<"\t"<<1.0 / (meas[i].sigma * meas[i].sigma)<<endl;
		}
	}
};

void MeasureVector::writeFile(string filePath) {
	ofstream outfile(filePath.c_str(),ios::out);
	string str;
	char ch[500];
	sprintf(ch,"MEASURE	DATA	 	FOLLOWS		%d ITEMS",measNum);
	outfile<<ch;
	for(int i = 0; i < measNum; i++) {
		outfile<<endl;
		char ch[500];
		meas[i].toString(ch);
		outfile<<ch;
	}
	outfile.close();
};


