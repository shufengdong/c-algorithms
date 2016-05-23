#include "NewtonAlg.h"
#include <math.h>
#include "slu_ddefs.h"
#include "MeasureConstants.h"
#include "SparseMatrix.h"


using namespace std;
using namespace cpscpu;

bool NewtonAlg::cal(NewtonModel * model, double * result) {
    int iterNum = 1;
	int maxIter = model->getMaxIter();

	int dim = model->getDimension();
	double * z = new double[dim];
	double * z_est = new double[dim];	
	AbstractSM * matrix = new SparseMatrix(dim, dim);
	bool isConverged = false;

	model->getInitial(result);
    model->getZ(z);
	while (iterNum <= maxIter) {
        iterNum++;
        //compute estimated measurement
        model->calZ(result, z_est);

		SuperMatrix A, L, U, B;
		double   *a, *rhs;
		int      *asub, *xa;
		int      *perm_r; /* row permutations from partial pivoting */
		int      *perm_c; /* column permutation vector */
		int      nrhs, info, i, m, n, nnz, permc_spec;
		superlu_options_t options;
		SuperLUStat_t stat;
        
		
        //-----  evaluate jacobian  -----       
		model->getJocobian(result, matrix);
         /* Initialize matrix A. */
		m = matrix->getM();
		n = matrix->getN();
		nnz =matrix->getUnzeroNum();
		if ( !(a = doubleMalloc(nnz)) ) ABORT("Malloc fails for a[].");
		if ( !(asub = intMalloc(nnz)) ) ABORT("Malloc fails for asub[].");
		if ( !(xa = intMalloc(n+1)) ) ABORT("Malloc fails for xa[].");		
		/*int count = 0;
		for(int i = 0; i < n; i++) {
			xa[i] = count;			
			MatrixNode * current =  h->matrix->colHeadNode[i].down;
			while(current !=  h->matrix->colHeadNode + i){								
				a[count] = current->triple.value;
				asub[count++] = current->triple.row;
				current = current->down;
			}			
		}
		xa[n] = count;*/
		matrix->formMatrixByCol(xa, asub, a);

		dCreate_CompCol_Matrix(&A, m, n, nnz, a, asub, xa, SLU_NC, SLU_D, SLU_GE);
		
		/* Create right-hand side matrix B. */
	    nrhs = 1;	
		if ( !(rhs = doubleMalloc(m * nrhs)) ) ABORT("Malloc fails for rhs[].");
		for (i = 0; i < m; ++i) rhs[i] = z[i] - z_est[i];
	    dCreate_Dense_Matrix(&B, m, nrhs, rhs, m, SLU_DN, SLU_D, SLU_GE);

		if ( !(perm_r = intMalloc(m)) ) ABORT("Malloc fails for perm_r[].");
		if ( !(perm_c = intMalloc(n)) ) ABORT("Malloc fails for perm_c[].");

		/* Set the default input options. */
		set_default_options(&options);
		options.ColPerm = NATURAL;

		/* Initialize the statistics variables. */
		StatInit(&stat);
    
		dgssv(&options, &A, perm_c, perm_r, &L, &U, &B, &stat, &info);
    
		//check for convergence    
		double max = 0;
		for (int i = 0; i < n; i++) {
			double error = abs(((double*)((DNformat*)B.Store)->nzval)[i]);
			if (error > max)
				max = error;
		}
		if (max < model->getTolerance())
			isConverged = true;
        
        //update state
		for (int i = 0; i < n; i++) 
            result[i] += ((double*)((DNformat*)B.Store)->nzval)[i];
        
		/* De-allocate storage */
		SUPERLU_FREE (rhs);
		SUPERLU_FREE (perm_r);
		SUPERLU_FREE (perm_c);
		Destroy_CompCol_Matrix(&A);
		Destroy_SuperMatrix_Store(&B);
		Destroy_SuperNode_Matrix(&L);
		Destroy_CompCol_Matrix(&U);
		StatFree(&stat);
    
		if(isConverged)
			break;
    }	
	delete[] z;
	delete[] z_est;	
	delete matrix;
    return isConverged;
}

