#ifndef XJUS_EPOS_H_
#define XJUS_EPOS_H_

#define PYTHON_EXPORT extern "C"

// Initialization
PYTHON_EXPORT int  openDevices();
PYTHON_EXPORT void closeDevices();
PYTHON_EXPORT void clearFault(unsigned short node);
PYTHON_EXPORT unsigned long getErrorCode();
PYTHON_EXPORT unsigned short getState(unsigned short node);

PYTHON_EXPORT unsigned short getPositionRegulatorGain(unsigned short node, int index);
PYTHON_EXPORT void printPositionRegulatorGain(unsigned short node);
PYTHON_EXPORT void setPositionRegulatorGain(unsigned short node, unsigned short P, unsigned short I, unsigned short D);
PYTHON_EXPORT unsigned short getPositionRegulatorFeedForward(unsigned short node, int index);
PYTHON_EXPORT void setPositionRegulatorFeedForward(unsigned short node,unsigned short velocityFeedForward, unsigned short accelerationFeedForward);

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
PYTHON_EXPORT long getFreeBufferSize(unsigned short node);
PYTHON_EXPORT void addPVT(unsigned short node, long p, long v, unsigned int t);
PYTHON_EXPORT void addPvtAll(int N, unsigned short node[], long p[], long v[], unsigned char t[]);
PYTHON_EXPORT void startIPM(unsigned short node);
PYTHON_EXPORT void stopIPM(unsigned short node);
PYTHON_EXPORT void printIpmStatus(unsigned short node);
PYTHON_EXPORT void clearIpmBuffer(unsigned short node);

PYTHON_EXPORT void setMaxFollowingError(unsigned short node, unsigned long followingError);
PYTHON_EXPORT void setMaxVelocity(unsigned int node, unsigned long velocity);
PYTHON_EXPORT void setMaxAcceleration(unsigned int node, unsigned long acceleration);
PYTHON_EXPORT unsigned long getMaxFollowingError(unsigned short node);
PYTHON_EXPORT unsigned long getMaxVelocity(unsigned int node);
PYTHON_EXPORT unsigned long getMaxAcceleration(unsigned int node);

// Homing mode
PYTHON_EXPORT void homingMode(unsigned short node);
PYTHON_EXPORT void zeroPosition(unsigned short node);

// Motion info
PYTHON_EXPORT long getPosition(unsigned short node);
PYTHON_EXPORT int isFinished(unsigned short node);

// Power info
PYTHON_EXPORT signed short getNodeAvgCurrent(unsigned short node);

// Utility functions
PYTHON_EXPORT void printError();

#endif /* XJUS_EPOS_H_ */
