#!/usr/bin/python
# -*- coding: utf-8 -*-
################################################
# xJus Control script
################################################

import sys, os
from time import sleep, time
from ctypes import *
from math import *

import pygame
from pygame import key, draw
from pygame.locals import *

# Import our C++ library
xjus = CDLL('/home/hayk/workspace/libxjus/Library/libxjus.so')

# List of nodes
nodes = [FL, FR] = [1, 2]
sign = {FL: 1, FR: -1}

# Refresh rate
FPS = 50

# Colors for drawing
WHITE = (255, 255, 255)
BLACK = (  0,   0,   0)
RED   = (255,   0,   0)
GREEN = (  0, 255,   0)
BLUE  = (  0,   0, 255)

standing = False

REV = 59720;

dt = 50;
T = 1.5;

baseThetaG = 50.
standAngle = 110.
offsetPos = int(round(baseThetaG/2 * (REV/360)))

t0 = 0
M = 10
GEAR_RATIO = 729.0/25.0
ANG_TO_QC = (512.0*4)/(2*pi) * GEAR_RATIO
ANG_VEL_TO_RPM = (60.0)/(360) * GEAR_RATIO

def initialize():
	""" Initializes xjus and pygame """

	pygame.init()

	xjus.openDevice()
	for node in nodes:
		xjus.clearFault(node)
		xjus.enable(node)
	
def deinitialize():
	""" Deconstructs xjus and pygame """

	for node in nodes:
		xjus.disable(node)
	xjus.closeDevice()
	
	pygame.quit()

def keyDown(k):
	""" Returns True if the given k is currently down """

	return key.get_pressed()[k]

def degToPos(angle):
	""" Returns the number of encoder ticks for the given angle """

	return int(round(REV*(angle/360.)));

def stand():
	""" Raises the chassis into a standing position """

	global standing
	standing = True

 	for node in nodes:
		xjus.setPositionProfile(node, 500, 5000, 5000)

	for node in nodes:
		xjus.moveRelative(node, sign[node] * degToPos(standAngle))

	wait()
	
	for node in nodes:
		xjus.zeroPosition(node)
		xjus.setPositionProfile(node, 1000, 5000, 5000)

def sit():
	""" Lowers the chassis to the ground """

	global standing
	standing = False

 	for node in nodes:
		xjus.setPositionProfile(node, 500, 5000, 5000)

	for node in nodes:
		xjus.moveRelative(node, -sign[node] * degToPos(standAngle))

def walk(tTotal, turnAngle=0):
	""" Forward locomotion of the robot """

	thetaGL = baseThetaG - turnAngle
	thetaGR = baseThetaG + turnAngle

	for node in nodes:
		xjus.profilePositionMode(node)

	print "Moving to start position..."
	xjus.moveAbsolute(FL, +degToPos(getTheta(T/2, thetaGL)) - offsetPos)
	xjus.moveAbsolute(FR, -degToPos(getTheta(  0, thetaGR)) + offsetPos)
	wait();
	print("pL: %d, pR: %d" % (xjus.getPosition(FL), xjus.getPosition(FR)))

	for node in nodes:
		xjus.interpolationMode(node)

	# Start time is half of dt
	t = (dt/1000.) / 2

	# Add 10 initial points
	for i in range(10):
		addTrajectoryPoint(t, turnAngle)
		t += dt/1000.

	for node in nodes:
		xjus.startIPM(node)

	while t < tTotal:

		bufferNotFull = [xjus.getBufferSize(node) > 0 for node in nodes]
		if all(bufferNotFull):
			addTrajectoryPoint(t, turnAngle)
			t += dt/1000.

	sleep(2 * dt / 1000.)

	#xjus.addPVT(FL, +getTheta(t+T/2, thetaGL)-offsetPos, 0, 0);
	#xjus.addPVT(FR, -getTheta(t,     thetaGR)+offsetPos, 0, 0);
	
	for node in nodes:
		xjus.printIpmStatus(node)

	wait()
	for node in nodes:
		xjus.stopIPM(node)

def addTrajectoryPoint(t, turnAngle=0):
	
	thetaGL = baseThetaG - turnAngle
	thetaGR = baseThetaG + turnAngle

	pos_FL = +degToPos(getTheta(T/2+t, thetaGL)) - offsetPos
	pos_FR = -degToPos(getTheta(    t, thetaGR)) + offsetPos
	vel_FL = +int(round(getThetaDot(t+T/2, thetaGL) * ANG_VEL_TO_RPM))
	vel_FR = -int(round(getThetaDot(t,     thetaGR) * ANG_VEL_TO_RPM))
	print("pL: %d, pR, %d, vL: %d, vR: %d" % (pos_FL, pos_FR, vel_FL, vel_FR))

	xjus.addPVT(FL, pos_FL, vel_FL, dt)
	xjus.addPVT(FR, pos_FR, vel_FR, dt)

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

# long getThetaDot(double t, double thetaG) {
# 	double thetaDot = (2*pi/T);
# 	for (int m = 1; m <= M; m++) {
# 		double B = 8*(thetaG-pi)/((2*m-1)*pi*T);
# 		thetaDot += B*sin(2*pi*(2*m-1)*t/T);
# 	}
# 	return round2(thetaDot * angVelToRPM);
# }
def wait():
	for node in nodes:
		while not xjus.isFinished(node):
			sleep(0.010)

def mainLoop(clock, surface):
	"""
	Represents the main control loop, where key events are
	processed and high-level routines activated.
	"""

	while True:

		# Processing all events for the frame
		for event in pygame.event.get():

			# Key down events
			if event.type == KEYDOWN:

				# Tooggle stand on spacebar
				if event.key == K_SPACE:
					if standing:
						print "Go to sitting position."
						sit()
					else:
						print "Go to standing position."
						stand()

				# Toggle walking
				elif event.key == K_UP:
					if standing:
						print "Start walking forward!"
						walk(T*2)
					else:
						print "Must stand first!"

				elif event.key == K_RIGHT:
					print "RIGHT ARROW!"
				elif event.key == K_LEFT:
					print "LEFT_ARROW!"
				elif event.key == K_DOWN:
					print "DOWN ARROW!"

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
