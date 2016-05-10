#include "SparseMatrix.h"
#include<cassert>
#include<fstream>

using namespace std;
using namespace cpscpu;

SparseMatrix::SparseMatrix(){
	headNode = NULL;
	colHeadNode = NULL;
}

SparseMatrix::SparseMatrix(int rows, int cols)  {	
	headNode = NULL;
	colHeadNode = NULL;
	initial(rows, cols);
}

SparseMatrix::~SparseMatrix(){
	freeSpace(); 
}

void SparseMatrix::setValue(int i, int j, double value, bool isAddTo) {
	MatrixNode * pre = headNode + i;
	MatrixNode * current = headNode[i].right;
	while(current != headNode + i) {
		if(current->triple.col == j) {
			if(isAddTo) current->triple.value += value;
			else current->triple.value = value;
			return;
		} else if(current->triple.col > j) 
			break;
		pre = current;
		current = current->right;
	}
	
	MatrixNode * node = new MatrixNode();
	node->triple.row = i;
	node->triple.col = j; 
	node->triple.value = value;
	
	node->right = current;
	pre->right = node;
	unzeroNum++;

	MatrixNode * pre2 = colHeadNode + j;
	MatrixNode * current2 = colHeadNode[j].down;
	while(current2 != colHeadNode + j) {
		if(current2->triple.row > i) 
			break;
		pre2 = current2;
		current2 = current2->down;
	}
	node->down = current2;
	pre2->down = node;
}

void SparseMatrix::freeSpace(){
	if(headNode == NULL && colHeadNode == NULL)
		return;
	for(int i = 0; i < m; i++) {
		MatrixNode * current = headNode[i].right;
		while(current != headNode + i){				
			MatrixNode * t = current->right;
			delete current;
			current = t;
		}	
	}
	delete[] headNode;
	delete[] colHeadNode;
	headNode = NULL;
	colHeadNode = NULL;
	unzeroNum = 0; 
	m = 0; 
	n = 0;
}

void SparseMatrix::initial(int rows, int cols) {
	freeSpace();
	m = rows;
	n = cols;

	headNode = new MatrixNode[m];	
	for(int i = 0; i < m; i++)
		headNode[i].right = headNode + i;	
	
	colHeadNode = new MatrixNode[n];	
	for(int i = 0; i < n; i++)
		colHeadNode[i].down = colHeadNode + i;
}

void SparseMatrix::setValue(int i, int j, double value) {
	setValue(i, j, value, false);
}

double SparseMatrix::getValue(int i, int j) {
	MatrixNode * current = headNode[i].right;
	while(current != headNode + i) {
		if(current->triple.col == j)
			return current->triple.value;		
		current = current->right;
	}
	return -9999;//?
}

void SparseMatrix::add(SparseMatrix * matrix, double w) {
	assert(matrix->m == m);
	assert(matrix->n == n);
	for(int i = 0; i < matrix->m; i++) {
		MatrixNode * current = matrix->headNode[i].right;
		while(current != matrix->headNode + i){
			setValue(current->triple.row, current->triple.col, current->triple.value * w);
			current = current->right;
		}
	}
}

double SparseMatrix::rowSum(int i){
	double rowSum = 0.0;
	MatrixNode * current_row = headNode[i].right;
	while(current_row != headNode + i) {
		rowSum += current_row->triple.value;
		current_row = current_row->right;
	}
	return rowSum;
}

void SparseMatrix::printOnScreen() {
	for(int i = 0; i < m; i++) {
		MatrixNode * current = headNode[i].right;
		while(current != headNode + i){
			cout<<current->triple.row<<"\t"<<current->triple.col<<"\t"<<current->triple.value<<endl;
			current = current->right;
		}
	}
}

void SparseMatrix::getStructure(int * iRow, int * jCol) {
	int count = 0;
	for(int i = 0; i < m; i++) {
		MatrixNode * current = headNode[i].right;
		while(current != headNode + i){				
			iRow[count] = current->triple.row;
			jCol[count] = current->triple.col;
			count++;
			current = current->right;
		}
	}	
}

void SparseMatrix::makeZero() {
	for(int i = 0; i < m; i++) {
		MatrixNode * current = headNode[i].right;
		while(current != headNode + i){				
			current->triple.value = 0;
			current = current->right;
		}
	}	
}

void SparseMatrix::formMatrixByRow(int * unzeroNumEachRow, int * cols, double * values) {
	int count = 0;
	for(int i = 0; i < getM(); i++) {
		unzeroNumEachRow[i] = count;			
		MatrixNode * current = headNode[i].right;
		while(current != headNode + i){								
			values[count] = current->triple.value;
			cols[count++] = current->triple.col;
			current = current->right;
		}			
	}	
	unzeroNumEachRow[getM()] = count;
}

void SparseMatrix::formMatrixByCol(int * unzeroNumEachCol, int * rows, double * values) {
	int count = 0;
	for(int i = 0; i < getN(); i++) {
		unzeroNumEachCol[i] = count;			
		MatrixNode * current = colHeadNode[i].down;
		while(current != colHeadNode + i){								
			values[count] = current->triple.value;
			rows[count++] = current->triple.row;
			current = current->down;
		}			
	}
	unzeroNumEachCol[getN()] = count;
}

void SparseMatrix::formValueByRow(double * values) {
	int count = 0;
	for(int i = 0; i < getM(); i++) {			
		MatrixNode * current =  headNode[i].right;
		while(current !=  headNode + i){					
			values[count++] = current->triple.value;
			current = current->right;
		}			
	}
}

void SparseMatrix::formValueByCol(double * values) {
	int count = 0;
	for(int i = 0; i < getN(); i++) {			
		MatrixNode * current =  colHeadNode[i].down;
		while(current != colHeadNode + i){					
			values[count++] = current->triple.value;
			current = current->down;
		}			
	}
}

void SparseMatrix::getValues(double * values) {
	int count = 0;
	for(int i = 0; i < m; i++) {
		MatrixNode * current = headNode[i].right;
		while(current != headNode + i){
			values[count++] = current->triple.value;
			current = current->right;
		}
	}		
}