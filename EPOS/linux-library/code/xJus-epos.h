/*
 * xJus-epos.h
 *
 *  Created on: Jan 18, 2013
 *      Author: hayk
 */

#ifndef XJUS_EPOS_H_
#define XJUS_EPOS_H_

// Initialization
void openDevice();
void closeDevice();
void clearFault(unsigned int node);
void clearAllFaults();

// Enable / disable
void enable(unsigned int node);
void disable(unsigned int node);
void enableAll();
void disableAll();

// Profile position mode
void profilePositionMode(unsigned int node);
void setPositionProfile(unsigned int node, long vel, long accel, long deaccel);
void moveRelative(unsigned int node, long pos);
void moveAbsolute(unsigned int node, long pos);
void moveToZeroAll();

// Interpolated position mode
void interpolationMode(unsigned int node);
long getBufferSize(unsigned int node);
void addPVT(unsigned int node, long p, long v, unsigned int t);
void startIPM(unsigned int node);
void stopIPM(unsigned int node);

// Homing mode
void homingMode(unsigned int node);
void zeroPosition(unsigned int node);
void zeroPositionAll();

// Motion info
long getPosition(unsigned int node);
void printPositions();
int isFinished(unsigned int node);

// Utility functions
void sleep(int milliseconds);
void printError();
void wait();

// High level locomotion routines
void walkStraight(double tTotal);
void walk(double tTotal, double turnAngle);
void stand();


#endif /* XJUS_EPOS_H_ */
