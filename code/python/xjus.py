#!/usr/bin/python
# -*- coding: utf-8 -*-
################################################
# xJus Control script
################################################

import sys, os
from os.path import expanduser

from time import time
from ctypes import *
from math import *
import numpy as np
import matplotlib.pyplot as plt
import itertools

import pygame
from pygame import key, draw
from pygame import time as pytime
from pygame.locals import *

from xjus_trajectory import getTheta, getThetaDot

# Import libxjus, the wrapper library for libEposCmd.so
libxjus_dir = expanduser("~") + '/project-thesis/code/definition-files/libxjus.so'
xjus = CDLL(libxjus_dir)

#################################################
# Editable Parameters
#################################################
T = 1.1        # Trajectory period
DT = 100          # IPM time step (ms)
FPS = 100        # PyGame refresh rate

GROUND_ANGLE = 80. # Ground contact angle
BACK_GROUND_ANGLE = 40.

STAND_ANGLE = 145.
MOUNTED_STAND_ANGLE = 20.
BACK_OFFSET_ANGLE = 0.

# Fraction of base ground angle modified for turning
TURN_FRACTION = 0.5
GROUND_ANGLE_TURNING_REDUCTION = 0.75

# Amount of chunking
CHUNK_SIZE = 5

FOLLOWING_ERROR = 35000

################################################
# System constants
################################################

# Encoder ticks per revolution
REV = 59720.

# Gearbox ratio
GEAR_RATIO = 729.0/25.0

# Encoder ticks per radian
ANG_TO_QC = (512.0*4)/(2*pi) * GEAR_RATIO

# RPM for one radian per second rotation
ANG_VEL_TO_RPM = (60.0)/(360) * GEAR_RATIO

#################################################
# Node definitions                
#################################################

# List of nodes
allNodes = [FL, FR, ML, MR, BL, BR] = [1, 2, 3, 4, 5, 6]

# Active nodes
nodes = [FL, FR, ML, MR, BL, BR]

# Left and right tripods
left  = [FL, MR, BL]
right = [FR, ML, BR]

# Signs based on motor orientation
sign = {FL: 1, FR: -1, ML: 1, MR: -1, BL: 1, BR: -1}

# Node names
name = {FL: "FL", FR: "FR", ML: "ML", MR: "MR", BL: "BL", BR: "BR"}

# Signs for each tripod
tripodSign = {FL: 1, FR: -1, ML: -1, MR: 1, BL: 1, BR: -1}
zSign = {FL: 1, FR: 0, ML: 0, MR: 1, BL: 1, BR: 0}

# Colors for drawing
WHITE = (255, 255, 255)
BLACK = (  0,   0,   0)
RED   = (255,   0,   0)
GREEN = (  0, 255,   0)
BLUE  = (  0,   0, 255)

standing = False
walking = False
tapMode = False
turnLeft = False
turnRight = False
tapModeBack = False

# Command line arguments
for arg in sys.argv:
	if arg == "mounted":
		STAND_ANGLE = MOUNTED_STAND_ANGLE

def initialize():
	""" Initializes xjus and pygame """

	pygame.font.init()
	
	print "Opening connection to device..."
	xjus.openDevice()

	print "Clearing faults and enabling nodes..."
	for node in nodes:
		xjus.clearFault(node)
		
		errorCode = xjus.getErrorCode()
		if (errorCode is 872415239) or (errorCode is 10000003):
			pygame.quit()
			raise Exception("No connection to device!")
		if (errorCode == 34000007):
			pygame.quit()
			raise Exception("Turn on motors!")


		xjus.clearIpmBuffer(node)
		xjus.setMaxFollowingError(node, FOLLOWING_ERROR)
		xjus.setMaxVelocity(node, 8700)
		xjus.setMaxAcceleration(node, 1000000)
		xjus.enable(node)

		errorCode = xjus.getErrorCode()
		if (errorCode == 34000007):
			pygame.quit()
			raise Exception("Turn on motors!")

	print "Ready for action!"

	# get PID | node: 1, pP: 136, pI: 322, pD: 300 
	# get PID | node: 2, pP: 122, pI: 277, pD: 282 
	# get PID | node: 3, pP: 124, pI: 284, pD: 285 
	# get PID | node: 4, pP: 110, pI: 273, pD: 231 
	# get PID | node: 5, pP: 118, pI: 274, pD: 265 
	# get PID | node: 6, pP: 116, pI: 287, pD: 250 

	for node in nodes:
		#xjus.printPositionRegulatorGain(node)
		#pP = xjus.getPositionRegulatorGain(node, 1)
		#pI = xjus.getPositionRegulatorGain(node, 2)
		#pD = xjus.getPositionRegulatorGain(node, 3)

		#pI = int(float(pI) * 0.5)
		pP = 120
		pI =  20
		pD = 350

		xjus.setPositionRegulatorGain(node, pP, pI, pD)

			
		#xjus.printPositionRegulatorGain(node)

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
		move(node, degToPos(STAND_ANGLE))

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
		move(node, -degToPos(STAND_ANGLE))

	wait()

