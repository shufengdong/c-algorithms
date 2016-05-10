#ifndef __cpscpu_MathUtil_H__
#define __cpscpu_MathUtil_H__

#include <math.h>

#define PI 3.1415926535897932384626433832795

static void trans_rect2polar(double * toTrans) {
    double a = toTrans[0];
    double b = toTrans[1];
    toTrans[0] = sqrt(a * a + b * b);
    toTrans[1] = atan2(b, a);
};

static void trans_polar2rect(double* toTrans) {
    double a = toTrans[0];
    double b = toTrans[1];
    toTrans[0] = a * cos(b);
    toTrans[1] = a * sin(b);
};

static void trans_polar2rect(double* toTrans, double* result) {
    double a = toTrans[0];
    double b = toTrans[1];
    result[0] = a * cos(b);
    result[1] = a * sin(b);
};

/**
 * @param toTrans the rectangular coordinates values
 */
static void trans_rect2polar(double** toTrans) {
	for (int i = 0; i < 3; i++) {
	  double a = toTrans[i][0];
	  double b = toTrans[i][1];
	  toTrans[i][0] = sqrt(a * a + b * b);
	  toTrans[i][1] = atan2(b, a);
	}
};

/**
* conversion of polar coordinates (v theta) to rectangular coordinates (x,?0?2y)
*
* @param toTrans the polar coordinates values
*/
static void trans_polar2rect(double** toTrans) {
	for (int i = 0; i < 3; i++) {
	  double a = toTrans[i][0];
	  double b = toTrans[i][1];
	  toTrans[i][0] = a * cos(b);
	  toTrans[i][1] = a * sin(b);
	}
};

/**
* conversion of polar coordinates (r,?0?2theta) to rectangular coordinates (x,?0?2y)
*
* @param toTrans the polar coordinates values
* @param result  storage of lineImpedance
*/
static void trans_polar2rect(double** toTrans, double** result) {
	for (int i = 0; i < 3; i++) {
	  double a = toTrans[i][0];
	  double b = toTrans[i][1];
	  result[i][0] = a * cos(b);
	  result[i][1] = a * sin(b);
	}
};

#endif