bool NewtonAlg::cal2(NewtonModel * model, double * result) {
    int iterNum = 1;
	int maxIter = model->getMaxIter();

	int dim = model->getDimension();
  	double * z = new double[dim];
	model->getInitial(result);
    model->getZ(z);
	bool isConverged = false;
	
	char           equed[1];
	yes_no_t       equil;
	trans_t        trans;
	SuperMatrix    A, L, U;
	SuperMatrix    B, X;
	NCformat       *Astore;
	NCformat       *Ustore;
	SCformat       *Lstore;
	double         *a;
	int            *asub, *xa;
	int            *perm_r; /* row permutations from partial pivoting */
	int            *perm_c; /* column permutation vector */
	int            *etree;
	void           *work;
	int            info, lwork, nrhs, ldx;
	int            i, j, m, n, nnz;
	double         *rhsb, *rhsx;
	double         *R, *C;
	double         *ferr, *berr;
	double         u, rpg, rcond;
	mem_usage_t    mem_usage;
	superlu_options_t options;
	SuperLUStat_t stat;
	// Defaults
	lwork = 0;
	nrhs  = 1;
	equil = YES;	
	u     = 1.0;
	trans = NOTRANS;
	set_default_options(&options);

	options.Equil = equil;
	options.DiagPivotThresh = u;
	options.Trans = trans;

	if ( lwork > 0 ) {
		work = SUPERLU_MALLOC(lwork);
		if ( !work ) {
			ABORT("DLINSOLX: cannot allocate work[]");
		}
	}
	
	AbstractSM * matrix = new SparseMatrix(dim, dim);
	model->getJocobian(result, matrix);
    double * z_est = new double[dim];
	model->calZ(result, z_est);// evalue estimated z

	/* Initialize matrix A. */
	m = matrix->getM();
	n = matrix->getN();
	nnz = matrix->getUnzeroNum();
	if ( !(a = doubleMalloc(nnz)) ) ABORT("Malloc fails for a[].");
	if ( !(asub = intMalloc(nnz)) ) ABORT("Malloc fails for asub[].");
	if ( !(xa = intMalloc(n+1)) ) ABORT("Malloc fails for xa[].");		
	/*int count = 0;
	for(i = 0; i < n; i++) {
		xa[i] = count;			
		MatrixNode * current =  h->matrix.colHeadNode[i].down;
		while(current !=  h->matrix.colHeadNode + i){								
			a[count] = current->triple.value;
			asub[count++] = current->triple.row;
			current = current->down;
		}			
	}
	xa[n] = count;*/
	matrix->formMatrixByCol(xa, asub, a);

	dCreate_CompCol_Matrix(&A, m, n, nnz, a, asub, xa, SLU_NC, SLU_D, SLU_GE);
    Astore = (NCformat*)A.Store;
    printf("Dimension %dx%d; # nonzeros %d\n", A.nrow, A.ncol, Astore->nnz);
    
    /* Create right-hand side matrix B and X. */
	if ( !(rhsb = doubleMalloc(m * nrhs)) ) ABORT("Malloc fails for rhsb[].");
    if ( !(rhsx = doubleMalloc(m * nrhs)) ) ABORT("Malloc fails for rhsx[].");	
	for (i = 0; i < m; ++i) {
		rhsb[i] = z[i] - z_est[i];
		rhsx[i] = z[i] - z_est[i];
	}
    dCreate_Dense_Matrix(&B, m, nrhs, rhsb, m, SLU_DN, SLU_D, SLU_GE);
    dCreate_Dense_Matrix(&X, m, nrhs, rhsx, m, SLU_DN, SLU_D, SLU_GE);
    
    if ( !(perm_c = intMalloc(n)) ) ABORT("Malloc fails for perm_c[].");
    if ( !(perm_r = intMalloc(m)) ) ABORT("Malloc fails for perm_r[].");
    if ( !(etree = intMalloc(n)) ) ABORT("Malloc fails for etree[].");
    if ( !(R = (double *) SUPERLU_MALLOC(A.nrow * sizeof(double))) ) 
        ABORT("SUPERLU_MALLOC fails for R[].");
    if ( !(C = (double *) SUPERLU_MALLOC(A.ncol * sizeof(double))) )
        ABORT("SUPERLU_MALLOC fails for C[].");
    if ( !(ferr = (double *) SUPERLU_MALLOC(nrhs * sizeof(double))) )
        ABORT("SUPERLU_MALLOC fails for ferr[].");
    if ( !(berr = (double *) SUPERLU_MALLOC(nrhs * sizeof(double))) ) 
        ABORT("SUPERLU_MALLOC fails for berr[].");

    /* Initialize the statistics variables. */
    StatInit(&stat);
    
    /* ------------------------------------------------------------
       WE SOLVE THE LINEAR SYSTEM FOR THE FIRST TIME: AX = B
       ------------------------------------------------------------*/
    dgssvx(&options, &A, perm_c, perm_r, etree, equed, R, C,
           &L, &U, work, lwork, &B, &X, &rpg, &rcond, ferr, berr,
           &mem_usage, &stat, &info);    
    
    if ( options.PrintStat ) StatPrint(&stat);
    StatFree(&stat);
	//update state
	for (int i = 0; i < n; i++) 
		result[i] += ((double*)((DNformat*)X.Store)->nzval)[i];
	while (iterNum <= maxIter) {
        iterNum++;        
		model->getJocobian(result, matrix);// evaluate jacobian 

		/*count = 0;
		for(i = 0; i < n; i++) {			
			MatrixNode * current =  h->matrix.colHeadNode[i].down;
			while(current !=  h->matrix.colHeadNode + i){					
				a[count++] = current->triple.value;
				current = current->down;
			}			
		}*/
		matrix->formValueByCol(a);

		model->calZ(result, z_est);// evalue estimated z
		for (i = 0; i < m; ++i)
			rhsb[i] = z[i] - z_est[i];
		
		/* ------------------------------------------------------------
			NOW WE SOLVE ANOTHER LINEAR SYSTEM: A1*X = B1
			ONLY THE SPARSITY PATTERN OF A1 IS THE SAME AS THAT OF A.
			------------------------------------------------------------*/
		options.Fact = SamePattern;
		StatInit(&stat); /* Initialize the statistics variables. */

		dCreate_CompCol_Matrix(&A, m, n, nnz, a, asub, xa,
                           SLU_NC, SLU_D, SLU_GE);
		dCreate_Dense_Matrix(&B, m, nrhs, rhsb, m, SLU_DN, SLU_D, SLU_GE);

		dgssvx(&options, &A, perm_c, perm_r, etree, equed, R, C,
           &L, &U, work, lwork, &B, &X, &rpg, &rcond, ferr, berr,
           &mem_usage, &stat, &info);	
	
		StatFree(&stat);		

		//check for convergence    
		//todo: check stat and info first!
		double max = 0;
		for (int i = 0; i < n; i++) {
			double error = abs(((double*)((DNformat*)X.Store)->nzval)[i]);
			if (error > max)
				max = error;
		}
		if (max < model->getTolerance())
			isConverged = true;
      
		if(isConverged) 
			break;
        //update state
		for (int i = 0; i < n; i++) 
            result[i] += ((double*)((DNformat*)X.Store)->nzval)[i];			
    }	
	cout<<"It iters "<<iterNum<<" times."<<endl;
	Destroy_CompCol_Matrix(&A);
    Destroy_Dense_Matrix(&B);
    SUPERLU_FREE (etree);
    SUPERLU_FREE (perm_r);
    SUPERLU_FREE (perm_c);
    SUPERLU_FREE (R);
    SUPERLU_FREE (C);
    SUPERLU_FREE (ferr);
    SUPERLU_FREE (berr);
    Destroy_Dense_Matrix(&X);
	Destroy_SuperNode_Matrix(&L);
    Destroy_CompCol_Matrix(&U);
	delete[] z;	
	delete[] z_est;
	delete matrix;
	return isConverged;
}