def plotWalk(tTotal):
	""" Creates a plot of the trajectories created for the equivalent
		call of walk(). Does not plot turning.
	"""

	thetaG = GROUND_ANGLE
	offsetPos = int(round(thetaG/2 * (REV/360)))

	t = np.arange((DT/1000.) / 2, tTotal, DT/1000.)
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

#########################################################################
# TRIPOD FUNCTIONS
#########################################################################

def startTripod(turnAngle=0, back=False):
	""" Move to the starting position of the tripod gait and fill the
	    buffer with some initial points. """

	for node in nodes:
		xjus.profilePositionMode(node)
		xjus.setPositionProfile(node, 500, 10000, 5000)

	print "Moving to start position..."
	for node in nodes:

		[p, v, dt] = getTripodPVT(node, 0, turnAngle=turnAngle, back=back)

		if back:
			move(node, -p, absolute=True)
		else:
			move(node, p, absolute=True)
	
	wait()

	for node in nodes:
		xjus.interpolationMode(node)

	# Start time is half of dt
	t = (DT/1000.) / 2

	# fill buffer
	for i in range(5):
		addTripodPoint(t, turnAngle, back=back)
		t += DT/1000.

	for node in nodes:
		xjus.startIPM(node)

	return t;

def tripodFrame(t0, turnAngle=0, back=False):
	""" Loop function of the tripod gait - fills in trajectory points
	    as needed to keep the controller buffers full. """

	t = t0

	bufferSize = [64 - xjus.getFreeBufferSize(node) for node in nodes]
	bufferMaxLimit = CHUNK_SIZE * 1.5
	
	fillBuffer= len([b for b in bufferSize if b <= bufferMaxLimit]) == len(nodes)
	if fillBuffer:
		for i in range(CHUNK_SIZE):
			addTripodPoint(t, turnAngle, back=back)
			t += DT/1000.

	print("Buffer: %s, Added points: %r" % (bufferSize, fillBuffer))

	return t

def stopTripod(t, turnAngle=0, back=False):
	""" Adds an ending point to the tripod gait and returns to a
	    standing position. """

	pytime.wait(DT)
	for node in nodes:
		addTripodPoint(t, turnAngle, back=back, end=True)

	for node in nodes:
		xjus.stopIPM(node)

	wait()

	returnToStand()

def addTripodPoint(t, turnAngle=0, back=False, end=False):
	""" Adds a PVT point for each node, for the tripod trajectory. """

	nA = []
	pA = []
	vA = []
	tA = []

	for node in nodes:

		[p, v, dt] = getTripodPVT(node, t, turnAngle=turnAngle, back=back)

		nA.append(node)
		pA.append(p)
		if end:
			vA.append(0)
			tA.append(0)
		else:
			vA.append(v)
			tA.append(dt)

	if back:
		pA = [-p for p in pA]
		vA = [-v for v in vA]

	addPvtArray(nA, pA, vA, tA)

def getTripodPVT(node, t, turnAngle=0, back=False):
	""" Given a node and time, returns a single PVT point. """

	if back:
		thetaG = BACK_GROUND_ANGLE
	else:
		thetaG = GROUND_ANGLE

	if turnAngle != 0:
		thetaG *= GROUND_ANGLE_TURNING_REDUCTION

	if (sign[node] * turnAngle < 0):
		thetaG += sign[node] * turnAngle * GROUND_ANGLE_TURNING_REDUCTION

	offsetPos = int(round(thetaG/2 * (REV/360)))
	p = degToPos(getTheta(t + zSign[node]*T/2., T, thetaG)) - offsetPos
	v = int(round(getThetaDot(t + zSign[node]*T/2., T, thetaG) * ANG_VEL_TO_RPM))
	dt = DT

	if back:
		backOffsetPos = int(round(BACK_OFFSET_ANGLE * (REV/360)))
		p += backOffsetPos

	return [p, v, dt]

def addPvtArray(nodes, positions, velocities, times):
	""" Sends the given PVT points for each node to the controller. """

	#print("positions: %s" % positions)

	N = len(nodes)

	for i in range(N):
		node = nodes[i]
		positions[i] *= sign[node]
		velocities[i] *= sign[node]

	n = (c_ushort * N)(*nodes)
	p = (c_long * N)(*map(int, map(round, positions)))
	v = (c_long * N)(*map(int, map(round, velocities)))
	t = (c_ubyte * N)(*times)

	xjus.addPvtAll.argtypes = [c_int, (c_ushort * N), (c_long * N), (c_long * N), (c_ubyte * N)]
	xjus.addPvtAll(N, n, p, v, t)

def addPvt(node, position, velocity, time):
	""" Sends the given PVT point to the controller. """

	p = sign[node] * int(round(position))
	v = sign[node] * int(round(velocity))
	t = int(round(time))
	xjus.addPVT(node, p, v, t)

