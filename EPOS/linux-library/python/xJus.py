#!/usr/bin/python
################################################
# xJus Control script
################################################

from ctypes import *
from time import sleep, time
import pygame
from pygame import key
from pygame.locals import *

# Import the C++ library
xjus = CDLL('/home/hayk/workspace/libxjus/Library/libxjus.so')

# List of nodes
nodes = [FL, FR] = [1, 2]

# Refresh rate
FPS = 40

def initialize():
	xjus.openDevice()
	xjus.clearAllFaults()
	xjus.enableAll()
	pygame.init()
	

def deinitialize():
	xjus.disableAll()
	xjus.closeDevice()
	pygame.quit()

def main():

	initialize()
	
	clock = pygame.time.Clock()
	surface = pygame.display.set_mode((400, 400))
	surface.fill(pygame.Color(255, 255, 255))

	while True:
		
		# Key press polling, once per frame
		keystate = key.get_pressed()
		if keystate[K_RIGHT]:
			print "right pressed!"

		# Key press events, once per key
		for event in pygame.event.get():
			if event.type == KEYDOWN:
				if event.key == K_RIGHT:
					print "RIGHT ARROW!"
				elif event.key == K_LEFT:
					print "LEFT_ARROW!"

				# Stand on spacebar
				elif event.key == K_SPACE:
					print "Stand up!"

				# Exit on escape
				elif event.key == K_ESCAPE:
					deinitialize()
					exit()

		# Pygame frame
		pygame.display.update()
		clock.tick(FPS)

	# In case the loop breaks
	raw_input('Press enter to exit...')
	deinitialize()

# Call the main function when script is executed
if __name__ == "__main__":
    main()
