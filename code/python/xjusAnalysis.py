#!/usr/bin/env python
""" This utility returns current and acceleration characteristics of an xJus trial run """



#Basic imports
from ctypes import *
import sys

#Graphic processing
import numpy as np
import matplotlib.pyplot as plt
import math

#Accelerometer specific imports
from Phidgets.Phidget import Phidget
from Phidgets.PhidgetException import PhidgetErrorCodes, PhidgetException
from Phidgets.Events.Events import SpatialDataEventArgs, AttachEventArgs, DetachEventArgs, ErrorEventArgs
from Phidgets.Devices.Spatial import Spatial, SpatialEventData, TimeSpan

# xJus 
sys.path.insert(0, "/home/xjus/project-thesis/code/libxjus")
import xjus_API as xjus

orientation = True

def startAccel(trialId, graph):
    global accelFilename
    global outPutTxt_a
    global orientation
    global fA
    global totalTimeElapsed
    global totalSamplesTaken_a
    global totalAbsZAccel 
    global totalAVec
    global totalAvgAVec

    totalAvgAVec = 0;
    totalAVec = 0;
    totalAbsZAccel = 0;
    totalSamplesTaken_a = 0;
    totalTimeElapsed = 0;

    # Setutp graphing ability
    outPutTxt_a = graph
    orientation = True # True corresponds to "right-side up"

    if graph:
        accelFilename = str(trialId) + "_accelOutput.txt"
        fA = open(accelFilename, 'w')  #WRITES OVER PREVIOUS DATA
        header = "#Trial: " + str(trialId) + "  (x[g], y[g], z[g], 3D A_Vec, samples) \n"
        fA.write(header)

    #Create an accelerometer object
    try:
        global spatial
        spatial = Spatial()
    except RuntimeError as e:
        print("Runtime Exception: %s" % e.details)
        print("Exiting....")

        exit(1)

    try:
        spatial.setOnAttachHandler(SpatialAttached)
        spatial.setOnDetachHandler(SpatialDetached)
        spatial.setOnErrorhandler(SpatialError)
        spatial.setOnSpatialDataHandler(SpatialData)
    except PhidgetException as e:
        print("Phidget Exception %i: %s" % (e.code, e.details))
        print("Exiting....")
        exit(1)

    print("Opening phidget object....")

    try:
        spatial.openPhidget()
    except PhidgetException as e:
        print("Phidget Exception %i: %s" % (e.code, e.details))
        print("Exiting....")
        exit(1)

    print("Waiting for attach....")

    try:
        spatial.waitForAttach(10000)
    except PhidgetException as e:
        print("Phidget Exception %i: %s" % (e.code, e.details))
        try:
            spatial.closePhidget()
        except PhidgetException as e:
            print("Phidget Exception %i: %s" % (e.code, e.details))
            print("Exiting....")
            exit(1)
        print("Exiting....")
        exit(1)
    else:
        spatial.setDataRate(24)  ############### Sample rate is 24ms averaged
        #DisplayDeviceInfo()

#Information Display Function
#def DisplayDeviceInfo():
    #print("|- %8s -|- %30s -|- %10d -|- %8d -|" % (spatial.isAttached(), spatial.getDeviceName(), spatial.getSerialNum(), spatial.getDeviceVersion()))

#Event Handler Callback Functions
def SpatialAttached(e):
    attached = e.device
    print("Spatial %i Attached!" % (attached.getSerialNum()))

def SpatialDetached(e):
    detached = e.device
    print("Spatial %i Detached!" % (detached.getSerialNum()))

def SpatialError(e):
    try:
        source = e.device
        print("Spatial %i: Phidget Error %i: %s" % (source.getSerialNum(), e.eCode, e.description))
    except PhidgetException as e:
        print("Phidget Exception %i: %s" % (e.code, e.details))