def wait():
	""" Waits until all nodes are inactive. """
	for node in nodes:
		while not xjus.isFinished(node):
			pytime.wait(10)
			#print(xjus.getErrorCode())
def printCurrentToCommand():
	""" Queries the current use for each node and prints it to the command line """

	for node in nodes:
		measuredCurrent = xjus.getNodeAvgCurrent(node)
		
		if (node == 1):
			output = str(measuredCurrent) + ", "
		elif (node == 6):
			output += str(measuredCurrent)
		else:
			output += str(measuredCurrent) + ", "

	print(output);

def finishCurrentToFile(fileId):
	""" Closes the file containing current information """
	fileId.close()

def currentToFile(fileId):
	""" Queries the current use for each node and writes it to a specified file """

	for node in nodes:
		measuredCurrent = xjus.getNodeAvgCurrent(node)

		if (node == 1):
			output = str(measuredCurrent) + ", "
		elif (node == 6):
			output += str(measuredCurrent)
		else:
			output += str(measuredCurrent) + ", "

	fileId.write(output + '\n')

def mainLoop(clock, surface):
	"""
	Represents the main control loop, where key events are
	processed and high-level routines activated.
	"""
	printCurrent = False
	fileId = open('currentOutput.txt', 'a')

	global walking, tapMode, tapModeBack, turnLeft, turnRight
	global T, GROUND_ANGLE

	# IPM time variable
	t = 0

	timer = time()
	while True:
		
		if (printCurrent):
			printCurrentToCommand()
			currentToFile(fileId)
			printCurrent = False

		# Processing all events for the frame
		for event in pygame.event.get():

			# Key down events
			if event.type == KEYDOWN:

				# Get current on request
				if event.key == K_c:
					printCurrent = not printCurrent

				# Tooggle stand on spacebar
				if event.key == K_SPACE:

					if standing and not walking:
						drawText("Sitting down")
						sit()
					else:
						drawText("Standing up")
						stand()

				# Toggle continuous walking
				elif (event.key is K_w) and (tapMode is False):
					if standing and not walking:
						drawText("Walking forward")
						walking = True
						turnLeft = False
						turnRight = False
						t = startTripod()

					else:
						print "Must stand first!"

				elif (event.key is K_s) and (tapModeBack is False):
					if standing and not walking:
						drawText("Walking backward")
						walking = True
						turnLeft = False
						turnRight = False
						t = startTripod(back=True)

				if (event.key is K_w):
					tapMode = not tapMode
				if (event.key is K_s):
					tapModeBack = not tapModeBack
				if (event.key is K_a):
					turnLeft = not turnLeft
					turnRight = False
				if (event.key is K_d):
					turnLeft = False
					turnRight = not turnRight
		
				if (event.key is K_t) and not walking:
					T = float(raw_input('New movement period: '))
				if (event.key is K_g) and not walking:
					GROUND_ANGLE = float(raw_input('New ground angle: '))
				if (event.key is K_e):
					print("Error code: %x" % xjus.getErrorCode())
				# Exit on escape
				if event.key == K_ESCAPE:
					return

			# Quit event, clicking the X
			if event.type == QUIT:
				return

		#print("walking: %r, up key down: %r" % (walking, keyDown(K_UP)))
		if walking:

			# Get the turn angle
			turnFraction = 0
			if turnRight:
				turnFraction = +TURN_FRACTION
			elif turnLeft:
				turnFraction = -TURN_FRACTION
				
			if tapMode:
				#timer = time()
				t = tripodFrame(t, turnFraction * GROUND_ANGLE)
				#print("Time of tripodFrame() call: %f" % (time()-timer))
			elif tapModeBack:
				t = tripodFrame(t, turnFraction * BACK_GROUND_ANGLE, back=True)
			else:
				drawText("Standing up")
				walking = False
				turnLeft = False
				turnRight = False
				stopTripod(t, turnFraction * GROUND_ANGLE)

		# Pygame frame
		pygame.display.update()
		clock.tick(FPS)

def drawText(string):
	""" Draws centered text in the control window """

	screen.fill(WHITE)
	font = pygame.font.SysFont(None, 48)
	text = font.render(string, True, BLACK)
	textRect = text.get_rect()
	textRect.centerx = screen.get_rect().centerx
	textRect.centery = screen.get_rect().centery
	screen.blit(text, textRect)

def main():
	""" Starting point for the program, calls mainLoop() """

	global screen

	initialize()

	# Creates the control window
	screen = pygame.display.set_mode((400, 400))
	pygame.display.set_caption("xJüs Control Window")
	screen.fill(WHITE)

	drawText('Control Window')

	# Object that maintains a constant FPS
	clock = pygame.time.Clock()

	mainLoop(clock, screen)

	deinitialize()
	return

# Call the main function when script is executed
if __name__ == "__main__":
    main()
