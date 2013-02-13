#ifndef XJUS_EPOS_H_
#define XJUS_EPOS_H_

// Initialization
void openDevice();
void closeDevice();
void clearFault(unsigned short node);
void clearAllFaults();

// Enable / disable
void enable(unsigned short node);
void disable(unsigned short node);
void enableAll();
void disableAll();

// Profile position mode
void profilePositionMode(unsigned short node);
void setPositionProfile(unsigned short node, long vel, long accel, long deaccel);
void moveRelative(unsigned short node, long pos);
void moveAbsolute(unsigned short node, long pos);
void moveToZeroAll();

// Interpolated position mode
void interpolationMode(unsigned short node);
long getBufferSize(unsigned short node);
void addPVT(unsigned short node, long p, long v, unsigned int t);
void addTrajectoryPoint(double t, double turnAngle);
void startIPM(unsigned short node);
void stopIPM(unsigned short node);
void printIpmStatus(unsigned short node);


// Homing mode
void homingMode(unsigned short node);
void zeroPosition(unsigned short node);
void zeroPositionAll();

// Motion info
long getPosition(unsigned short node);
void printPositions();
double getAngle(unsigned short node);
void printAngles();
int isFinished(unsigned short node);

// Utility functions
void sleep(int milliseconds);
void printError();
void wait();
long degToPos(double deg);
long radToPos(double rad);

// High level locomotion routines
void walk(double tTotal);
void walk(double tTotal, double turnAngle);
void stand();
void returnToStand();
void sit();

#endif /* XJUS_EPOS_H_ */
