#ifndef __cpscpu_AbstractSE_H__
#define	__cpscpu_AbstractSE_H__

#include "SEModel.h"

#define SE_MODE_VTHETA 0 //using v and theta as variables
#define SE_MODE_U 1 //using voltage's real and imag part as variables
#define SE_MODE_UI 2
#define SE_MODE_PQVTHETA 3 //using bus's p, q, v and theta as variables

namespace cpscpu {
	class AbstractSE {
	public:
		bool isDebug;

		AbstractSE();

		AbstractSE(SEModel * model);

		AbstractSE(SEModel * model, int mode);

		virtual ~AbstractSE(){};

		void setModel(SEModel * model) {
			this->model = model;
		};

		virtual void doSE() = 0;

		int getMode();

		void setMode(int mode);

	private:
		void initial();

		int mode;

		bool isPrintPath;

	protected:
		SEModel * model;

		bool initialModel();

		bool getIsPrintPath();

		void setIsPrintPath(bool);
	};
}

#endif