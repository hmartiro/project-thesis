#ifndef XJUS_EPOS_H_
#define XJUS_EPOS_H_

// Initialization
int  openDevices();
void closeDevices();
void clearFault(unsigned short node);
unsigned long getErrorCode();
unsigned short getState(unsigned short node);

unsigned short getPositionRegulatorGain(unsigned short node, int index);
void printPositionRegulatorGain(unsigned short node);
void setPositionRegulatorGain(unsigned short node, unsigned short P, unsigned short I, unsigned short D);
unsigned short getPositionRegulatorFeedForward(unsigned short node, int index);
void setPositionRegulatorFeedForward(unsigned short node,unsigned short velocityFeedForward, unsigned short accelerationFeedForward);

// Enable / disable
void enable(unsigned short node);
void disable(unsigned short node);

// Profile position mode
void profilePositionMode(unsigned short node);
void setPositionProfile(unsigned short node, long vel, long accel, long deaccel);
void moveRelative(unsigned short node, long pos);
void moveAbsolute(unsigned short node, long pos);
long getTargetPosition(unsigned short node);

long getVelocity(unsigned short node);

// Interpolated position mode
void interpolationMode(unsigned short node);
long getFreeBufferSize(unsigned short node);
void addPVT(unsigned short node, long p, long v, unsigned int t);
void addPvtFrame(int pvt[6][4]);
void startIPM(unsigned short node);
void stopIPM(unsigned short node);
void printIpmStatus(unsigned short node);
void clearIpmBuffer(unsigned short node);

void setMaxFollowingError(unsigned short node, unsigned long followingError);
void setMaxVelocity(unsigned int node, unsigned long velocity);
void setMaxAcceleration(unsigned int node, unsigned long acceleration);
unsigned long getMaxFollowingError(unsigned short node);
unsigned long getMaxVelocity(unsigned int node);
unsigned long getMaxAcceleration(unsigned int node);

// Homing mode
void homingMode(unsigned short node);
void zeroPosition(unsigned short node);

// Motion info
long getPosition(unsigned short node);
int isFinished(unsigned short node);

// Power info
signed short getNodeAvgCurrent(unsigned short node);

// Utility functions
void printError();

#endif /* XJUS_EPOS_H_ */
