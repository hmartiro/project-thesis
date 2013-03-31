#!/usr/bin/python
################################################
# xJus Trajectory Script
################################################

from math import *

# Fourier approximation terms
M = 10

# System constants
REV = 59720
GEAR_RATIO = 729.0/25.0
ANG_TO_QC = (512.0*4)/(2*pi) * GEAR_RATIO
ANG_VEL_TO_RPM = (60.0)/(360) * GEAR_RATIO

def getTheta(t, thetaG):
	""" 
	Returns the target angle given a time and a ground
	contact angle. In degrees.
	"""

	theta = (2*pi/T) * t
	for m in range(1,M+1):
		B = 4*(radians(thetaG)-pi)/((2*m-1)*(2*m-1)*pi*pi)
		theta += B*(1-cos(2*pi*(2*m-1)*t/T))

	return degrees(theta)

def getThetaDot(t, thetaG):
	""" 
	Returns the target angular velocity given a time and
	a ground contact angle. In degrees per second.
	"""

	thetaDot = (2*pi/T)
	for m in range(1,M+1):
		B = 8*(radians(thetaG)-pi)/((2*m-1)*pi*T)
		thetaDot += B*sin(2*pi*(2*m-1)*t/T)

	return degrees(thetaDot)