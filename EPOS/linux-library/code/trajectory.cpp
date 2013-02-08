#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
using namespace std;

#include "trajectory.h"

double T = 2.0;
double t0 = 0;

int M = 10;

const double gearRatio = 729.0/25.0;
const double angleToQC = (512.0*4)/(2*M_PI) * gearRatio;
const double angVelToRPM = (60.0)/(2*M_PI) * gearRatio;

long getTheta(double t, double thetaG) {
	double theta = (2*M_PI/T) * t;
	for (int m = 1; m <= M; m++) {
		double B = 4*(thetaG-M_PI)/((2*m-1)*(2*m-1)*M_PI*M_PI);
		theta += B*(1-cos(2*M_PI*(2*m-1)*t/T));
	}
	return round(theta * angleToQC);
}

long getThetaDot(double t, double thetaG) {
	double thetaDot = (2*M_PI/T);
	for (int m = 1; m <= M; m++) {
		double B = 8*(thetaG-M_PI)/((2*m-1)*M_PI*T);
		thetaDot += B*sin(2*M_PI*(2*m-1)*t/T);
	}
	return round2(thetaDot * angVelToRPM);
}

long round2(double r) {
    return (long)((r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5));
}
