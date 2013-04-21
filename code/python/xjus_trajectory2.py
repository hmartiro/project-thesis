#!/usr/bin/python
################################################
# xJus Trajectory Script
################################################

from math import *
import numpy as np
import matplotlib.pyplot as plt
from scipy import integrate

def getTheta(t, T, thetaG, dc):
	""" 
	Returns the target angle given a time and a ground
	contact angle. In degrees.
	"""

	#def integrand(tP):
	#	return getThetaDot(tP, T, thetaG, dc)
	#return integrate.quad(integrand, 0, t)[0]

	wG = getThetaDot(  0, T, thetaG, dc)
	wA = getThetaDot(T/2, T, thetaG, dc)
	
	periods = (t - (t%T))/T
	theta = periods * 360.

	tPart = t % T
	if tPart < (dc * T):
		theta += tPart * wG
	else:
		theta += (dc * T) * wG
		theta += (tPart - (dc * T)) * wA
	print theta
	return theta


def getThetaDot(t, T, thetaG, dc):
	""" 
	Returns the target angular velocity given a time and
	a ground contact angle. In degrees per second.
	"""

	if ((t/T % 1.0) < dc):
		thetaDot = radians(thetaG) / (dc * T)
	else:
		thetaDot = (radians(thetaG) - 2*pi)/((dc-1)*T)

	return degrees(thetaDot)

T = 2.0
thetaG = 90.
dc = 0.7
DT = 30
tTotal = 2 * T

#getTheta(10.2, T, thetaG, dc)

getThetaVector = np.vectorize(getTheta)
getThetaDotVector = np.vectorize(getThetaDot)

t = np.arange((DT/1000.) / 2, tTotal, DT/1000.)
t[0] = 0.0

theta = getThetaVector(t, T, thetaG, dc)
thetaDot = getThetaDotVector(t, T, thetaG, dc)

plt.plot(t, theta, 'r', t, thetaDot, 'b')
plt.xlabel('Time (seconds)')
plt.ylabel('Angular Velocity (deg)')
plt.legend(['theta','thetaDot'])
plt.show()