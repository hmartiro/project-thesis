#!/usr/bin/python
# -*- coding: utf-8 -*-
################################################
# xJus Simulation Control script
################################################

import bge
import math

from xjus_trajectory import getTheta, getThetaDot
from xjus_sim_parameters import *

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

################################################
# BGE Object Definitions
################################################

# Top-level BGE objects
cont = bge.logic.getCurrentController()
scene = bge.logic.getCurrentScene()

chassis_F = scene.objects["Chassis_F"]
chassis_M = scene.objects["Chassis_M"]
chassis_B = scene.objects["Chassis_B"]
chassis = [chassis_F, chassis_M, chassis_B]

leg_FL = scene.objects["Leg_FL"]
leg_FR = scene.objects["Leg_FR"]
leg_ML = scene.objects["Leg_ML"]
leg_MR = scene.objects["Leg_MR"]
leg_BL = scene.objects["Leg_BL"]
leg_BR = scene.objects["Leg_BR"]
legs = [leg_FL, leg_FR, leg_ML, leg_MR, leg_BL, leg_BR]

motor_FL = scene.objects['Motor_FL']
motor_FR = scene.objects['Motor_FR']
motor_ML = scene.objects['Motor_ML']
motor_MR = scene.objects['Motor_MR']
motor_BL = scene.objects['Motor_BL']
motor_BR = scene.objects['Motor_BR']
motors = [motor_FL, motor_FR, motor_ML, motor_MR, motor_BL, motor_BR]

motor_FL2 = scene.objects['Motor_FL2']
motor_FR2 = scene.objects['Motor_FR2']
motor_ML2 = scene.objects['Motor_ML2']
motor_MR2 = scene.objects['Motor_MR2']
motor_BL2 = scene.objects['Motor_BL2']
motor_BR2 = scene.objects['Motor_BR2']
motors2 = [motor_FL2, motor_FR2, motor_ML2, motor_MR2, motor_BL2, motor_BR2]

floor = scene.objects['Tile_Floor']

###############################################
## Leg-specific variables
###############################################

for leg in legs:
    
    leg['angle'] = 180.0
    leg['rev']   = 0
    leg['error'] = 0.0
    
    leg['Pin'] = 0.0
    leg['Pout'] = 0.0
    
    leg['angles'] = []
    leg['targets'] = []
    leg['w'] = []
    
###############################################
## Chassis-specific variables
###############################################

for c in chassis:
    c['w'] = []

###############################################
## Global simulation variables
###############################################

t = 0.
active = False

##################################################
## Top level loop function
##################################################

def loop_frame():
    ''' Top level function that is called once per frame 
        and handles locomotion and custom dynamics 
    '''
    
    global active, t
    
    print("t = %f" % (t))
    
    # leg encoders
    for leg in legs:
        detectLegPosition(leg)
    
    # reset analytics
    for leg in legs:
        leg['Pin'] = 0
        leg['Pout'] = 0
    
    if keyPressed(bge.events.SPACEKEY):
        active = not active
        
        t = 0.
        for leg in legs:
            leg['rev'] = 0
            leg['error'] = 0
       
    # resets the time and leg revolutions if a routine was just started
    if (
        keyPressed(bge.events.UPARROWKEY) or
        keyPressed(bge.events.DOWNARROWKEY)
       ):
        t = T/4.
        
        for leg in legs:
            leg['rev'] = 0
            leg['error'] = 0
    
    # locomotion routines
    if keyDown(bge.events.UPARROWKEY):
        
        if keyDown(bge.events.RIGHTARROWKEY):
            goForward(+TURN_FRACTION * GROUND_ANGLE)
        elif keyDown(bge.events.LEFTARROWKEY):
            goForward(-TURN_FRACTION * GROUND_ANGLE)
        else:
            goForward(0)
            
    elif keyDown(bge.events.DOWNARROWKEY):
        
        if keyDown(bge.events.RIGHTARROWKEY):
            goBackward(+TURN_FRACTION * GROUND_ANGLE)
        elif keyDown(bge.events.LEFTARROWKEY):
            goBackward(-TURN_FRACTION * GROUND_ANGLE)
        else:
            goBackward(0)
    
    elif active:
        standingPosition()
        for leg in legs:
            leg['rev'] = 0
    
    else:
        inactive()
    
    # compliance in the spine
    torsionalSpringX(chassis_B, chassis_M,  1, -1)
    torsionalSpringX(chassis_F, chassis_M,  1, -1)
    torsionalSpringY(chassis_B, chassis_M,  1, -1)
    torsionalSpringY(chassis_F, chassis_M,  1, -1)
    
    # Increment time
    t += 1.0/FPS


