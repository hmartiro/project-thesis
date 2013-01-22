//============================================================================
// Name        : xJus-epos.cpp
// Author      : Hayk Martirosyan
// Description : Usage of the EPOS2 API over C++
//============================================================================

#include "xJus-epos.h"
#include "Definitions.h"
#include <iostream>
using namespace std;

// Protocol information
char deviceName[] = "EPOS2";
char protocolStackName[] = "EPOS2_USB"; // EPOS2_USB // MAxon SERIAL V2
char interfaceName[] = "USB";
char portName[] = "USB3";

// Protocol stack settings
const unsigned long BAUDRATE = 1000000;
const unsigned long TIMEOUT  = 500;

// Variables that hold error states
unsigned long errorCode;
const unsigned short ERROR_INFO_LENGTH = 100;
char errorInfo[ERROR_INFO_LENGTH];

// Handle of the connected device
void* device;

int main() {

	const int maxStrSize = 100;

	char deviceNameSelection[maxStrSize];
	int endOfSel;
	//get first protocol stack name
	if(VCS_GetDeviceNameSelection(1,
	deviceNameSelection, maxStrSize, &endOfSel, &errorCode))
	{
		cout << deviceNameSelection << endl;
		while(!endOfSel)
		{
			VCS_GetDeviceNameSelection (0,
			deviceNameSelection, maxStrSize, &endOfSel, &errorCode);
			cout << deviceNameSelection << endl;
		}
	}

	char strProtocolStackName[maxStrSize];
	if(VCS_GetProtocolStackNameSelection(deviceName, 1,
	strProtocolStackName, maxStrSize, &endOfSel, &errorCode))
	{
		cout << strProtocolStackName << endl;
		while(!endOfSel)
		{
			VCS_GetProtocolStackNameSelection (deviceName, 0,
			strProtocolStackName, maxStrSize, &endOfSel, &errorCode);
			cout << strProtocolStackName << endl;
		}
	}

	char interfaceNameSelection[maxStrSize];
	if(VCS_GetInterfaceNameSelection(deviceName, protocolStackName, 1,
	interfaceNameSelection, maxStrSize, &endOfSel, &errorCode))
	{
		cout << interfaceNameSelection << endl;
		while(!endOfSel)
		{
			VCS_GetInterfaceNameSelection (deviceName, protocolStackName, 0,
			interfaceNameSelection, maxStrSize, &endOfSel, &errorCode);
			cout << interfaceNameSelection << endl;
		}
	}

	char portNameSelection[maxStrSize];
	if(VCS_GetPortNameSelection(deviceName, protocolStackName, interfaceName, 1,
	portNameSelection, maxStrSize, &endOfSel, &errorCode))
	{
		//cout << portNameSelection << endl;
		while(!endOfSel)
		{
			VCS_GetPortNameSelection (deviceName, protocolStackName, interfaceName, 0,
			portNameSelection, maxStrSize, &endOfSel, &errorCode);
			//cout << portNameSelection << endl;
		}
	}

	cout << "Attempting connection to " << deviceName;
	cout << " at " << portName << "..." << endl;
	device = VCS_OpenDevice(deviceName, protocolStackName, interfaceName, portName, &errorCode);
	printError();
	cout << "Connected Device" << ": " << device << endl;

	//cout << "Attempting to set protocol stack settings with ";
	//cout << "baudrate " << BAUDRATE << " and timeout " << TIMEOUT << "..." << endl;
	//VCS_SetProtocolStackSettings(device, BAUDRATE, TIMEOUT, &errorCode);
	//printError();

	//cout << "Closing connection to device..." << endl;
	//VCS_CloseDevice(device, &errorCode);
	//printError();

	return 0;
}

void printError() {
	VCS_GetErrorInfo(errorCode, errorInfo, ERROR_INFO_LENGTH);
	cout << "Error info: " << errorInfo << endl;
}
