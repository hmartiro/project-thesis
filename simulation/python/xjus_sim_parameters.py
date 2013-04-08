#!/usr/bin/python
################################################
# xJus Simulation Parameters
################################################

##################################################
## Design parameters to play with
##################################################

# Trajectory period
T = 1.0

# The ground contact angle (degrees)
GROUND_ANGLE = 45.0

# Fraction of the ground angle that is 
# added/subtracted for turning routines
TURN_FRACTION = 0.5

# Is the robot mounted in the air?
MOUNTED = False

if MOUNTED:
	standAngle = 180. + 25.  # Mounted standing angle
else:
	standAngle = 180. + 130. # Standing angle

# Motor PID control parameters
Kp = 0.15
Kd = 0.00

# Simulation frames per second
FPS = 60

# Flexible spine spring parameters
SAGGITAL_STIFFNESS = 50
TORSIONAL_STIFFNESS = 150
SAGGITAL_DAMPING = 0.1
TORSIONAL_DAMPING = 0.7

# Smoothing of parameters
SMOOTHING = 1

##################################################
## Plant parameters
##################################################

# Motor parameters
Vs = 22.2 # source voltage (V)
N = 729./25. # gear ratio
mu = 0.6 # motor+gear efficiency (motor * gear)
Ra = 2.36 # motor resistance (from motor wiki) (V/A)
Ramp = 0.5 # amplifier resistance (don't know!) (V/A)

Ks = 0.75 # motor speed constant (V-s)
Kt = 0.042 #0.052 # motor conversion factor (Nm/A)

CONTROLLER_EFFICIENCY = 0.90
MOTOR_EFFICIENCY = 0.86
GEARBOX_EFFICIENCY = 0.70