###########################################################
## Utility functions
###########################################################

def keyPressed(key):
    ''' Returns True if the key was just pressed. '''
    
    keyEvents = bge.logic.keyboard.events
    if keyEvents[key] == bge.logic.KX_INPUT_JUST_ACTIVATED:
        return True
    else:
        return False

def keyDown(key):
    ''' Returns true if the key is currently down. '''
    
    keyEvents = bge.logic.keyboard.events
    if keyEvents[key] == bge.logic.KX_INPUT_ACTIVE:
        return True
    elif keyPressed(key):
        return True
    else:
        return False

###########################################################
## Functions that find relative angles and velocities
###########################################################

def getAngleX(bodyA, bodyB):
    ''' Returns the relative angle in the x-direction
        between bodies A and B.
    '''
    angleA = bodyA.localOrientation.to_euler().x * (180/math.pi)
    angleB = bodyB.localOrientation.to_euler().x * (180/math.pi)
    return angleA - angleB

def getAngleY(bodyA, bodyB):
    ''' Returns the relative angle in the y-direction
        between bodies A and B.
    '''
    angleA = bodyA.localOrientation.to_euler().y * (180/math.pi)
    angleB = bodyB.localOrientation.to_euler().y * (180/math.pi)
    return angleA - angleB

def getAngleZ(bodyA, bodyB):
    ''' Returns the relative angle in the z-direction
        between bodies A and B.
    '''
    angleA = bodyA.localOrientation.to_euler().z * (180/math.pi)
    angleB = bodyB.localOrientation.to_euler().z * (180/math.pi)
    return angleA - angleB

def getAngVelX(bodyA, bodyB):
    ''' Returns the relative angular velocity in the local
        x-direction between bodies A and B.
    '''
    angVelA = bodyA.getAngularVelocity(True).x * (180/math.pi) 
    angVelB = bodyB.getAngularVelocity(True).x * (180/math.pi)
    return angVelA - angVelB

def getAngVelY(bodyA, bodyB):
    ''' Returns the relative angular velocity in the local
        y-direction between bodies A and B.
    '''
    angVelA = bodyA.getAngularVelocity(True).y * (180/math.pi) 
    angVelB = bodyB.getAngularVelocity(True).y * (180/math.pi)
    return angVelA - angVelB

def getAngVelZ(bodyA, bodyB):
    ''' Returns the relative angular velocity in the local
        z-direction between bodies A and B.
    '''
    angVelA = bodyA.getAngularVelocity(True).z * (180/math.pi) 
    angVelB = bodyB.getAngularVelocity(True).z * (180/math.pi)
    return angVelA - angVelB

###########################################################
## Leg sensing
###########################################################

def getLegAngle(leg):
    ''' Returns the given leg's relative angle to the chassis segment
    '''
    i = legs.index(leg)
    return getAngleX(leg, motors[i])

def getLegVelocity(leg):
    ''' Returns the given leg's relative angular velocity to the chassis
    '''
    i = legs.index(leg)
    return getAngVelX(leg, motors[i])