def SpatialData(e):
    
    global totalSamplesTaken_a
    totalSamplesTaken_a = totalSamplesTaken_a + 1.0;
    global orientation
    global a_z
    global totalAbsZAccel
    global avgAbsZAccel
    global totalAVec
    global totalAvgAVec

    source = e.device
    for index, spatialData in enumerate(e.spatialData):

        if outPutTxt_a: 
            # Write a full value out to file for graphical processing
            global totalTimeElapsed
            totalTimeElapsed += spatialData.Timestamp.microSeconds # In u seconds

            # Set the current average acceleration
            a_x = spatialData.Acceleration[0]
            a_y = spatialData.Acceleration[1]
            a_z = spatialData.Acceleration[2]

            a_vec = math.sqrt( (a_x*a_x*1.0) + (a_y*a_y*1.0) + (a_z*a_z*1.0) )

            textValues = str(spatialData.Acceleration[0]) + ", " + str(spatialData.Acceleration[1]) + ", " + str(spatialData.Acceleration[2]) + ", " + str(a_vec) + ", " + str(totalSamplesTaken_a)
            fA.write(textValues + '\n')



        if  a_z > 0: # Positive Z axis
            orientation = True
        else:
            orientation = False

 

    if a_z < 0:
        a_z = -a_z

    

    totalAVec += a_vec

    totalAbsZAccel += a_z

    avgAbsZAccel = totalAbsZAccel / totalSamplesTaken_a

    totalAvgAVec = totalAVec / totalSamplesTaken_a


#Returns avg deviation from gravity
def getAvgAbsZAccel():

    localCopyAvgAbsZAccel = avgAbsZAccel
    localCopyAvgAbsZAccel = localCopyAvgAbsZAccel - 1

    if localCopyAvgAbsZAccel < 0:
        localCopyAvgAbsZAccel = -localCopyAvgAbsZAccel

    return localCopyAvgAbsZAccel

def getAvgAccel():

    localCopy = totalAvgAVec

    localCopy = localCopy - 1.0

    if (localCopy < 0):
        localCopy = -localCopy
  
    return localCopy

def getAbsZAccel():
    # Take the absolute acceleration
   
    localCopyA_z =  a_z
    if localCopyA_z < 0:
       localCopyA_z = - localCopyA_z
    # Remove gravity
    # a_z = a_z - 1

    return localCopyA_z

def getOrientation():
    return orientation

def endAccel(graph):

    try:
        spatial.closePhidget()
    except PhidgetException as e:
        print("Phidget Exception %i: %s" % (e.code, e.details))
        print("Exiting....")
        exit(1)

    if graph:
        fA.close()

 



def startAvgCurrent(trialId, graph):

    global totalSamplesTaken_c 
    global totalCurrent
    global outPutTxt_c


    outPutTxt_c = graph
    totalSamplesTaken_c = 0
    totalCurrent = 0

    if graph:
        global currentFilename
        currentFilename = str(trialId) + "_currentOutput.txt"
        global fC
        fC = open(currentFilename, 'w')  #WRITES OVER PREVIOUS DATA
        header = "#Trial: " + str(trialId) + "  Nodes(1-6) mA, sample number \n"
        fC.write(header)

def startAvgVelocity(trialId):

    global totalSamplesTaken_v
    totalSamplesTaken_v = 0

    global velocityFilename
    velocityFilename = str(trialId) + "_veclocityOutput.txt"
    global fV
    fV = open(currentFilename, 'w')  #WRITES OVER PREVIOUS DATA
    header = "#Trial: " + str(trialId) + "  Nodes(1-6) mA, rpm \n"
    fV.write(header)


def sampleAvgVelocity():

    global totalSamplesTaken_v 
    totalSamplesTaken_v = totalSamplesTaken_v + 1.0

    for node in range(1,7):
        measuredVelocity = xjus.getVelocityAveraged(node)
            
        if (node == 1):
            output = str(measuredVelocity) + ", "
        else:
            output += str(measuredVelocity) + ", "

            
    fV.write(output + str(totalSamplesTaken_v) + '\n')

def endAvgVelocity():
    fV.close()



