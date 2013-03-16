#!/usr/bin/python
# -*- coding: utf-8 -*-
################################################
# xJus Control script
################################################

import sys, os
from time import sleep, time
from ctypes import *
from math import *
import numpy as np
import matplotlib.pyplot as plt

import pygame
from pygame import key, draw
from pygame.locals import *

# Import our C++ library
xjus = CDLL('/home/hayk/workspace/libxjus/Library/libxjus.so')

#################################################
# Editable Parameters
#################################################
T = 1.5          # Trajectory period
dt = 75          # IPM time step (ms)
baseThetaG = 45. # Ground contact angle
FPS = 70         # PyGame refresh rate

# Is the robot mounted in the air?
MOUNTED = True

if MOUNTED:
	standAngle = 25.  # Mounted standing angle
else:
	standAngle = 145. # Standing angle

# How many periods the walk is for
walkPeriods = 2

# Fraction of base ground angle modified for turning
turning = 0.5

#################################################
# Node definitions                
#################################################

# List of nodes
allNodes = [FL, FR, ML, MR, BL, BR] = [1, 2, 3, 4, 5, 6]

# Active nodes
nodes = [FR, MR, BR, FL, ML, BL]

# Left and right tripods
left  = [FL, MR, BL]
right = [FR, ML, BR]

# Signs based on motor orientation
sign = {FL: 1, FR: -1, ML: 1, MR: -1, BL: 1, BR: -1}

# Node names
name = {FL: "FL", FR: "FR", ML: "ML", MR: "MR", BL: "BL", BR: "BR"}

# Signs for each tripod
tripodSign = {FL: 1, FR: -1, ML: -1, MR: 1, BL: 1, BR: -1}

# Trajectory offset for legs
t0Left  = T/2
t0Right =  0.
t0 = {FL: t0Left, FR: t0Right, ML: t0Right, MR: t0Left, BL: t0Left, BR: t0Right}

# Colors for drawing
WHITE = (255, 255, 255)
BLACK = (  0,   0,   0)
RED   = (255,   0,   0)
GREEN = (  0, 255,   0)
BLUE  = (  0,   0, 255)

standing = False
walking = False

REV = 59720

M = 10
GEAR_RATIO = 729.0/25.0
ANG_TO_QC = (512.0*4)/(2*pi) * GEAR_RATIO
ANG_VEL_TO_RPM = (60.0)/(360) * GEAR_RATIO

def initialize():
	""" Initializes xjus and pygame """

	pygame.init()

	print "Opening connection to device..."
	xjus.openDevice()

	print "Clearing faults and enabling nodes..."
	for node in nodes:
		xjus.clearFault(node)

		if (xjus.getErrorCode() == 872415239):
			pygame.quit()
			raise Exception("No connection to device!")

		xjus.clearIpmBuffer(node)
		xjus.setMaxFollowingError(node, 20000)
		xjus.setMaxVelocity(node, 8700)
		xjus.setMaxAcceleration(node, 1000000)
		xjus.enable(node)

	print "Ready for action!"

def deinitialize():
	""" Deconstructs xjus and pygame """

	print "Disabling nodes..."
	for node in nodes:
		xjus.disable(node)

	xjus.closeDevice()
	print "Connection to device closed."

	pygame.quit()

def keyDown(k):
	""" Returns True if the given k is currently down """

	return key.get_pressed()[k]

def degToPos(angle):
	""" Returns the number of encoder ticks for the given angle """

	return int(round(REV*(angle/360.)));

def move(node, distance, absolute=False):
	""" Moves the given node in the forward direction. """

	if absolute:
		xjus.moveAbsolute(node, sign[node] * int(distance))
	else:
		xjus.moveRelative(node, sign[node] * int(distance))

def stand():
	""" Raises the chassis into a standing position """

	global standing
	standing = True

 	for node in nodes:
		xjus.setPositionProfile(node, 500, 5000, 5000)

	for node in nodes:
		move(node, degToPos(standAngle))

	wait()
	
	for node in nodes:
		xjus.zeroPosition(node)

def sit():
	""" Lowers the chassis to the ground """

	global standing
	standing = False

 	for node in nodes:
		xjus.setPositionProfile(node, 500, 5000, 5000)

	for node in nodes:
		move(node, -degToPos(standAngle))

	wait()

def plotWalk(tTotal):
	""" Creates a plot of the trajectories created for the equivalent
		call of walk(). Does not plot turning.
	"""

	thetaG = baseThetaG
	offsetPos = int(round(thetaG/2 * (REV/360)))

	t = np.arange((dt/1000.) / 2, tTotal, dt/1000.)
	t[0] = 0.0

	# Vectorized functions
	getThetaVector = np.vectorize(getTheta)
	getThetaDotVector = np.vectorize(getThetaDot)
	degToPosVector = np.vectorize(degToPos)

	thetaR = getThetaVector(t,     thetaG)
	thetaL = getThetaVector(t+T/2., thetaG)

	posR = degToPosVector(thetaR) + offsetPos
	posL = degToPosVector(thetaL) - offsetPos

	plt.plot(t, posR, 'r', t, posL, 'b')
	plt.xlabel('Time (seconds)')
	plt.ylabel('Position (ticks)')
	plt.title('Walking Trajectory')
	plt.legend(('Right Tripod', 'Left Tripod'))
	plt.show()