def detectLegPosition(leg):
    ''' Updates the leg's angle variable to reflect
        its current orientation (like an encoder)
    '''             
    
    # encoder ticks
    angle = leg['angle']
    
    ## Why is there a minus sign here?
    newAngle = -getLegAngle(leg) + 180
    
    if abs(angle - (newAngle + 180)) < 15:
        newAngle += 180
    elif abs(newAngle - (angle + 180)) < 15:
        newAngle -= 180
    #if False: pass
    else:
        if newAngle + 150 < angle: 
            leg['rev'] += 1
        elif newAngle - 150 > angle: 
            leg['rev'] -= 1
    
    leg['angle'] = newAngle
    
    if len(leg['angles']) >= SMOOTHING:
        leg['angles'].pop()
    leg['angles'].insert(0, newAngle + leg['rev']*360)
    
    w = getLegVelocity(leg)
    if len(leg['w']) >= SMOOTHING:
        leg['w'].pop()
    leg['w'].insert(0, w)
    
    if leg is leg_FR:
        #print("angle: %d, rev: %d, newAngle: %d, newRev: %d" % (angle, rev, newAngle, leg['rev']))
        #print(leg['angles'])
        pass

###########################################################
## Locomotion routines
###########################################################

def inactive():
    ''' Motors are not powered, but provide resistance based
        on the motor model.
    '''
    for leg in legs:
        torque = motorModel(leg, 0)
        applyLegTorque(leg, torque)

def standingPosition():
    ''' Keeps the robot fully upright
    '''
    for leg in legs:
        
        angle = leg['angle'] + leg['rev'] * 360
        
        targetAng = standAngle
        #if angle > 215: targetAng = 540
        
        if leg is leg_FR:
            #print("leg_FR. angle: %f, targetAng: %f" % (leg['angle'], targetAng))
            pass
        
        applyLegFeedback(leg, targetAng)

def goForward(turnAngle=0):
    """
    Calculates a PVT point for each node at time t and adds 
    it to the IPM buffer.
    """

    for node in nodes:

        thetaG = GROUND_ANGLE + sign[node] * turnAngle
        targetAng = getTheta(t + t0[node], T, thetaG)
        targetAng += 320 - thetaG/2
        applyLegFeedback(legs[node-1], targetAng)

def goBackward(turnAngle=0):
    """
    Calculates a PVT point for each node at time t and adds 
    it to the IPM buffer.
    """

    for node in nodes:

        thetaG = GROUND_ANGLE + sign[node] * turnAngle
        targetAng = -getTheta(t + t0[node], T, thetaG)
        targetAng += 320 + thetaG/2 + 15
        applyLegFeedback(legs[node-1], targetAng)

###########################################################
## Leg actuation
###########################################################

def applyLegFeedback(leg, targetAng):
    ''' Applies proportional control to the leg based on the
        current angle and the given target angle.
    '''
    
    if len(leg['targets']) >= SMOOTHING:
        leg['targets'].pop()
    leg['targets'].insert(0, targetAng)
    
    angle = sum(leg['angles'])/(1.0*len(leg['angles']))
    #angle = leg['angles'][0]
    
    #while targetAng - angle > +360: angle += 360
    #while targetAng - angle < -360: angle -= 360
    
    error = targetAng - angle
    
    derivative = error - leg['error']
    
    # Takes care of crazy derivative spike 
    '''
    while error > +360: 
        error -= 360
        derivative = 0
    while error < -360:
        error += 360
        derivative = 0
    '''
    Vpd = - (Kp * error + Kd * derivative)
    
    torque = motorModel(leg, Vpd)
    applyLegTorque(leg, torque)
    
    w = getLegVelocity(leg) * (math.pi/180)
    
    if leg is leg_FR: 
        #print("leg_FR. vel: %f, torque: %f" % (getLegVelocity(leg), torque))
        pass
    
    if leg is leg_FR:
        #print("ang: %d, rev: %d, angle: %d, targetAng: %d, torque: %f" % (leg['angle'], leg['rev'], angle, targetAng, torque))
        #print("time: %d, ang: %d, vel: %f, target: %d, error: %d, voltage: %d, torque %f" % (t, angle, w, targetAng, error, Vpd, torque/100))
        #print("FR_ang: %d, FR_w: %d, voltage: %d, torque %f, F_ang: %d, F_w: %d" % (angle, w, Vpd, torque/100, getAngleX(chassis_F, chassis_B), getAngVelX(chassis_F, chassis_B)))
        #print("angle: %d, target: %d, voltage: %d, torque %d" % (angle, targetAng, voltage, torque))
        pass
    
    leg['error'] = error
    
