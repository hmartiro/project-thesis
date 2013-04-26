#!/usr/bin/python
# -*- coding: utf-8 -*-
################################################
# xJus Control script
################################################

import sys, os
from os.path import expanduser

from time import time
from math import *
import numpy as np
import matplotlib.pyplot as plt
import itertools

import pygame
from pygame import key, draw
from pygame import time as pytime
from pygame.locals import *

from xjus_trajectory2 import getTheta, getThetaDot

import xjusAnalysis

sys.path.insert(0, '/home/xjus/project-thesis/code/libxjus')
import xjus_API as xjus

#################################################
# Editable Parameters
#################################################
T =  0.90        # Trajectory period
DT = 150    # IPM time step (ms)
FPS = 50        # PyGame refresh rate
PLOT_ANALYSIS = True

GROUND_ANGLE = 95. # Ground contact angle
BACK_GROUND_ANGLE = 100.

DUTY_CYCLE = 0.65

PHASE_OFFSET = -GROUND_ANGLE/2
TIME_OFFSET = T * (DUTY_CYCLE / 2)

STAND_ANGLE = 145.
MOUNTED_STAND_ANGLE = 20.
BACK_OFFSET_ANGLE = 0.

BUFFER_MAX_LIMIT = 10

# Fraction of base ground angle modified for turning
TURN_FRACTION = 0.1
DUTY_TURN_FRACTION = 0.03
GROUND_ANGLE_TURNING_REDUCTION = 1.0

FOLLOWING_ERROR = 150000
MAX_VELOCITY = 8700
MAX_ACCELERATION = 1000000

P_GAIN = 200
I_GAIN =  10
D_GAIN = 200
FEEDFORWARD_VELOCITY = 0
FEEDFORWARD_ACCELERATION = 100

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
	xjus.openDevices()

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
		xjus.setMaxVelocity(node, MAX_VELOCITY)
		xjus.setMaxAcceleration(node, MAX_ACCELERATION)

		err = xjus.getMaxFollowingError(node)
		vel = xjus.getMaxVelocity(node)
		acc = xjus.getMaxAcceleration(node)
		print("node: %d, following error: %d, max velocity: %d, max acceleration: %d" % (node, err, vel, acc))

		xjus.enable(node)

		errorCode = xjus.getErrorCode()
		if (errorCode == 34000007):
			pygame.quit()
			raise Exception("Turn on motors!")

	print "Ready for action!"

	# get PID | node: 1, pP: 136, pI: 322, pD: 300, fV: 0,    fA: 105
	# get PID | node: 2, pP: 122, pI: 277, pD: 282, fV: 0,    fA: 95
	# get PID | node: 3, pP: 124, pI: 284, pD: 285, fV: 0,    fA: 87
	# get PID | node: 4, pP: 110, pI: 273, pD: 231, fV: 0,    fA: 83
	# get PID | node: 5, pP: 118, pI: 274, pD: 265, fV: 0,    fA: 90
	# get PID | node: 6, pP: 116, pI: 287, pD: 250, fV: 1703, fA: 99

	for node in nodes:

		xjus.setPositionRegulatorGain(node, P_GAIN, I_GAIN, D_GAIN)
		xjus.setPositionRegulatorFeedForward(node, FEEDFORWARD_VELOCITY, FEEDFORWARD_ACCELERATION)

		pP = xjus.getPositionRegulatorGain(node, 1)
		pI = xjus.getPositionRegulatorGain(node, 2)
		pD = xjus.getPositionRegulatorGain(node, 3)
		fV = xjus.getPositionRegulatorFeedForward(node, 1)
		fA = xjus.getPositionRegulatorFeedForward(node, 2)

		print("pP: %d, pI: %d, pD: %d, fV: %d, fA: %d" % (pP, pI, pD, fV, fA))

def deinitialize():
	""" Deconstructs xjus and pygame """

	print "Disabling nodes..."
	for node in nodes:
		xjus.disable(node)

	xjus.closeDevices()
	print "Connection to device closed."

	pygame.quit()

