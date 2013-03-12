#ifndef XJUS_EPOS_H_
#define XJUS_EPOS_H_

#define PYTHON_EXPORT extern "C"

// Initialization
PYTHON_EXPORT void openDevice();
PYTHON_EXPORT void closeDevice();
PYTHON_EXPORT void clearFault(unsigned short node);

// Enable / disable
PYTHON_EXPORT void enable(unsigned short node);
PYTHON_EXPORT void disable(unsigned short node);

// Profile position mode
PYTHON_EXPORT void profilePositionMode(unsigned short node);
PYTHON_EXPORT void setPositionProfile(unsigned short node, long vel, long accel, long deaccel);
PYTHON_EXPORT void moveRelative(unsigned short node, long pos);
PYTHON_EXPORT void moveAbsolute(unsigned short node, long pos);

// Interpolated position mode
PYTHON_EXPORT void interpolationMode(unsigned short node);
PYTHON_EXPORT long getBufferSize(unsigned short node);
PYTHON_EXPORT void addPVT(unsigned short node, long p, long v, unsigned int t);
PYTHON_EXPORT void startIPM(unsigned short node);
PYTHON_EXPORT void stopIPM(unsigned short node);
PYTHON_EXPORT void printIpmStatus(unsigned short node);


// Homing mode
PYTHON_EXPORT void homingMode(unsigned short node);
PYTHON_EXPORT void zeroPosition(unsigned short node);

// Motion info
PYTHON_EXPORT long getPosition(unsigned short node);
PYTHON_EXPORT int isFinished(unsigned short node);

// Utility functions
PYTHON_EXPORT void printError();

#endif /* XJUS_EPOS_H_ */