def moveToFullRotation(node):

	p = getPosition(node)
	targetForward = p + (REV - (p % REV))
	targetBack    = p - (p % REV)

	forwardDist = targetForward - p
	backwardDist = p - targetBack

	#print("node: %s, p: %d, forward: %d, back: %d" % (name[node], p, forwardDist, backwardDist))

	xjus.setPositionProfile(node, 1000, 10000, 5000)

	if (backwardDist < REV/15.):
		#print("moving backward to %d" % targetBack)
		move(node, targetBack, absolute=True)
	else:
		#print("moving forward to %d" % targetForward)
		move(node, targetForward, absolute=True)

def returnToStand():

	for node in left:
		moveToFullRotation(node)

	wait()

	for node in right:
		moveToFullRotation(node)

	wait()

	for node in nodes:
		xjus.zeroPosition(node)

def getPosition(node):

	return sign[node] * xjus.getPosition(node)

def startContinuousWalk(turnAngle=0):

	global walking
	walking = True

	for node in nodes:
		xjus.profilePositionMode(node)
		xjus.setPositionProfile(node, 500, 10000, 5000)

	print "Moving to start position..."
	for node in nodes:
		thetaG = baseThetaG + sign[node] * turnAngle
		offsetPos = int(round(thetaG/2 * (REV/360)))
		p_start = tripodSign[node] * degToPos(getTheta(t0[node], thetaG)) - offsetPos
		move(node, p_start, absolute=True)
		#print("node: %d, p_start: %d, thetaG: %f" % (node, p_start, thetaG))

	wait();

	for node in nodes:
		xjus.interpolationMode(node)

	# Start time is half of dt
	t = (dt/1000.) / 2

	# fill buffer
	for i in range(5):
		addTrajectoryPoint(t, turnAngle)
		t += dt/1000.

	for node in nodes:
		xjus.startIPM(node)

	return t;

def walkFrame(time, turnAngle=0):

	t = time
	bufferSize = [xjus.getFreeBufferSize(node) for node in nodes]

	#bufferHas10 = [xjus.getFreeBufferSize(node) > 9 for node in nodes]
	print("t = %f, buffer: %s" % (t, bufferSize))
	#for node in nodes:
	#	print("node: %d, position: %d" % (node, xjus.getPosition(node)))

	chunkSize = 10
	if len([b for b in bufferSize if b >= chunkSize]) == len(nodes):
		for i in range(chunkSize):
			addTrajectoryPoint(t, turnAngle)
			t += dt/1000.

	return t

def stopContinuousWalk(t, turnAngle=0):

	global walking
	walking = False

	sleep(dt/1000.)
	for node in nodes:
		addTrajectoryPoint(t, turnAngle, end=True)

	#for node in nodes:
	#	xjus.printIpmStatus(node)

	#wait()

	for node in nodes:
		xjus.stopIPM(node)

	wait()

	returnToStand()

def walk(tTotal, turnAngle=0):
	""" Forward locomotion of the robot for a fixed amount of time"""

	for node in nodes:
		xjus.profilePositionMode(node)
		xjus.setPositionProfile(node, 500, 10000, 5000)

	print "Moving to start position..."
	for node in nodes:
		thetaG = baseThetaG + sign[node] * turnAngle
		offsetPos = int(round(thetaG/2 * (REV/360)))
		p_start = tripodSign[node] * degToPos(getTheta(t0[node], thetaG)) - offsetPos
		move(node, p_start, absolute=True)
		#print("node: %d, p_start: %d, thetaG: %f" % (node, p_start, thetaG))

	wait();
	#print("pL: %d, pR: %d" % (xjus.getPosition(FL), xjus.getPosition(FR)))
	#return
	for node in nodes:
		xjus.interpolationMode(node)

	# Start time is half of dt
	t = (dt/1000.) / 2

	# fill buffer
	for i in range(5):
		addTrajectoryPoint(t, turnAngle)
		t += dt/1000.

	for node in nodes:
		xjus.startIPM(node)

	while t < tTotal:

		bufferSize = [xjus.getFreeBufferSize(node) for node in nodes]

		#bufferHas10 = [xjus.getFreeBufferSize(node) > 9 for node in nodes]
		print("t = %f, buffer: %s" % (t, bufferSize))
		#for node in nodes:
		#	print("node: %d, position: %d" % (node, xjus.getPosition(node)))

		chunkSize = 10
		if len([b for b in bufferSize if b >= chunkSize]) == len(nodes):
			for i in range(chunkSize):
				addTrajectoryPoint(t, turnAngle)
				t += dt/1000.

	sleep(dt/1000.)
	for node in nodes:
		addTrajectoryPoint(t, turnAngle, end=True)

	print("Adding points ended, t = %f" % t)
	for node in nodes:
		xjus.printIpmStatus(node)

	wait()

	for node in nodes:
		xjus.stopIPM(node)

	wait()

	returnToStand()

