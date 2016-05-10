#include "MapSparseMatrix.h"
#include<cassert>
#include<fstream>

using namespace cpscpu;

MapSparseMatrix::MapSparseMatrix(){
}

MapSparseMatrix::MapSparseMatrix(int rows, int cols)  {	
	initial(rows, cols);
}

MapSparseMatrix::~MapSparseMatrix(){
	freeSpace(); 
}

void MapSparseMatrix::setValue(int i, int j, double value, bool isAddTo) {
	int key = n * i +j;
	if(isAddTo) {//pair exist, only need to add value
		iter = dataMap.find(key);
		if(iter == dataMap.end()){//pair not exists before
			dataMap[key] = value;
			unzeroNum++;
		} else{
			dataMap[key] += value;
		}
	} else{//check exists of pair
		iter = dataMap.find(key);
		if(iter == dataMap.end())//pair not exists before
			unzeroNum++;
		dataMap[key] = value;//不管key是否存在，均是直接赋值操作
	}
}
void MapSparseMatrix::freeSpace(){
	dataMap.clear();
	unzeroNum = 0; 
	m = 0; 
	n = 0;
}

void MapSparseMatrix::initial(int rows, int cols) {
	freeSpace();
	m = rows;
	n = cols;
}

void MapSparseMatrix::setValue(int i, int j, double value) {
	setValue(i, j, value, false);
}

double MapSparseMatrix::getValue(int i, int j) {
	int key = m * i + j;
	iter = dataMap.find(key);
	if(iter == dataMap.end()){//pair not exists before
		return -9999;		
	} else
		return dataMap[key];			
}

void MapSparseMatrix::printOnScreen() {
	for (iter = dataMap.begin(); iter != dataMap.end(); iter++)	{
		int key = iter->first;
		double value = iter->second;		
		int i = key / n;
		int j = key % n;		
		cout<<i<<"\t"<<j<<"\t"<<value<<endl;
	}
}

void MapSparseMatrix::getStructure(int * iRow, int * jCol) {
	int count = 0;
	for (iter = dataMap.begin(); iter != dataMap.end(); iter++)	{
		int key = iter->first;
		double vulue = iter->second;
		int i = key / n;
		int j = key % n;		
		iRow[count] = i;
		jCol[count] = j;
		count++;
	}
}

void MapSparseMatrix::makeZero() {
	for (iter = dataMap.begin(); iter != dataMap.end(); iter++)	
		iter->second = 0;		
}

void MapSparseMatrix::formMatrixByRow(int * unzeroNumEachRow, int * cols, double * values) {
	cout<<"not implemented"<<endl;
}

void MapSparseMatrix::formMatrixByCol(int * unzeroNumEachCol, int * rows, double * values) {
	cout<<"not implemented"<<endl;
}

void MapSparseMatrix::formValueByRow(double * values) {
	cout<<"not implemented"<<endl;
}

void MapSparseMatrix::formValueByCol(double * values) {
	cout<<"not implemented"<<endl;
}

void MapSparseMatrix::getValues(double * values) {
	int count = 0;
	for (iter = dataMap.begin(); iter != dataMap.end(); iter++)	{
		int key = iter->first;
		double vulue = iter->second;
		values[count++] = vulue;
	}
}
