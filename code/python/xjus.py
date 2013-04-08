#!/usr/bin/python
# -*- coding: utf-8 -*-
################################################
# xJus Control script
################################################

import sys, os
from os.path import expanduser

from time import sleep, time
from ctypes import *
from math import *
import numpy as np
import matplotlib.pyplot as plt
import itertools

import pygame
from pygame import key, draw
from pygame.locals import *

from xjus_trajectory import getTheta, getThetaDot

# Import libxjus, the wrapper library for libEposCmd.so
libxjus_dir = expanduser("~") + '/project-thesis/code/definition-files/libxjus.so'
xjus = CDLL(libxjus_dir)

#################################################
# Editable Parameters
#################################################
T = 1.5          # Trajectory period
dt = 90          # IPM time step (ms)
baseThetaG = 45. # Ground contact angle
FPS = 100        # PyGame refresh rate

# Is the robot mounted in the air?
MOUNTED = False

if MOUNTED:
	standAngle = 25.  # Mounted standing angle
else:
	standAngle = 145. # Standing angle

# How many periods the walk is for
walkPeriods = 2

# Fraction of base ground angle modified for turning
turning = 0.4

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

def initialize():
	""" Initializes xjus and pygame """

	pygame.init()

	print "Opening connection to device..."
	xjus.openDevice()

	print "Clearing faults and enabling nodes..."
	for node in nodes:
		xjus.clearFault(node)

		if xjus.getErrorCode() is 872415239 or 10000003:
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
		p_start = tripodSign[node] * degToPos(getTheta(t0[node], T, thetaG)) - offsetPos
		move(node, p_start, absolute=True)
		#print("node: %d, p_start: %d, thetaG: %f" % (node, p_start, thetaG))

	wait()

	for node in nodes:
		xjus.interpolationMode(node)

	# Start time is half of dt
	t = (dt/1000.) / 2

	# fill buffer
	for i in range(20):
		addTrajectoryPoint(t, turnAngle)
		t += dt/1000.

	for node in nodes:
		xjus.startIPM(node)

	return t;

def walkFrame(t0, turnAngle=0):

	timer = time()

	t = t0
	bufferSize = [xjus.getFreeBufferSize(node) for node in nodes]
	print("time to get buffer size: %f" % (time()-timer))
	#bufferHas10 = [xjus.getFreeBufferSize(node) > 9 for node in nodes]
	
	#for node in nodes:
	#	print("node: %d, position: %d" % (node, xjus.getPosition(node)))

	chunkSize = 10
	
	if len([b for b in bufferSize if b >= chunkSize]) == len(nodes):
		print("Adding %d points!" % chunkSize)

		tArray = []
		for i in range(chunkSize):
			tArray.append(t)
			t += dt/1000.

		print("time array: %s" % tArray)
		timer = time()
		addTrajectoryArray(tArray, turnAngle)
		print("time of addTrajectoryArray call: %f" % (time()-timer))
		# for _ in itertools.repeat(None, chunkSize):
		# 	addTrajectoryPoint(t, turnAngle)
		# 	t += dt/1000.

	

	print("t = %f, buffer: %s" % (t, bufferSize))

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

	t = startContinuousWalk(turnAngle)

	while t < tTotal:
		t = walkFrame(t, turnAngle)

	stopContinuousWalk(t, turnAngle)

def addTrajectoryArray(tArray, turnAngle=0):
	"""
	Calculates the array of PVT points for the given times and 
	turnAngle and adds them to the IPM buffer.
	"""
	nA = []
	pA = []
	vA = []
	tA = []

	for t in tArray:
		for node in nodes:

			thetaG = baseThetaG + sign[node] * turnAngle
			offsetPos = int(round(thetaG/2 * (REV/360)))
			p = degToPos(getTheta(t + t0[node], T, thetaG)) - offsetPos
			v = int(round(getThetaDot(t + t0[node], T, thetaG) * ANG_VEL_TO_RPM))

			nA.append(node)
			pA.append(p)
			vA.append(v)
			tA.append(dt)
			#print("node: %d, P: %d, V: %d, T: %d" % (node, p, v, dt))

	timer = time()
	addPvtAll(nA, pA, vA, tA)
	print("time of addPvtAll call: %f" % (time() - timer))
def addTrajectoryPoint(t, turnAngle=0, end=False):
	"""
	Calculates a PVT point for each node at time t and adds 
	it to the IPM buffer.
	"""
	nA = []
	pA = []
	vA = []
	tA = []

	for node in nodes:

		thetaG = baseThetaG + sign[node] * turnAngle
		offsetPos = int(round(thetaG/2 * (REV/360)))
		p = degToPos(getTheta(t + t0[node], T, thetaG)) - offsetPos
		v = int(round(getThetaDot(t + t0[node], T, thetaG) * ANG_VEL_TO_RPM))

		nA.append(node)
		pA.append(p)
		if end:
			vA.append(0)
			tA.append(0)
		else:
			vA.append(v)
			tA.append(dt)
		#print("node: %d, P: %d, V: %d, T: %d" % (node, p, v, dt))

	addPvtAll(nA, pA, vA, tA)

def addPvtAll(nodes, positions, velocities, times):
	""" Sends the given PVT points for each node to the controller. """

	N = len(nodes)

	for i in range(N):
		node = nodes[i]
		positions[i] *= sign[node]
		velocities[i] *= sign[node]

	n = (c_ushort * N)(*nodes)
	p = (c_long * N)(*map(int, map(round, positions)))
	v = (c_long * N)(*map(int, map(round, velocities)))
	t = (c_ubyte * N)(*times)

	#for i in range(N):
	#	print("n: %d, p: %d, v: %d, t: %d" % (n[i], p[i], v[i], t[i]))

	#xjus.addPvtAll.argtypes = [c_int, (c_ushort * N), (c_long * N), (c_long * N), (c_ubyte * N)]
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
			sleep(0.010)

def mainLoop(clock, surface):
	"""
	Represents the main control loop, where key events are
	processed and high-level routines activated.
	"""

	# IPM time variable
	t = 0

	timer = time()

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

				# Toggle continuous walking
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

				elif event.key == K_w:
					if standing and not walking:
						periods = float(raw_input('Walk forward for how many periods? '))
						walk(periods * T)
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
				timer = time()
				t = walkFrame(t, turnAngle)
				print("Time of walkFrame() call: %f" % (time()-timer))
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