def addTrajectoryPoint(t, turnAngle=0, end=False):
	"""
	Calculates a PVT point for each node at time t and adds 
	it to the IPM buffer.
	"""

	for node in nodes:

		thetaG = baseThetaG + sign[node] * turnAngle
		offsetPos = int(round(thetaG/2 * (REV/360)))
		p = degToPos(getTheta(t + t0[node], thetaG)) - offsetPos
		v = int(round(getThetaDot(t + t0[node], thetaG) * ANG_VEL_TO_RPM))

		if end:
			addPVT(node, p, 0, 0)
		else:
			addPVT(node, p, v, dt)
		#print("node: %d, P: %d, V: %d, T: %d" % (node, p, v, dt))

def addPVT(node, position, velocity, time):
	""" Sends the given PVT point to the controller. """

	p = sign[node] * int(round(position))
	v = sign[node] * int(round(velocity))
	t = int(round(time))
	xjus.addPVT(node, p, v, t)

def getTheta(t, thetaG):
	""" 
	Returns the target angle given a time and a ground
	contact angle. In degrees.
	"""

	theta = (2*pi/T) * t
	for m in range(1,M+1):
		B = 4*(radians(thetaG)-pi)/((2*m-1)*(2*m-1)*pi*pi)
		theta += B*(1-cos(2*pi*(2*m-1)*t/T))
		#print("m: %d, B: %f, theta: %f" % (m, B, theta))

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

def wait():
	""" Waits until all nodes are inactive. """
	for node in nodes:
		while not xjus.isFinished(node):
			sleep(0.010)

def mainLoop(clock, surface):
	"""
	Represents the main control loop, where key events are
	processed and high-level routines activated.
	"""

	# IPM time variable
	t = 0

	while True:

		# Processing all events for the frame
		for event in pygame.event.get():

			# Key down events
			if event.type == KEYDOWN:

				# Tooggle stand on spacebar
				if event.key == K_SPACE:
					if standing and not walking:
						print "Go to sitting position."
						sit()
					else:
						print "Go to standing position."
						stand()

				# Toggle walking
				elif event.key == K_UP:
					if standing and not walking:
						print "Start walking forward!"
						if keyDown(K_RIGHT):
							print "Turning right!"
							#walk(T * walkPeriods, +turning * baseThetaG)
							t = startContinuousWalk(+turning * baseThetaG)
						elif keyDown(K_LEFT):
							print "Turning left!"
							#walk(T * walkPeriods, -turning * baseThetaG)
							t = startContinuousWalk(-turning * baseThetaG)
						else:
							print "Walking forward!"
							#walk(T * walkPeriods)
							t = startContinuousWalk()
					else:
						print "Must stand first!"

				elif event.key == K_RIGHT:
					pass
				elif event.key == K_LEFT:
					pass
				elif event.key == K_DOWN:
					pass

				# Exit on escape
				elif event.key == K_ESCAPE:
					return

			# Key up events
			if event.type == KEYUP:

				if event.key == K_UP:
					if standing:
						print "Stop walking and resume stand!"

			# Quit event, clicking the X
			if event.type == QUIT:
				return

		#print("walking: %r, up key down: %r" % (walking, keyDown(K_UP)))
		if walking:

			# Get the turn angle
			if keyDown(K_RIGHT):
				turnAngle = +turning * baseThetaG
			elif keyDown(K_LEFT):
				turnAngle = -turning * baseThetaG
			else:
				turnAngle = 0

			if keyDown(K_UP):
				t = walkFrame(t, turnAngle)
			else:
				stopContinuousWalk(t, turnAngle)

		# Pygame frame
		pygame.display.update()
		clock.tick(FPS)

def drawText(screen, string):
	""" Draws centered text in the control window """

	font = pygame.font.SysFont(None, 48)
	text = font.render(string, True, BLACK)
	textRect = text.get_rect()
	textRect.centerx = screen.get_rect().centerx
	textRect.centery = screen.get_rect().centery
	screen.blit(text, textRect)

def main():
	""" Starting point for the program, calls mainLoop() """

	initialize()

	# Creates the control window
	screen = pygame.display.set_mode((400, 400))
	pygame.display.set_caption("xJüs Control Window")
	screen.fill(WHITE)
	drawText(screen, 'Control Window')

	# Object that maintains a constant FPS
	clock = pygame.time.Clock()

	mainLoop(clock, screen)

	deinitialize()
	return

# Call the main function when script is executed
if __name__ == "__main__":
    main()
