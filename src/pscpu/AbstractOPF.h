#ifndef __cpscpu_AbstractOPF_H__
#define	__cpscpu_AbstractOPF_H__

#include"OPFModel.h"
#include"OPFObjFunc.h"

namespace cpscpu {
	class AbstractOPF {
		public:

		virtual void doOPF() = 0;	

		AbstractOPF();

		AbstractOPF(OPFModel * model, int mode);

		AbstractOPF(OPFModel * model);

		void setModel(OPFModel * model);

		private:

		void initial();
	
		protected:

		OPFModel * model;

		OPFObjFunc * objFunc;
			
		int mode;

		bool isPrintPath;

		bool isDebug;

		public:
	
		bool initialModel();

		bool getIsPrintPath();

		void setIsPrintPath(bool);

		int getMode();

		void setMode(int mode);

		void setObjFunc(OPFObjFunc * obj);

		OPFObjFunc * getObjFunc();
	};
}//end of namespace

#endif