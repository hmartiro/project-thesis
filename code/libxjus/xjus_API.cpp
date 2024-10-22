#include <iostream>
#include <stdio.h>
#include <string.h>
#include <ctime>
#include <sys/time.h>

using namespace std;

#include "Definitions.h"
#include "xjus_API.hpp"

// Protocol information
char deviceName[] = "EPOS2";
char protocolStackName[] = "EPOS2_USB";
char interfaceName[] = "USB";
char portName1[] = "USB0";
char portName2[] = "USB1";

// Protocol stack settings
const unsigned long BAUDRATE = 1000000;
const unsigned long TIMEOUT  = 500;

// Variables that hold error states
unsigned long errorCode;
const unsigned long ERROR_INFO_LENGTH = 100;
char errorInfo[ERROR_INFO_LENGTH];

// Handle of the connected devices
void* device1;
void* device2;

void* device(unsigned int node) {

	//printf("Getting device ID for node %d", node);
	if ((node == 1) || (node == 2) || (node == 3) || (node == 4) || (node == 5) || (node == 6)) {
		//printf(", returning device 1\n");
		return device1;
	} else if (false) {
		//printf(", returning device 2\n");
		return device2;
	} else {
		printf(", no device matched!\n");
		return 0;
	}
}

void printError() {
	VCS_GetErrorInfo(errorCode, errorInfo, ERROR_INFO_LENGTH);
	if (errorCode)
		printf("ERROR: x%lx, Message: %s\n", errorCode, errorInfo);
}

unsigned long getErrorCode() {
	return errorCode;
}

int openDevices() {
	printf("Attempting connection to %s \n", deviceName);

	device1 = VCS_OpenDevice(deviceName, protocolStackName, interfaceName, portName1, &errorCode);
	printError();
	printf("Connected Device 1: %p\n", device1);
	VCS_SetProtocolStackSettings(device1, BAUDRATE, TIMEOUT, &errorCode);

	/*
	device2 = VCS_OpenDevice(deviceName, protocolStackName, interfaceName, portName2, &errorCode);
	printError();
	printf("Connected Device 2: %p\n", device2);
	VCS_SetProtocolStackSettings(device2, BAUDRATE, TIMEOUT, &errorCode);

	clearFault(1);
	clearFault(2);
	clearFault(3);
	clearFault(4);
	clearFault(5);
	clearFault(6);
	*/

	//if ((device1) && (device2)) return 1;
	if (device1) return 1;
	else return -1;
}

void closeDevices() {
	printf("Closing connection to device.\n");
	VCS_CloseDevice(device1, &errorCode);
	printError();
	//VCS_CloseDevice(device2, &errorCode);
	//printError();
}

unsigned short getState(unsigned short node) {

	//clock_t t0, t1;
	//t0 = clock();
	unsigned short state;
	VCS_GetState(device(node), node, &state, &errorCode);
	//t1 = clock();
	//printf("getState() call: %f \n", (double(t1 - t0) / CLOCKS_PER_SEC));
	return state;
}

void clearFault(unsigned short node) {
	printf("Clearing fault at node %d...\n", node);
	VCS_ClearFault(device(node), node, &errorCode);
	printError();
}

void enable(unsigned short node) {
	//printf("Enabling node %d...\n", node);
	VCS_SetEnableState(device(node), node, &errorCode);
	printError();
}

void disable(unsigned short node) {
	//printf("Disabling node %d...\n", node);
	VCS_SetDisableState(device(node), node, &errorCode);
	printError();
}


long getPosition(unsigned short node) {
	long pos;
	VCS_GetPositionIs(device(node), node, &pos, &errorCode);
	return pos;
}

long getVelocity(unsigned short node) {
	long vel;
	VCS_GetVelocityIs(device(node), node, &vel, &errorCode);
	return vel;
}

unsigned long getMaxFollowingError(unsigned short node) {
	unsigned long followingError;
	VCS_GetMaxFollowingError(device(node), node, &followingError, &errorCode);
	return followingError;
}

void setMaxFollowingError(unsigned short node, unsigned long followingError) {
	VCS_SetMaxFollowingError(device(node), node, followingError, &errorCode);
}

