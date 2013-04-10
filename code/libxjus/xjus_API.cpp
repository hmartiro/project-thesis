#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#include "Definitions.h"
#include "xjus_API.hpp"

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

void printError() {
	VCS_GetErrorInfo(errorCode, errorInfo, ERROR_INFO_LENGTH);
	if (errorCode)
		printf("ERROR: %lx\n", errorCode);
}

unsigned long getErrorCode() {
	return errorCode;
}

int openDevice() {
	printf("Attempting connection to %s", deviceName);
	printf(" at %s...\n", portName);
	device = VCS_OpenDevice(deviceName, protocolStackName, interfaceName, portName, &errorCode);
	printError();
	printf("Connected Device: %p\n", device);

	//printf("Protocol stack settings are ");
	//printf("baudrate %lu and timeout %lu.\n", BAUDRATE, TIMEOUT);
	VCS_SetProtocolStackSettings(device, BAUDRATE, TIMEOUT, &errorCode);
	printError();

	if (device) return 1;
	else return -1;
}

void clearFault(unsigned short node) {
	//printf("Clearing fault at node %d...\n", node);
	VCS_ClearFault(device, node, &errorCode);
	printError();
}

void enable(unsigned short node) {
	//printf("Enabling node %d...\n", node);
	VCS_SetEnableState(device, node, &errorCode);
	printError();
}

void disable(unsigned short node) {
	//printf("Disabling node %d...\n", node);
	VCS_SetDisableState(device, node, &errorCode);
	printError();
}

void closeDevice() {
	//printf("Closing connection to device.\n");
	VCS_CloseDevice(device, &errorCode);
	printError();
}

long getPosition(unsigned short node) {
	long pos;
	VCS_GetPositionIs(device, node, &pos, &errorCode);
	printError();
	return pos;
}

unsigned long getMaxFollowingError(unsigned short node) {
	unsigned long followingError;
	VCS_GetMaxFollowingError(device, node, &followingError, &errorCode);
	return followingError;
}

void setMaxFollowingError(unsigned short node, unsigned long followingError) {
	VCS_SetMaxFollowingError(device, node, followingError, &errorCode);
}

unsigned long getMaxVelocity(unsigned int node) {
	unsigned long velocity;
	VCS_GetMaxProfileVelocity(device, node, &velocity, &errorCode);
	return velocity;
}

void setMaxVelocity(unsigned int node, unsigned long velocity) {
	VCS_SetMaxProfileVelocity(device, node, velocity, &errorCode);
}

unsigned long getMaxAcceleration(unsigned int node) {
	unsigned long acceleration;
	VCS_GetMaxAcceleration(device, node, &acceleration, &errorCode);
	return acceleration;
}

void setMaxAcceleration(unsigned int node, unsigned long acceleration) {
	VCS_SetMaxAcceleration(device, node, acceleration, &errorCode);
}

void profilePositionMode(unsigned short node) {
	//printf("Switching to profile position mode at node %d \n", node);
	VCS_ActivateProfilePositionMode(device, node, &errorCode);
	printError();
}

void moveRelative(unsigned short node, long pos) {
	profilePositionMode(node);
	VCS_MoveToPosition(device, node, pos, false, false, &errorCode);
	printError();
}

void moveAbsolute(unsigned short node, long pos) {
	profilePositionMode(node);
	VCS_MoveToPosition(device, node, pos, true, false, &errorCode);
	printError();
}

void interpolationMode(unsigned short node) {
	//printf("Switching to IPM mode at node %d \n", node);
	VCS_ActivateInterpolatedPositionMode(device, node, &errorCode);
	printError();
	VCS_ClearIpmBuffer(device, node, &errorCode);
	printError();
}

void addPVT(unsigned short node, long p, long v, unsigned int t) {
	VCS_AddPvtValueToIpmBuffer(device, node, p, v, t, &errorCode);
	printError();
}

void addPvtAll(int N, unsigned short node[], long p[], long v[], unsigned char t[]) {

	//int N = sizeof(p)/sizeof(p[0]);

	//printf("sizeofP = %d, sizeof(p[0]) = %d, N = %d \n", sizeof(p), sizeof(p[0]), N);

	for(int i = 0; i < N; i++) {
		//printf("node: %u, p: %ld, v: %ld, t: %u \n", node[i], p[i], v[i], t[i]);
		VCS_AddPvtValueToIpmBuffer(device, node[i], p[i], v[i], t[i], &errorCode);
	}
	printError();
}

void startIPM(unsigned short node) {
	//printf("Starting IPM trajectory at node %d \n", node);
	VCS_StartIpmTrajectory(device, node, &errorCode);
	printError();
}

void stopIPM(unsigned short node) {
	//printf("Stopping IPM trajectory at node %d \n", node);
	VCS_StopIpmTrajectory(device, node, &errorCode);
	printError();
}

long getFreeBufferSize(unsigned short node) {
	unsigned long bufferSize;
	VCS_GetFreeIpmBufferSize(device, node, &bufferSize, &errorCode);
	//printError();
	return bufferSize;
}

void clearIpmBuffer(unsigned short node) {
	interpolationMode(node);
	VCS_ClearIpmBuffer(device, node, &errorCode);
	printError();
}
void homingMode(unsigned short node) {
	//printf("Switching to homing position mode at node %d \n", node);
	VCS_ActivateHomingMode(device, node, &errorCode);
	printError();
}

void zeroPosition(unsigned short node) {
	//printf("Setting current position to zero at node %d \n", node);
	VCS_DefinePosition(device, node, 0, &errorCode);
	printError();
}

void setPositionProfile(unsigned short node, long vel, long accel, long deaccel) {
	//printf("Setting position profile for node %d \n", node);
	VCS_SetPositionProfile(device, node, vel, accel, deaccel, &errorCode);
	printError();
}

void getPositionRegulatorGain(unsigned short node) {
	unsigned short pP, pI, pD;
	VCS_GetPositionRegulatorGain(device, node, &pP, &pI, &pD, &errorCode);
	printf("node: %d, pP: %d, pI: %d, pD: %d \n", node, pP, pI, pD);
}

void getNodeCurrent(unsigned short node, signed short nodeCurrent){
	// signed short nodeCurrent;   // INT16
	// Average current 0x2027
	unsigned short currentObject = 8231;  //WORD, uINT16
	unsigned long pNbOfBytesRead = 2;   //DWORD, uINT32

	printf("**coming in %d \n", nodeCurrent);
	// Load current values into the array for each node
	//for (unsigned short n = 0; n < 6; n++){

	VCS_GetObject(device, node, currentObject, 0, &nodeCurrent, 2, &pNbOfBytesRead, &errorCode );

	printf("**coming out of VCS %d \n", nodeCurrent);

	nodeCurrent = 3;
	// printf("For node %d, got current of %d\n", node, nodeCurrent);

	printf("**return function %d \n", nodeCurrent);

	//}

	printError();
	//return nodeCurrents;
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
	VCS_GetMovementState(device, node, &targetReached, &errorCode);
	printError();
	return targetReached;
}
