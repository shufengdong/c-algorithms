#ifndef __cpscpu_AbstractPFCal_H__
#define	__cpscpu_AbstractPFCal_H__

#include"PFModel.h"

namespace cpscpu{
	class AbstractPFCal {
	public:

		virtual void doPF() = 0;	

		bool isDebug;
		double tol;

		AbstractPFCal(){
			initial();
		};

		AbstractPFCal(PFModel * model) {
			initial();
			this->model = model;			
		};

		void setModel(PFModel * model) {
			this->model = model;
		};

	private:
		void initial() {
			isDebug = false;
			tol = 1e-4;
		};
	protected:
		PFModel * model;
	};
}//end of namespace

#endif