def nodeFault():
	""" Returns True if any node is in a fault state. """

	for node in nodes:
		if xjus.getState(node) == 3:
			return True
	return False

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
		xjus.setPositionProfile(node, 500, 10000, 10000)

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
		xjus.setPositionProfile(node, 500, 10000, 10000)

	for node in nodes:
		move(node, -degToPos(STAND_ANGLE))

	wait()

def moveToFullRotation(node):

	p = getPosition(node)
	targetForward = p + (REV - (p % REV))
	targetBack    = p - (p % REV)

	forwardDist = targetForward - p
	backwardDist = p - targetBack

	#print("node: %s, p: %d, forward: %d, back: %d" % (name[node], p, forwardDist, backwardDist))

	xjus.setPositionProfile(node, 1500, 10000, 5000)

	if (backwardDist < REV/15.):
		#print("moving backward to %d" % targetBack)
		move(node, targetBack, absolute=True)
	else:
		#print("moving forward to %d" % targetForward)
		move(node, targetForward, absolute=True)

def returnToStand():

	leftCurrent = 0
	for node in left:
		leftCurrent += xjus.getNodeAvgCurrent(node)

	rightCurrent = 0
	for node in right:
		rightCurrent += xjus.getNodeAvgCurrent(node)

	if leftCurrent > rightCurrent:
		first = right
		second = left
	else:
		first = left
		second = right

	for node in first:
		moveToFullRotation(node)

	wait()

	for node in second:
		moveToFullRotation(node)

	wait()

	for node in nodes:
		xjus.zeroPosition(node)

def getPosition(node):

	return sign[node] * xjus.getPosition(node)

#########################################################################
# TRIPOD FUNCTIONS
#########################################################################

def startTripod(turnAngle=0, back=False, duty_turn=0):
	""" Move to the starting position of the tripod gait and fill the
	    buffer with some initial points. """

	for node in nodes:
		xjus.profilePositionMode(node)
		xjus.setPositionProfile(node, 1500, 10000, 5000)

	print "Moving to start position..."
	for node in nodes:

		[p, v, dt] = getTripodPVT(node, TIME_OFFSET, turnAngle=turnAngle, back=back)

		print("Start position. node: %d, p: %d, v: %d" % (node, p, v))

		#if node in left:
		if back:
			move(node, -p, absolute=True)
		else:
			move(node, p, absolute=True)

	wait()

	for node in nodes:
		xjus.interpolationMode(node)

	# Start time is half of dt
	t = (DT/1000.) / 2 + TIME_OFFSET

	# fill buffer
	for i in range(BUFFER_MAX_LIMIT/2):

		[nA, pA, vA, tA] = addTripodPoint(t, turnAngle, back=back, duty_turn=duty_turn)
		addPvtArray(nA, pA, vA, tA)
		t += DT/1000.

	for node in nodes:
		xjus.startIPM(node)

	xjusAnalysis.startAccel(2, PLOT_ANALYSIS)
	xjusAnalysis.startAvgCurrent(2, PLOT_ANALYSIS)

	return t;

def tripodFrame(t0, turnAngle=0, back=False, duty_turn=0):
	""" Loop function of the tripod gait - fills in trajectory points
	    as needed to keep the controller buffers full. """

	t = t0
	timer = time()

	#bufferSize = [64 - xjus.getFreeBufferSize(node) for node in nodes]
	bufferSize = 64 - xjus.getFreeBufferSize(FR)
	#print("Time to check buffer size: %fs" % (time() - timer))
	#fillBuffer= len([b for b in bufferSize if b <= BUFFER_MAX_LIMIT]) == len(nodes)
	fillBuffer = (bufferSize <= BUFFER_MAX_LIMIT)
	if fillBuffer:

		timer = time()

		[nA, pA, vA, tA] = addTripodPoint(t, turnAngle, back=back, duty_turn=duty_turn)
		addPvtArray(nA, pA, vA, tA)
		t += DT/1000.

		#print("Add PVT time in Python: %fs" % (time() - timer))

	print("Buffer: %s, Added points: %r" % (bufferSize, fillBuffer))

	# [nA, pA, vA, tA] = addTripodPoint(t-(DT/1000.)*bufferSize, turnAngle, back=back, duty_turn=duty_turn)
	# current = [xjus.getPosition(node) for node in nodes]
	# for i in range(6):
	# 	node = nodes[i]
	# 	current[i] *= sign[node]

	#errors = [pA[i]-current[i] for i in range(len(nodes))]
	#print("following error: %s" % errors)

	timer = time()
	xjusAnalysis.sampleAvgCurrent()
	print("Time to sample current: %f" % (time() - timer))

	return t

