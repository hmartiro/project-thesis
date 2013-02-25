#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
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
const unsigned long ERROR_INFO_LENGTH = 100;
char errorInfo[ERROR_INFO_LENGTH];

// Handle of the connected device
void* device;

// Node IDs
unsigned short FL = 1;
unsigned short FR = 2;

const int NODE_COUNT = 2;
const int REV = 59720;

int dt = 50;
double T = 0.5;

double baseThetaG = 50.0 * (M_PI/180.0);
double standAngle = 110.0 * (M_PI/180.0);

long offsetPos = radToPos(baseThetaG/2);

// Positions
//long position[NODE_COUNT];

int main() {

	openDevice();
	clearAllFaults();
	enableAll();

	profilePositionMode(FL);
	profilePositionMode(FR);

	cout << "Press enter to stand...\n";
	cin.ignore();
	//sit();
	//wait();
	stand();
	wait();

	zeroPositionAll();

	setPositionProfile(FL, 1000, 5000, 5000);
	setPositionProfile(FR, 1000, 5000, 5000);

	cout << "Press enter to walk...\n";
	cin.ignore();
	walk(T*10);
	wait();

	//cout << "Press enter to return to stand...\n";
	//cin.ignore();
	returnToStand();
	wait();

	cout << "Press enter to sit...\n";
	cin.ignore();
	sit();
	wait();



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

void clearFault(unsigned short node) {
	cout << "Clearing fault at node " << node << "..." << endl;
	VCS_ClearFault(device, node, &errorCode);
	printError();
}

void clearAllFaults() {
	clearFault(FL);
	clearFault(FR);
	cout << endl;
}

void enable(unsigned short node) {
	cout << "Enabling node " << node << "..." << endl;
	VCS_SetEnableState(device, node, &errorCode);
	printError();
}

void enableAll() {
	enable(FL);
	enable(FR);
	cout << endl;
}

void disable(unsigned short node) {
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

long getPosition(unsigned short node) {
	long pos;
	VCS_GetPositionIs(device, node, &pos, &errorCode);
	printError();
	return pos;
}

double getAngle(unsigned short node) {
	long pos = abs(getPosition(node));
	return ((pos%REV)/(double)REV) * 360;
}

void printPositions() {
	cout << "left: " << getPosition(FL);
	cout << " right: " << getPosition(FR);
	cout << endl;
}

void printAngles() {
	cout << "left: " << getAngle(FL);
	cout << " right: " << getAngle(FR);
	cout << endl;
}

// Platform independent sleep wrapper
void sleep(int milliseconds) {
	struct timespec tim, tim2;
    tim.tv_sec = 1;
    tim.tv_nsec = milliseconds * 1000;
    nanosleep(&tim , &tim2);
}

void profilePositionMode(unsigned short node) {
	cout << "Switching to profile position mode at node ";
	cout << node << "..." << endl;
	VCS_ActivateProfilePositionMode(device, node, &errorCode);
	printError();
}

void moveRelative(unsigned short node, long pos) {
	VCS_MoveToPosition(device, node, pos, false, false, &errorCode);
	printError();
}

void moveAbsolute(unsigned short node, long pos) {
	VCS_MoveToPosition(device, node, pos, true, false, &errorCode);
	printError();
}

void interpolationMode(unsigned short node) {
	cout << "Switching to IPM mode at node ";
	cout << node << "..." << endl;
	VCS_ActivateInterpolatedPositionMode(device, node, &errorCode);
	printError();
	VCS_ClearIpmBuffer(device, node, &errorCode);
	printError();
}

void addPVT(unsigned short node, long p, long v, unsigned int t) {
	VCS_AddPvtValueToIpmBuffer(device, node, p, v, t, &errorCode);
	printError();
}

void startIPM(unsigned short node) {
	cout << "Starting IPM trajectory at node ";
	cout << node << "..." << endl;
	VCS_StartIpmTrajectory(device, node, &errorCode);
	printError();
}

void stopIPM(unsigned short node) {
	cout << "Stopping IPM trajectory at node ";
	cout << node << "..." << endl;
	VCS_StopIpmTrajectory(device, node, &errorCode);
	printError();
}

long getBufferSize(unsigned short node) {
	unsigned long bufferSize;
	VCS_GetFreeIpmBufferSize(device, node, &bufferSize, &errorCode);
	printError();
	return bufferSize;
}

void homingMode(unsigned short node) {
	cout << "Switching to homing position mode at node ";
	cout << node << "..." << endl;
	VCS_ActivateHomingMode(device, node, &errorCode);
	printError();
}

void zeroPosition(unsigned short node) {
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
void walk(double tTotal) {
	walk(tTotal, 0);
}

void walk(double tTotal, double turnAngle) {

	double thetaGL = baseThetaG - turnAngle;
	double thetaGR = baseThetaG + turnAngle;

	profilePositionMode(FL);
	profilePositionMode(FR);
	cout << "Moving to start position..." << endl;
	moveAbsolute(FL, +getTheta(T/2, thetaGL)-offsetPos);
	moveAbsolute(FR, -getTheta(0,   thetaGR)+offsetPos);
	wait();
	//cout << "Enter to start IPM..." << endl;
	//cin.ignore();
	interpolationMode(FL);
	interpolationMode(FR);
	cout << endl;

	cout << "Filling buffer..." << endl;
	double t = (double)dt/2000.0;
	for (int i = 0; i < 10; i++) {
		addTrajectoryPoint(t, turnAngle);
		t += (double)dt/1000.0;

	}

	startIPM(FL);
	startIPM(FR);

	while (t < tTotal) {
		if ((getBufferSize(FL) > 0) && (getBufferSize(FR) > 0)) {
			addTrajectoryPoint(t, turnAngle);
			t += (double)dt/1000.0;
		}
	}

	sleep(dt * 2);
	addPVT(FL, +getTheta(t+T/2, thetaGL)-offsetPos, 0, 0);
	addPVT(FR, -getTheta(t,     thetaGR)+offsetPos, 0, 0);
	printIpmStatus(FL);
	printIpmStatus(FR);

	wait();
	stopIPM(FL);
	stopIPM(FR);
}

void addTrajectoryPoint(double t, double turnAngle) {

	double thetaGL = baseThetaG - turnAngle;
	double thetaGR = baseThetaG + turnAngle;

	addPVT(FL, +getTheta(t+T/2, thetaGL)-offsetPos, +getThetaDot(t+T/2, thetaGL), dt);
	addPVT(FR, -getTheta(t,     thetaGR)+offsetPos, -getThetaDot(t,     thetaGR), dt);
}

void setPositionProfile(unsigned short node, long vel, long accel, long deaccel) {
	cout << "Setting position profile for node " << node << endl;
	VCS_SetPositionProfile(device, node, vel, accel, deaccel, &errorCode);
	printError();
}

void printIpmStatus(unsigned short node) {

	int trajectoryRunning;
	int underflowWarning;
	int overflowWarning;
	int velocityWarning;
	int accelerationWarning;
	int underflowError;
	int overflowError;
	int velocityError;
	int accelerationError;

	VCS_GetIpmStatus(device, node, &trajectoryRunning, &underflowWarning, &overflowWarning,
			&velocityWarning, &accelerationWarning, &underflowError, &overflowError,
			&velocityError, &accelerationError, &errorCode);

	cout << "IPM Status, node " << node << ":" << endl;
	if (underflowWarning)    cout << "Underflow Warning!\n";
	if (overflowWarning )    cout << "Overflow Warning!\n";
	if (velocityWarning )    cout << "Velocity Warning!\n";
	if (accelerationWarning) cout << "Acceleration Warning!\n";
	if (underflowError)      cout << "Underflow Error!\n";
	if (overflowError )      cout << "Overflow Error!\n";
	if (velocityError )      cout << "Velocity Error!\n";
	if (accelerationError)   cout << "Acceleration Error!\n";
	cout << "Remaining buffer size: " << getBufferSize(node) << endl << endl;
}

int isFinished(unsigned short node) {
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

long radToPos(double rad) {
	return round2(REV*(rad/(2*M_PI)));
}

long degToPos(double deg) {
	return round2(REV*(deg/(360)));
}

void stand() {
	setPositionProfile(FL, 500, 5000, 5000);
	setPositionProfile(FR, 500, 5000, 5000);
	moveRelative(FL, +radToPos(standAngle));
	moveRelative(FR, -radToPos(standAngle));
}

void sit() {
	setPositionProfile(FL, 500, 5000, 5000);
	setPositionProfile(FR, 500, 5000, 5000);
	moveRelative(FL, -radToPos(standAngle));
	moveRelative(FR, +radToPos(standAngle));
}

void returnToStand() {

	int posL = getPosition(FL);
	int posR = getPosition(FR);

	profilePositionMode(FL);
	profilePositionMode(FR);

	//printPositions();
	//cout << "FL " << REV-posL%REV << " FR " << -REV-posR%REV << endl;

	moveRelative(FR, -REV-(posR%REV));
	wait();
	//printPositions();
	moveRelative(FL, REV-(posL%REV));
	wait();
	//printPositions();
}
