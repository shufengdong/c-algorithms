#include"YMatrix.h"
#include"MathUtil.h"
#include <cmath>

using namespace cpscpu;

YMatrix::YMatrix(){
	lineFrom = NULL;
	lineTo = NULL;
	lineFromAdmittance = NULL;
	lineToAdmittance = NULL;
}

void YMatrix::freeSpace(){
	if(lineFrom != NULL)
		delete[] lineFrom;
	if(lineTo != NULL)
		delete[] lineTo;
	if(lineFromAdmittance != NULL)
		delete[] lineFromAdmittance;
	if(lineToAdmittance != NULL)
		delete[] lineToAdmittance;

	lineFrom = NULL;
	lineTo = NULL;
	lineFromAdmittance = NULL;
	lineToAdmittance = NULL;
	admittance[0].freeSpace();
	admittance[1].freeSpace();
}

void YMatrix::getBranchAdmittance(BranchData branch, double v[][4]) {
	getBranchAdmittanceFrom(branch, v[0]);
	getBranchAdmittanceTo(branch, v[1]);
}

void YMatrix::getBranchAdmittanceFrom(BranchData branch, double v[]) {
	double r = branch.R;
	double x = branch.X;
	double g = r / (r * r + x * x);
	double b = -x / (r * r + x * x);    
	if (branch.type == 0) {
		v[0] = g;
		v[1] = b;
		v[2] = 0.0;
		v[3] = branch.lineB / 2.0;
	} else {
		double c = 1 / branch.transRatio;
		double tmp = c * (c - 1);
		v[0] = c * g;
		v[1] = c * b;
		v[2] = tmp * g;
		v[3] = tmp * b;
	}
}

void YMatrix::getBranchAdmittanceTo(BranchData branch, double v[4]) {
	double r = branch.R;
	double x = branch.X;
	double g = r / (r * r + x * x);
	double b = -x / (r * r + x * x);    
	if (branch.type == 0) {
		v[0] = g;
		v[1] = b;
		v[2] = 0.0;
		v[3] = branch.lineB / 2.0;
	} else {
		double c = 1 / branch.transRatio;
		double tmp = 1 - c;
		v[0] = c * g;
		v[1] = c * b;
		v[2] = tmp * g;
		v[3] = tmp * b;
	}
}

void YMatrix::formMatrix(IEEEDataIsland* island) {
	freeSpace();
	int n = island->busN;
	admittance[0].initial(n, n);	
	admittance[1].initial(n, n);
	lineFrom = new int[island->branchN];
	lineTo = new int[island->branchN];
	lineFromAdmittance = new double[4 * island->branchN];
	lineToAdmittance = new double[4 * island->branchN];
	for(int i = 0; i < island->branchN; i++) {		
		int f = island->branches[i].tapBusNumber - 1;
		int t = island->branches[i].zBusNumber - 1;
		lineFrom[i] = f;
		lineTo[i] = t;
		double v[2][4];//todo: not good
		getBranchAdmittance(island->branches[i], v);
		getBranchAdmittanceFrom(island->branches[i], lineFromAdmittance + 4 * i);
		getBranchAdmittanceTo(island->branches[i], lineToAdmittance + 4 * i);

		double r = island->branches[i].R;
		double x = island->branches[i].X;
		double g = r / (r * r + x * x);
		double b = -x / (r * r + x * x);

		//special for branchType4
		if(island->branches[i].type == 4) {		
			//cout<<"假设island中角度为[-180,180]"<<endl;
			double Ktheta = island->branches[i].transAngle*PI / 180;
			if (fabs(Ktheta) < 1e-4) {
				//cout<<"移相机支路的角度量约为0！"<<endl;
				continue;
			}
			double Kamplitude = island->branches[i].transRatio;
			double k1 = Kamplitude * cos(Ktheta);
			double k2 = Kamplitude * sin(Ktheta);
			double base = Kamplitude * Kamplitude;

			admittance[0].setValue(t, t, g, true);
			admittance[1].setValue(t, t, b, true);
			admittance[0].setValue(f, f, g / base, true);
			admittance[1].setValue(f, f, b / base, true);

			double temp = -g * k1 - b * k2;
			admittance[0].setValue(t, f, temp / base, true);
			temp = -b * k1 + g * k2;
			admittance[1].setValue(t, f, temp / base, true);
			temp = -g * k1 + b * k2;
			admittance[0].setValue(f, t, temp / base, true);
			temp = -b * k1 - g * k2;
			admittance[1].setValue(f, t, temp / base, true);

			continue;
		}
		else{
			admittance[0].setValue(f, f, v[0][0] + v[0][2], true);
			admittance[1].setValue(f, f, v[0][1] + v[0][3], true);

			admittance[0].setValue(t, t, v[1][0] + v[1][2], true);
			admittance[1].setValue(t, t, v[1][1] + v[1][3], true);


			admittance[0].setValue(f, t, -v[0][0], true);
			admittance[1].setValue(f, t, -v[0][1], true);
			admittance[0].setValue(t, f, -v[1][0], true);
			admittance[1].setValue(t, f, -v[1][1], true);
		}
	}

	for(int i = 0; i < island->busN; i++) {
		int tmp = island->buses[i].busNumber - 1;
		admittance[0].setValue(tmp, tmp, island->buses[i].G, true);
		admittance[1].setValue(tmp, tmp, island->buses[i].B, true);
	}

}
