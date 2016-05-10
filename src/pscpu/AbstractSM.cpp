#include "AbstractSM.h"
#include<cassert>
#include<fstream>

using namespace std;
using namespace cpscpu;

void AbstractSM::printFullMatrix(char* filepath) {
	ofstream out;
	out.open(filepath,ios::out);
	if(!out) {
		cout<<"Failed to write matrix to file: "<<filepath<<endl;
		return;
	}

	int i,j,jj;
	double d;
	for( jj=0; jj<n ; jj+=8){
		out.width(5);
		out<<" r\\c:";
		for( j=jj; j<jj+8 && j<n ; j++){
			out.width(12);
			out<<j;
		}
		out<<endl;
		for( i = 0; i < m; i++) {
			out.width(4);
			out<<i<<":";
			for( j =jj; j<jj+8 && j<n ; j++){
				d = getValue(i,j);
				if( d == -9999) d =0.0;
				out.width(12);
				out.precision(4);
				out<<d;
			}
			out<<endl;
		}
		out<<endl;
	}
	cout<<"Logged "<<filepath<<endl;
	out.close();
}