def stopTripod(t, turnAngle=0, back=False, duty_turn=0):
	""" Adds an ending point to the tripod gait and returns to a
	    standing position. """

	xjusAnalysis.endAccel(PLOT_ANALYSIS)
	#acc = xjusAnalysis.getAvgAbsZAccel()
	#print("===============================================")
	#print("Stability measure: %.4f" % (acc))
	current = xjusAnalysis.getAvgCurrent()
	#print("Power usage measure: %.4f" % (current))
	#print("===============================================")
	
	pytime.wait(DT)
	for node in nodes:
		[nA, pA, vA, tA] = addTripodPoint(t, turnAngle, back=back, end=True, duty_turn=duty_turn)
		addPvtArray(nA, pA, vA, tA)

	for node in nodes:
		xjus.stopIPM(node)
		xjus.printIpmStatus(node)

	wait()

	returnToStand()

def addTripodPoint(t, turnAngle=0, back=False, end=False, duty_turn=0):
	""" Adds a PVT point for each node, for the tripod trajectory. """

	nA = np.zeros(shape=(len(nodes)))
	pA = np.zeros(shape=(len(nodes)))
	vA = np.zeros(shape=(len(nodes)))
	tA = np.zeros(shape=(len(nodes)))

	for i in range(len(nodes)):

		[p, v, dt] = getTripodPVT(nodes[i], t, turnAngle=turnAngle, back=back, duty_turn=duty_turn)

		nA[i] = nodes[i]
		pA[i] = p
		vA[i] = v
		tA[i] = dt

	if end:
		vA = [0. for v in vA]
		tA = [0. for t in tA]

	if back:
		pA = [-p for p in pA]
		vA = [-v for v in vA]

	return [nA, pA, vA, tA]

def getTripodPVT(node, t, turnAngle=0, back=False, duty_turn=0):
	""" Given a node and time, returns a single PVT point. """

	if back:
		thetaG = BACK_GROUND_ANGLE
	else:
		thetaG = GROUND_ANGLE

	if turnAngle != 0:
		thetaG *= GROUND_ANGLE_TURNING_REDUCTION

	dc = DUTY_CYCLE

	#if (sign[node] * duty_turn < 0):
	dc -= sign[node] * duty_turn

	if (sign[node] * turnAngle < 0):
		thetaG += sign[node] * turnAngle * GROUND_ANGLE_TURNING_REDUCTION

	offsetPos = int(round(PHASE_OFFSET * (REV/360)))
	p = degToPos(getTheta(t + zSign[node]*T/2., T, thetaG, dc)) + offsetPos
	v = int(round(getThetaDot(t + zSign[node]*T/2., T, thetaG, dc) * ANG_VEL_TO_RPM))
	dt = DT

	if back:
		backOffsetPos = int(round(BACK_OFFSET_ANGLE * (REV/360)))
		p += backOffsetPos

	return [p, v, dt]

def addPvtArray(nA, pA, vA, tA):
	""" Sends the given PVT points for each node to the controller. """

	N = len(nA)

	for i in range(N):
		node = nA[i]
		pA[i] *= sign[node]
		vA[i] *= sign[node]

	pvt = np.vstack([nA, pA, vA, tA])
	pvt = np.ascontiguousarray(pvt.transpose().astype(int))

	#timer = time()
	xjus.addPvtFrame(pvt)
	#print("Add PVT time to C code: %fs" % (time() - timer))

def wait():
	""" Waits until all nodes are inactive. """
	for node in nodes:
		while not xjus.isFinished(node):
			pytime.wait(10)
			#print(xjus.getErrorCode())

