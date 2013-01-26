// xJus_EPOS.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Definitions.h"
#include "xJus_EPOS.h"
#include <iostream>
using namespace std;

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

void printError() {
	VCS_GetErrorInfo(errorCode, errorInfo, ERROR_INFO_LENGTH);
	cout << "Error info: " << errorInfo << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "Attempting connection to " << deviceName;
	cout << " at " << portName << "..." << endl;
	device = VCS_OpenDevice(deviceName, protocolStackName, interfaceName, portName, &errorCode);
	//device = VCS_OpenDeviceDlg(&errorCode);
	printError();
	cout << "Connected Device" << ": " << device << endl;

	cout << "Attempting to set protocol stack settings with ";
	cout << "baudrate " << BAUDRATE << " and timeout " << TIMEOUT << "..." << endl;
	VCS_SetProtocolStackSettings(device, BAUDRATE, TIMEOUT, &errorCode);
	printError();

	//cout << "Closing connection to device..." << endl;
	//VCS_CloseDevice(device, &errorCode);
	//printError();

	cout << "hey" << endl;
	return 0;
}
