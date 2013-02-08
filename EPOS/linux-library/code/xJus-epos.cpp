#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <time.h>
using namespace std;

#include "Definitions.h"
#include "xJus-epos.h"
#include "trajectory.h"

// Protocol information
char deviceName[] = "EPOS2";
char protocolStackName[] = "EPOS2_USB";
char interfaceName[] = "USB";
char portName[] = "USB0";

// Protocol stack settings
const unsigned long BAUDRATE = 1000000;
const unsigned long TIMEOUT  = 500;

// Variables that hold error states
unsigned long errorCode;
const unsigned short ERROR_INFO_LENGTH = 100;
char errorInfo[ERROR_INFO_LENGTH];

// Handle of the connected device
void* device;

// Node IDs

unsigned int FL = 1;
unsigned int FR = 2;

const int NODE_COUNT = 2;
const int REV = 59720;

int dt = 50;
extern double T;
double baseThetaG = M_PI/4;

// Positions
//long position[NODE_COUNT];

int main() {

	errorCode = 0x10000001;
	printError();

	openDevice();
	clearAllFaults();
	enableAll();

	profilePositionMode(FL);
	profilePositionMode(FR);
	setPositionProfile(FL, 500, 5000, 5000);
	setPositionProfile(FR, 500, 5000, 5000);

	cout << "Press enter to stand...\n";
	cin.ignore();
	stand();
	wait();

	zeroPositionAll();

	//setPositionProfile(FL, 500, 5000, 5000);
	//setPositionProfile(FR, 500, 5000, 5000);

	cout << "Press enter to walk...\n";
	cin.ignore();
	walk(T*2, baseThetaG*0);
	wait();

	profilePositionMode(FL);
	profilePositionMode(FR);
	moveAbsolute(FL, +REV*2);
	moveAbsolute(FR, -REV*2);

	cout << "Press enter to disable...\n";
	cin.ignore();

	disableAll();
	closeDevice();
	return 0;
}


void printError() {
	VCS_GetErrorInfo(errorCode, errorInfo, ERROR_INFO_LENGTH);
	if (errorCode)
		cout << "ERROR: " << errorInfo << endl;
}

void openDevice() {
	cout << "Attempting connection to " << deviceName;
	cout << " at " << portName << "..." << endl;
	device = VCS_OpenDevice(deviceName, protocolStackName, interfaceName, portName, &errorCode);
	printError();
	cout << "Connected Device" << ": " << device << endl;

	cout << "Protocol stack settings are ";
	cout << "baudrate " << BAUDRATE << " and timeout " << TIMEOUT << "." << endl;
	VCS_SetProtocolStackSettings(device, BAUDRATE, TIMEOUT, &errorCode);
	printError();

	cout << endl;
}

void clearFault(unsigned int node) {
	cout << "Clearing fault at node " << node << "..." << endl;
	VCS_ClearFault(device, node, &errorCode);
	printError();
}

void clearAllFaults() {
	clearFault(FL);
	clearFault(FR);
	cout << endl;
}

void enable(unsigned int node) {
	cout << "Enabling node " << node << "..." << endl;
	VCS_SetEnableState(device, node, &errorCode);
	printError();
}

void enableAll() {
	enable(FL);
	enable(FR);
	cout << endl;
}

void disable(unsigned int node) {
	cout << "Disabling node " << node << "..." << endl;
	VCS_SetDisableState(device, node, &errorCode);
	printError();
}

void disableAll() {
	disable(FL);
	disable(FR);
	cout << endl;
}

void closeDevice() {
	cout << "Closing connection to device." << endl;
	VCS_CloseDevice(device, &errorCode);
	printError();
}

long getPosition(unsigned int node) {
	long pos;
	VCS_GetPositionIs(device, node, &pos, &errorCode);
	printError();
	return pos;
}

void printPositions() {
	cout << "left: " << getPosition(FL);
	cout << " right: " << getPosition(FR);
	cout << endl;
}

// Platform independent sleep wrapper
void sleep(int milliseconds) {
	struct timespec tim, tim2;
    tim.tv_sec = 1;
    tim.tv_nsec = milliseconds * 1000;
    nanosleep(&tim , &tim2);
}

void profilePositionMode(unsigned int node) {
	cout << "Switching to profile position mode at node ";
	cout << node << "..." << endl;
	VCS_ActivateProfilePositionMode(device, node, &errorCode);
	printError();
}

void moveRelative(unsigned int node, long pos) {
	VCS_MoveToPosition(device, node, pos, false, false, &errorCode);
	printError();
}

