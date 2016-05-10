#include<math.h>

#include "BasicCPF.h"
#include "MeasureConstants.h"
#include "slu_ddefs.h"

using namespace cpscpu;

BasicCPF::BasicCPF(){
	pf = NULL;
}

BasicCPF::BasicCPF(NewtonPF * pf){
	setPF(pf);
}

void BasicCPF::setPF(NewtonPF * pf) {
	this->pf = pf;
}

void BasicCPF::doCPF() {
	//prediction step: computation of delta X.
	//A = jacobian of x, b = - (jacobian of lamda) * (delta lamda)
	SparseMatrix jacobianOfX;
	double * result = new double(pf->getDimension());
	pf->getResult(result);
	pf->getJocobian(result, &jacobianOfX);

	double deltaLamda = 1.0;
	double * right = new double[pf->getDimension()];
	for(int i = 0; i < pf->getModel()->meas.measNum; i++) {
		int num = atoi(pf->getModel()->meas.meas[i].positionId.c_str());	
		int type = pf->getModel()->meas.meas[i].measureType;
		switch(type) {
			case TYPE_BUS_ANGLE:
				break;
			case TYPE_BUS_VOLTAGE:
				break;
			case TYPE_BUS_ACTIVE_POWER:
				right[i] = deltaLamda * model->getPIncreaseVector()[num - 1];
				break;
			case TYPE_BUS_REACTIVE_POWER:
				right[i] = deltaLamda * model->getQIncreaseVector()[num - 1];
				break;
			default:
				cout<<"<BasicCPF>unsupported measure type: "<<type<<endl;
				break;
		}
	}

	SuperMatrix aM,lM,uM,bM;
	
	int* aColSeq=new int[jacobianOfX.getN()+1];
	int nz=jacobianOfX.unzeroNum;
	double* aValues=new double[nz]; 
	int* aRowInd=new int[nz];

	double* pred=new double[pf->getDimension()];

	jacobianOfX.formMatrixByCol(aColSeq,aRowInd,aValues);

	dCreate_CompCol_Matrix(&aM,jacobianOfX.getM(),jacobianOfX.getN(),nz,aValues,aRowInd,aColSeq,SLU_NC,SLU_D,SLU_GE);
	dCreate_Dense_Matrix(&bM,pf->getDimension(),1,right,pf->getDimension(),SLU_DN,SLU_D,SLU_GE);

	int* perm_r=new int[jacobianOfX.getM()];
	int* perm_c=new int[jacobianOfX.getN()];
	superlu_options_t options;
	SuperLUStat_t stat;
	set_default_options(&options);
	options.ColPerm=NATURAL;
	StatInit(&stat);
	int info;

	dgssv(&options,&aM,perm_c,perm_r,&lM,&uM,&bM,&stat,&info);
	
	const double* b=reinterpret_cast<double*>(
		reinterpret_cast<DNformat*>(bM.Store)->nzval);
	
	double sqr=0.0;
	for(int i=0;i<pf->getDimension();i++){
		sqr+=b[i]*b[i];
	}
	sqr=sqrt(sqr);	
	for(int i=0;i<pf->getDimension();i++){
		pred[i]=b[i]/sqr;
	}

	//next is corrector step.


	//finish: corrector step.
	//free allocated spaces

	delete[] aValues;
	delete[] aColSeq;
	delete[] aRowInd;
	delete[] perm_r;
	delete[] perm_c;
	Destroy_CompCol_Matrix(&aM);
	Destroy_SuperMatrix_Store(&bM);
	Destroy_SuperNode_Matrix(&lM);
	Destroy_CompCol_Matrix(&uM);
	StatFree(&stat);

	//finish: free allocated spaces.
}