def motorModel(leg, Vpd):
    ''' Takes the given input for the motor and applies a torque
        based on the motor profile.
    '''
    
    d = Vpd / Vs
    if d > +1: d = +1
    if d < -1: d = -1
    
    w = sum(leg['w'])/(1.0*len(leg['w'])) * (math.pi/180)
    #w1 = leg['w'][0] * (math.pi/180)
    
    torque = mu * N * Kt * (d * Vs - Ks * w) / (Ra + d**2 * Ramp)
    
    leg['Pout'] = w * torque
    leg['Pin']  = leg['Pout'] / (MOTOR_EFFICIENCY * GEARBOX_EFFICIENCY * CONTROLLER_EFFICIENCY)
    
    #print("leg: %s, d: %f, angVel: %f, tau: %f, Pout: %f" % (leg, d, w, torque, leg['Pout']))
    
    return torque * 100

def applyLegTorque(leg, torque):
    ''' Applies the given torque to the given leg
    '''
    #if leg is leg_FR: print(torque)
    i = legs.index(leg)
    motors[i].applyTorque((-torque, 0, 0), True)
    motors2[i].applyTorque((+torque, 0, 0), True)

############################################################
## Analytics
############################################################

def totalPower():
    P = 0
    for leg in legs:
        if leg['Pout'] > 0:
            P += leg['Pout']
    return P

############################################################
## Compliant spine springs
############################################################

def torsionalSpringX(bodyA, bodyB, signA, signB):
    ''' Handles the dynamics of the springiness in the
        x-axis between bodyA and bodyB, with signA and 
        signB specifying positive directions.
    '''
    
    angle = getAngleX(bodyA, bodyB)
    angVel = getAngVelX(bodyA, bodyB)
    
    if len(bodyA['w']) >= SMOOTHING:
        bodyA['w'].pop()
    bodyA['w'].insert(0, angVel)
    w = sum(bodyA['w'])/(1.0*len(bodyA['w']))
    
    springTorque = - SAGGITAL_STIFFNESS * angle
    damperTorque = - SAGGITAL_DAMPING * w
    torque = springTorque + damperTorque
    
    bodyA.applyTorque((signA * torque, 0, 0), True)
    bodyB.applyTorque((signB * torque, 0, 0), True)
    
    if bodyA is chassis_F:
        #print("angle: %f, angVel: %f, spring: %d, damper: %f, torque: %d" % (angle, angVel, springTorque, damperTorque, torque/100))
        pass
    
def torsionalSpringY(bodyA, bodyB, signA, signB):
    ''' Handles the dynamics of the springiness in the
        y-axis between bodyA and bodyB, with signA and 
        signB specifying positive directions.
    '''
    
    angle = getAngleY(bodyA, bodyB)
    angVel = getAngVelY(bodyA, bodyB)
    
    springTorque = - TORSIONAL_STIFFNESS * angle
    damperTorque = - TORSIONAL_DAMPING * angVel
    torque = springTorque + damperTorque
    
    bodyA.applyTorque((0, signA * torque, 0), True)
    bodyB.applyTorque((0, signB * torque, 0), True)
    
    if bodyA is chassis_B:
        #print("angle: %f, angVel: %f, spring: %d, damper: %f, torque: %d" % (angle, angVel, springTorque, damperTorque, torque))
        pass

# Not used, we are setting yaw to zero
def torsionalSpringZ(bodyA, bodyB, signA, signB):
    ''' Handles the dynamics of the springiness in the
        z-axis between bodyA and bodyB, with signA and 
        signB specifying positive directions.
    '''

    angle = getAngleZ(bodyA, bodyB)
    angVel = getAngVelX(bodyA, bodyB)
    
    torque = - Z_STIFFNESS * angle - Z_DAMPING * angVel
    
    bodyA.applyTorque((signA * torque, 0, 0), True)
    bodyB.applyTorque((signB * torque, 0, 0), True)