void moveAbsolute(unsigned int node, long pos) {
	VCS_MoveToPosition(device, node, pos, true, false, &errorCode);
	printError();
}

void interpolationMode(unsigned int node) {
	cout << "Switching to IPM mode at node ";
	cout << node << "..." << endl;
	VCS_ActivateInterpolatedPositionMode(device, node, &errorCode);
	printError();
	VCS_ClearIpmBuffer(device, node, &errorCode);
	printError();
}

void addPVT(unsigned int node, long p, long v, unsigned int t) {
	VCS_AddPvtValueToIpmBuffer(device, node, p, v, t, &errorCode);
	printError();
}

void startIPM(unsigned int node) {
	cout << "Starting IPM trajectory at node ";
	cout << node << "..." << endl;
	VCS_StartIpmTrajectory(device, node, &errorCode);
	printError();
}

void stopIPM(unsigned int node) {
	cout << "Stopping IPM trajectory at node ";
	cout << node << "..." << endl;
	VCS_StopIpmTrajectory(device, node, &errorCode);
	printError();
}

long getBufferSize(unsigned int node) {
	unsigned long bufferSize;
	VCS_GetFreeIpmBufferSize(device, node, &bufferSize, &errorCode);
	printError();
	return bufferSize;
}

void homingMode(unsigned int node) {
	cout << "Switching to homing position mode at node ";
	cout << node << "..." << endl;
	VCS_ActivateHomingMode(device, node, &errorCode);
	printError();
}

void zeroPosition(unsigned int node) {
	cout << "Setting current position to zero at node ";
	cout << node << "..." << endl;
	VCS_DefinePosition(device, node, 0, &errorCode);
	printError();
}

void zeroPositionAll() {
	homingMode(FL);
	homingMode(FR);
	zeroPosition(FL);
	zeroPosition(FR);
	cout << endl;
}

void moveToZeroAll() {
	profilePositionMode(FL);
	profilePositionMode(FR);
	cout << "Moving to zero position..." << endl;
	moveAbsolute(FL, 0);
	moveAbsolute(FR, 0);
}

// walks forward for tTotal time
void walkStraight(double tTotal) {
	walk(tTotal, 0);
}

void walk(double tTotal, double turnAngle) {

	double thetaGL = baseThetaG - turnAngle;
	double thetaGR = baseThetaG + turnAngle;

	profilePositionMode(FL);
	profilePositionMode(FR);
	cout << "Moving to start position..." << endl;
	moveAbsolute(FL, getTheta(T/2, thetaGL));
	moveAbsolute(FR, getTheta(0,   thetaGR));
	wait();

	interpolationMode(FL);
	interpolationMode(FR);
	cout << endl;

	cout << "Adding buffer points..." << endl;
	for (int i = 0; i < 10; i++) {
		addPVT(FL,  getTheta(T/2, thetaGL), 0, dt);
		addPVT(FR,  getTheta(0,   thetaGR), 0, dt);
	}

	startIPM(FL);
	startIPM(FR);

	double t = 0;
	while (t < tTotal) {
		if ((getBufferSize(FL) > 0) && (getBufferSize(FR) > 0)) {
			addPVT(FL, +getTheta(t+T/2, thetaGL), +getThetaDot(t+T/2, thetaGL), dt);
			addPVT(FR, -getTheta(t,     thetaGR), -getThetaDot(t,     thetaGR), dt);
			t += (double)dt/1000.0;
		}
		//if (GetKeyState(VK_ESCAPE) < -10) break;
	}
	sleep(dt * 2);
	addPVT(FL, +getTheta(t+T/2, thetaGL), 0, 0);
	addPVT(FR, +getTheta(t,     thetaGR), 0, 0);

	//stopIPM(FL);
	//stopIPM(FR);
}

void setPositionProfile(unsigned int node, long vel, long accel, long deaccel) {
	cout << "Setting position profile for node " << node << endl;
	VCS_SetPositionProfile(device, node, vel, accel, deaccel, &errorCode);
	printError();
}

int isFinished(unsigned int node) {
	int targetReached;
	VCS_GetMovementState(device, node, &targetReached, &errorCode);
	printError();
	return targetReached;
}

void wait() {
	while (!isFinished(FL) || !isFinished(FR)) {
		cout << ".";
		sleep(10);
	}
	cout << endl;
}

void stand() {
	moveRelative(FL, +REV/4 + 500);
	moveRelative(FR, -REV/4 + 500);
}