unsigned long getMaxVelocity(unsigned int node) {
	unsigned long velocity;
	VCS_GetMaxProfileVelocity(device(node), node, &velocity, &errorCode);
	return velocity;
}

void setMaxVelocity(unsigned int node, unsigned long velocity) {
	VCS_SetMaxProfileVelocity(device(node), node, velocity, &errorCode);
}

unsigned long getMaxAcceleration(unsigned int node) {
	unsigned long acceleration;
	VCS_GetMaxAcceleration(device(node), node, &acceleration, &errorCode);
	return acceleration;
}

void setMaxAcceleration(unsigned int node, unsigned long acceleration) {
	VCS_SetMaxAcceleration(device(node), node, acceleration, &errorCode);
}

void profilePositionMode(unsigned short node) {
	//printf("Switching to profile position mode at node %d \n", node);
	VCS_ActivateProfilePositionMode(device(node), node, &errorCode);
	printError();
}

void moveRelative(unsigned short node, long pos) {
	profilePositionMode(node);
	VCS_MoveToPosition(device(node), node, pos, false, false, &errorCode);
	printError();
}

void moveAbsolute(unsigned short node, long pos) {
	profilePositionMode(node);
	VCS_MoveToPosition(device(node), node, pos, true, false, &errorCode);
	printError();
}

void interpolationMode(unsigned short node) {
	//printf("Switching to IPM mode at node %d \n", node);
	VCS_ActivateInterpolatedPositionMode(device(node), node, &errorCode);
	printError();
	VCS_ClearIpmBuffer(device(node), node, &errorCode);
	printError();
}

void addPVT(unsigned short node, long p, long v, unsigned int t) {
	VCS_AddPvtValueToIpmBuffer(device(node), node, p, v, t, &errorCode);
	printError();
}

void addPvtFrame(int pvt[6][4]) {

	timeval start,stop,result;
	gettimeofday(&start, NULL);

	for(int i = 0; i < 6; i++) {
		unsigned short node = (unsigned short)pvt[i][0];
		long p = (long)pvt[i][1];
		long v = (long)pvt[i][2];
		unsigned char t = (unsigned char)pvt[i][3];

		//printf("node: %u, p: %ld, v: %ld, t: %u \n", node, p, v, t);
		VCS_AddPvtValueToIpmBuffer(device(node), node, p, v, t, &errorCode);
	}
	printError();

	gettimeofday(&stop, NULL);
	timersub(&start,&stop,&result);
	printf("Add PVT time in C code: %fs\n", -(result.tv_sec + result.tv_usec/1000000.0));
}

void startIPM(unsigned short node) {
	//printf("Starting IPM trajectory at node %d \n", node);
	VCS_StartIpmTrajectory(device(node), node, &errorCode);
	printError();
}

void stopIPM(unsigned short node) {
	//printf("Stopping IPM trajectory at node %d \n", node);
	VCS_StopIpmTrajectory(device(node), node, &errorCode);
	printError();
}

long getTargetPosition(unsigned short node) {
	long targetPosition;
	VCS_GetTargetPosition(device(node), node, &targetPosition, &errorCode);
	return targetPosition;
}

long getFreeBufferSize(unsigned short node) {
	unsigned long bufferSize;
	VCS_GetFreeIpmBufferSize(device(node), node, &bufferSize, &errorCode);
	//printError();
	return bufferSize;
}

void clearIpmBuffer(unsigned short node) {
	interpolationMode(node);
	VCS_ClearIpmBuffer(device(node), node, &errorCode);
	printError();
}

void homingMode(unsigned short node) {
	//printf("Switching to homing position mode at node %d \n", node);
	VCS_ActivateHomingMode(device(node), node, &errorCode);
	printError();
}

void zeroPosition(unsigned short node) {
	//printf("Setting current position to zero at node %d \n", node);
	VCS_DefinePosition(device(node), node, 0, &errorCode);
	printError();
}

void setPositionProfile(unsigned short node, long vel, long accel, long deaccel) {
	//printf("Setting position profile for node %d \n", node);
	VCS_SetPositionProfile(device(node), node, vel, accel, deaccel, &errorCode);
	printError();
}

