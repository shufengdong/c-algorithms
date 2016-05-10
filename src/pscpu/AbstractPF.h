#ifndef __cpscpu_AbstractPF_H__
#define	__cpscpu_AbstractPF_H__

#define PF_MODE_VTHETA 0
#define PF_MODE_RECT 1

#include"PFModel.h"

namespace cpscpu {
	class AbstractPF {		
		protected:
		PFModel * model;
		int iterTimes;	
		int mode;
		bool isPrintPath;
		bool isDebug;

		public:

		virtual void doPF() = 0;	
	
		AbstractPF();

		AbstractPF(PFModel * model);

		AbstractPF(PFModel * model, int mode);

		virtual ~AbstractPF(){};

		void setModel(PFModel * model);
				
		int getIterTimes();
		
		bool getIsPrintPath();

		void setIsPrintPath(bool);

		int getMode();

		void setMode(int mode);

		PFModel * getModel();

		protected:
		bool initialModel();
		
		private:
		void initial();

	};
}//end of namespace

#endif