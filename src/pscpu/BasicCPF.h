#ifndef __cpscpu_BasicCPF_H__
#define	__cpscpu_BasicCPF_H__

#include "CPFModel.h"
#include "NewtonPF.h"

namespace cpscpu {
	class BasicCPF {
		public:
		
		BasicCPF();

		BasicCPF(NewtonPF * pf);
		
		void doCPF();
		
		void setPF(NewtonPF * pf);

		protected:
		CPFModel * model;

		private:
		NewtonPF * pf;
		double lamda;
	};
}

#endif