def mainLoop(clock, surface):
	"""
	Represents the main control loop, where key events are
	processed and high-level routines activated.
	"""


	global walking, tapMode, tapModeBack, turnLeft, turnRight
	global T, GROUND_ANGLE

	# IPM time variable
	t = 0
	frame = 0

	while True:

		timer0 = time()

		frame += 1
		print("--------- Main loop frame %d ---------- error code %d" % (frame, xjus.getErrorCode()))



		# Stops the program if there is a node in fault state
		if (frame % 10) == 0:

	 		timer = time()
			if nodeFault():
			 	print("Error occurred! Error code: %d" % xjus.getErrorCode())
			 	
			 	for node in nodes:
	 				xjus.printIpmStatus(node)
	 			return
			print("nodeFault() call: %f" % (time()-timer))


		# Processing all events for the frame
		for event in pygame.event.get():

			# Key down events
			if event.type == KEYDOWN:


				if event.key == (K_EQUALS):
					Tnew = T + 0.05
					t = (Tnew/T) * t
					T = Tnew

				if event.key == (K_MINUS):
					Tnew = T - 0.05
					t = (Tnew/T) * t
					T = Tnew

				if event.key == (K_RIGHTBRACKET):
					GROUND_ANGLE += 5

				if event.key == (K_LEFTBRACKET):
					GROUND_ANGLE -= 5

				# Tooggle stand on spacebar
				if event.key == K_SPACE:

					if standing and not walking:
						sit()
					elif not walking:
						stand()
					else:
						tapMode = False
						tapModeBack = False

				# Toggle continuous walking
				elif (event.key is K_w) and (tapMode is False):
					if standing and not walking:
						walking = True
						turnLeft = False
						turnRight = False
						t = startTripod()

					else:
						print "Must stand first!"

				elif (event.key is K_s) and (tapModeBack is False):
					if standing and not walking:
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
			duty_turn = 0
			if turnRight:
				turnFraction = +TURN_FRACTION
				duty_turn = +DUTY_TURN_FRACTION
			elif turnLeft:
				turnFraction = -TURN_FRACTION
				duty_turn = -DUTY_TURN_FRACTION

			if tapMode:
				timer = time()
				t = tripodFrame(t, turnFraction * GROUND_ANGLE, duty_turn=duty_turn)
				print("tripodFrame() call: %f" % (time()-timer))
			elif tapModeBack:
				t = tripodFrame(t, turnFraction * BACK_GROUND_ANGLE, back=True, duty_turn=duty_turn)
			else:
				walking = False
				turnLeft = False
				turnRight = False
				stopTripod(t, turnFraction * GROUND_ANGLE, duty_turn=duty_turn)

		
		if ((frame % 7) == 0):
			timer = time()
			# Drawing
			screen.fill(WHITE)
			renderText("t = %.2f" % t, -80, 60)
			renderText("T = %.2f" % T, -80, 90)
			renderText("DC = %.2f" % DUTY_CYCLE, -80, 120)
			renderText("GA = %.1f" % GROUND_ANGLE, -80, 150)

			if standing and not walking:
				stateText = "Standing up."
			elif walking and tapMode:
				stateText = "Walking forward."
			elif walking and tapModeBack:
				stateText = "Walking backward."
			elif not standing:
				stateText = "Lying down."

			renderText(stateText, 0, -100, size=40)
			#print("Time of drawing text: %f" % (time() - timer))

		# Pygame frame
		pygame.display.update()
		clock.tick(FPS)

		print("Total frame time: %fs" % (time() - timer0))

def renderText(string, x, y, size=35):

	font = pygame.font.SysFont(None, size)

	text = font.render(string, True, BLACK)
	textRect = text.get_rect()
	textRect.centerx = screen.get_rect().centerx + x
	textRect.centery = screen.get_rect().centery + y
	screen.blit(text, textRect)

def main():
	""" Starting point for the program, calls mainLoop() """

	global screen

	initialize()



	# Creates the control window
	screen = pygame.display.set_mode((400, 400))
	pygame.display.set_caption("xJüs Control Window")

	screen.fill(WHITE)
	renderText('Welcome to xJus', 0, 0)

	# Object that maintains a constant FPS
	clock = pygame.time.Clock()

	mainLoop(clock, screen)

	deinitialize()
	if(PLOT_ANALYSIS):
		xjusAnalysis.plotAll()
	return

# Call the main function when script is executed
if __name__ == "__main__":
    main()