def sampleAvgCurrent():

    global totalSamplesTaken_c
    totalSamplesTaken_c = totalSamplesTaken_c + 1.0
    global totalCurrent
    global AvgCurrent

    if (outPutTxt_c):
        for node in range(1, 7):
            measuredCurrent = xjus.getNodeAvgCurrent(node)
            
            if (node == 1):
                output = str(measuredCurrent) + ", "
                totalCurrent += measuredCurrent
            else:
                output += str(measuredCurrent) + ", "
                totalCurrent += measuredCurrent

            
        fC.write(output + str(totalSamplesTaken_c) + '\n')

        # Update Avg Current
        AvgCurrent = totalCurrent / totalSamplesTaken_c

    else: 

        for node in range(1, 6):
            totalCurrent += xjus.getNodeAvgCurrent(node)

    # Update Avg Current
    AvgCurrent = totalCurrent / totalSamplesTaken_c

def getAvgCurrent():

    if (outPutTxt_c):
        fC.close()

    return AvgCurrent



def plotAll():

        a, b, c, d, e, f, s = np.loadtxt(currentFilename, delimiter = ",", unpack = True)

        fig = plt.figure()
        ax1 = fig.add_subplot(212)

        ax1.plot(s,a, label = "node1")
        plt.hold(True)
        plt.plot(s,b, label = "node2")
        plt.plot(s,c, label = "node3")
        plt.plot(s,d, label = "node4")
        plt.plot(s,e, label = "node5")
        plt.plot(s,f, label = "node6")
        totalCurrentArray = a+b+c+d+e+f
        plt.plot(s, totalCurrentArray, label ="total")
        plt.legend()

        plt.xlabel("Samples")
        plt.ylabel("mA")

        #plt.show()


        # User inpute 

        distance = float(raw_input("What was the trial distance[ft]?"))

        distance_m = distance * 0.3048 # convert ft to m

        string = "What was the trial time for the " + str(distance)+ " [ft] trial?"

        speed = float(raw_input(string))

        speed = (distance_m/ speed) 

        voltage = float(raw_input("What was the trial voltage [V]? "))

        specificR = (voltage*getAvgCurrent()) / speed




        x, y, z, vec, sample_a = np.loadtxt(accelFilename, delimiter = ",", unpack = True)

        vec_var = np.var(vec)


        # Convert t to seconds
        ax1 = fig.add_subplot(211)

        ax1.plot(sample_a, vec, label = "total_g")
        plt.xlabel("time [s]")
        plt.ylabel("Total Acceleration [g]")

        #plt.hold(True)
        #plt.plot(t,y, label = "y")
        #plt.plot(t,z, label = "z")
        plt.title("AvgAccelMag Less Gravity: %.4f, AccelMagVar: %.4f, AvgCurrent: %.4f, (Current)(Accel)Samples: (%.4f)(%.4f)" % (getAvgAccel(), vec_var, getAvgCurrent(),totalSamplesTaken_c,totalSamplesTaken_a) )
        plt.show()

        trial = np.loadtxt("trialNumKeeper.txt")

        trial = trial + 1


        fR = open("trialResults.txt", 'a')
        outputInfo = "[" + str(trial) +"], " + str(getAvgAccel())  + ", " + str(vec_var) + ", " + str(getAvgCurrent()) + ", " + str(speed) + ", " + str(voltage) + ", " + str(specificR) + ", " + str(totalSamplesTaken_c) + ", " + str(totalSamplesTaken_a) + "\n"

        outputInfo = ("[%d], %1.5f, %0.5f, %5.0f, %1.2f, %2.2f, %5.1f, %5d, %5d \n" % (trial, getAvgAccel(), vec_var, getAvgCurrent(), speed, voltage, specificR, totalSamplesTaken_c, totalSamplesTaken_a))

        fT = open("trialNumKeeper.txt", 'w')
        fT.write(str(trial))
        fT.close()

        fR.write("[%d], %1.6f, %0.6f, %5.0f, %1.2f, %2.2f, %5.1f, %5d, %5d \n" % (trial, getAvgAccel(), vec_var, getAvgCurrent(), speed, voltage, specificR, totalSamplesTaken_c, totalSamplesTaken_a))
        fR.close()