unsigned short getPositionRegulatorGain(unsigned short node, int index) {
	unsigned short pP, pI, pD;
	VCS_GetPositionRegulatorGain(device(node), node, &pP, &pI, &pD, &errorCode);
	if (index == 1) return pP;
	else if (index == 2) return pI;
	else if (index == 3) return pD;
	else {
		printf("Invalid index to get regulator gain!\n");
		return -1;
	}
}

void printPositionRegulatorGain(unsigned short node) {
	unsigned short pP, pI, pD;
	VCS_GetPositionRegulatorGain(device(node), node, &pP, &pI, &pD, &errorCode);
    printf("node: %d, pP: %d, pI: %d, pD: %d \n", node, pP, pI, pD);
}

void setPositionRegulatorGain(unsigned short node, unsigned short P, unsigned short I, unsigned short D) {
	VCS_SetPositionRegulatorGain(device(node), node, P, I, D, &errorCode);
	printError();
}

unsigned short getPositionRegulatorFeedForward(unsigned short node, int index) {
	unsigned short velocityFeedForward, accelerationFeedForward;
	VCS_GetPositionRegulatorFeedForward(device(node), node, &velocityFeedForward, &accelerationFeedForward, &errorCode);
	if (index == 1) return velocityFeedForward;
	else if (index == 2) return accelerationFeedForward;
	else {
		printf("Invalid index to get regulator feed forward!");
		return -1;
	}
}

void setPositionRegulatorFeedForward(unsigned short node,unsigned short velocityFeedForward, unsigned short accelerationFeedForward) {
	printf("velocity: %u, acceleration: %u \n", velocityFeedForward, accelerationFeedForward);
	//VCS_SetPositionRegulatorFeedForward(device(node), node, velocityFeedForward, accelerationFeedForward, &errorCode);

	unsigned long bytesWritten;
	unsigned long bytesToWrite = 2;

	unsigned short objectIndex = 0x60FB;
	unsigned char velocitySubIndex = 4;
	unsigned char accelerationSubIndex = 5;

	VCS_SetObject(device(node), node, objectIndex, velocitySubIndex, &velocityFeedForward, bytesToWrite, &bytesWritten, &errorCode);
	VCS_SetObject(device(node), node, objectIndex, accelerationSubIndex, &accelerationFeedForward, bytesToWrite, &bytesWritten, &errorCode);

	printError();
}


signed short getNodeAvgCurrent(unsigned short node){
	signed short nodeCurrent;   // INT16

	// Average current 0x2027 = 8231
	unsigned short currentObject = 8231; //WORD, uINT16
	unsigned long pNbOfBytesRead = 2;    //DWORD, uINT32

	VCS_GetObject(device(node), node, currentObject, 0, &nodeCurrent, 2, &pNbOfBytesRead, &errorCode );

	// Take the absolute value of the current, ignoring regenerative effects
	if (nodeCurrent < 0)
		nodeCurrent = -nodeCurrent;

	//printf("For node %d, got current of %d\n", node, nodeCurrent);

	return nodeCurrent;
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

	VCS_GetIpmStatus(device(node), node, &trajectoryRunning, &underflowWarning, &overflowWarning,
			&velocityWarning, &accelerationWarning, &underflowError, &overflowError,
			&velocityError, &accelerationError, &errorCode);

	printf("IPM Status, node %d:\n", node);
	if (underflowWarning)    printf("Underflow Warning!\n");
	if (overflowWarning )    printf("Overflow Warning!\n");
	if (velocityWarning )    printf("Velocity Warning!\n");
	if (accelerationWarning) printf("Acceleration Warning!\n");
	if (underflowError)      printf("Underflow Error!\n");
	if (overflowError )      printf("Overflow Error!\n");
	if (velocityError )      printf("Velocity Error!\n");
	if (accelerationError)   printf("Acceleration Error!\n");
	printf("Remaining buffer size: %ld \n\n", getFreeBufferSize(node));
}

int isFinished(unsigned short node) {
	int targetReached;
	VCS_GetMovementState(device(node), node, &targetReached, &errorCode);
	printError();
	return targetReached;
}
