#include"PFModel_CI.h"
#include"PFUtil.h"
#include<math.h>

using namespace cpscpu;

void PFModel_CI::initial() {	
	
	PFUtil::find_CNnode(island);
	PFUtil::formMeas4PF_CI(island, &meas);
	y.formMatrix(island);

	int n = (*island).busN;
	initialX = new double[4 * n];//e(0_n-1),f,ix,iy(3n_4n-1)
	int busNum_1 = 0;
	for (int i =0; i < n; i++) {	
		busNum_1 = (*island).buses[i].busNumber - 1;
		if ((*island).buses[i].busType == 3) {
			double v = (*island).buses[i].finalVoltage;
			double a = (*island).buses[i].finalAngle * PI / 180.0;
			initialX[busNum_1] =  v * cos(a);  //设定e,f
			initialX[busNum_1 + n] = v * sin(a);
			initialX[busNum_1 + 2*n] = y.admittance[0].rowSum( busNum_1 );  //设定ix,iy
			initialX[busNum_1 + 3*n] = y.admittance[1].rowSum( busNum_1 );

		} else if((*island).buses[i].busType == 4){
			initialX[busNum_1] =  1.0;  //设定e,f
			initialX[busNum_1 + n] =  0.0;  
			initialX[busNum_1 + 2*n] = 0.0;  //设定ix,iy
			initialX[busNum_1 + 3*n] = 0.0;
		} else{
			initialX[busNum_1] =  1.0;  //设定e,f
			initialX[busNum_1 + n] =  0.0;  
			initialX[busNum_1 + 2*n] = y.admittance[0].rowSum( busNum_1 );  //设定ix,iy
			initialX[busNum_1 + 3*n] = y.admittance[1].rowSum( busNum_1 );
		}
	}
	
}
