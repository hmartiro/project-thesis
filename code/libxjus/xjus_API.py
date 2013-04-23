# This file was automatically generated by SWIG (http://www.swig.org).
# Version 2.0.7
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.



from sys import version_info
if version_info >= (2,6,0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_xjus_API', [dirname(__file__)])
        except ImportError:
            import _xjus_API
            return _xjus_API
        if fp is not None:
            try:
                _mod = imp.load_module('_xjus_API', fp, pathname, description)
            finally:
                fp.close()
            return _mod
    _xjus_API = swig_import_helper()
    del swig_import_helper
else:
    import _xjus_API
del version_info
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError(name)

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0



def openDevices():
  return _xjus_API.openDevices()
openDevices = _xjus_API.openDevices

def closeDevices():
  return _xjus_API.closeDevices()
closeDevices = _xjus_API.closeDevices

def clearFault(*args):
  return _xjus_API.clearFault(*args)
clearFault = _xjus_API.clearFault

def getErrorCode():
  return _xjus_API.getErrorCode()
getErrorCode = _xjus_API.getErrorCode

def getState(*args):
  return _xjus_API.getState(*args)
getState = _xjus_API.getState

def getPositionRegulatorGain(*args):
  return _xjus_API.getPositionRegulatorGain(*args)
getPositionRegulatorGain = _xjus_API.getPositionRegulatorGain

def printPositionRegulatorGain(*args):
  return _xjus_API.printPositionRegulatorGain(*args)
printPositionRegulatorGain = _xjus_API.printPositionRegulatorGain

def setPositionRegulatorGain(*args):
  return _xjus_API.setPositionRegulatorGain(*args)
setPositionRegulatorGain = _xjus_API.setPositionRegulatorGain

def getPositionRegulatorFeedForward(*args):
  return _xjus_API.getPositionRegulatorFeedForward(*args)
getPositionRegulatorFeedForward = _xjus_API.getPositionRegulatorFeedForward

def setPositionRegulatorFeedForward(*args):
  return _xjus_API.setPositionRegulatorFeedForward(*args)
setPositionRegulatorFeedForward = _xjus_API.setPositionRegulatorFeedForward

def enable(*args):
  return _xjus_API.enable(*args)
enable = _xjus_API.enable

def disable(*args):
  return _xjus_API.disable(*args)
disable = _xjus_API.disable

def profilePositionMode(*args):
  return _xjus_API.profilePositionMode(*args)
profilePositionMode = _xjus_API.profilePositionMode

def setPositionProfile(*args):
  return _xjus_API.setPositionProfile(*args)
setPositionProfile = _xjus_API.setPositionProfile

def moveRelative(*args):
  return _xjus_API.moveRelative(*args)
moveRelative = _xjus_API.moveRelative

def moveAbsolute(*args):
  return _xjus_API.moveAbsolute(*args)
moveAbsolute = _xjus_API.moveAbsolute

def getTargetPosition(*args):
  return _xjus_API.getTargetPosition(*args)
getTargetPosition = _xjus_API.getTargetPosition

def interpolationMode(*args):
  return _xjus_API.interpolationMode(*args)
interpolationMode = _xjus_API.interpolationMode

def getFreeBufferSize(*args):
  return _xjus_API.getFreeBufferSize(*args)
getFreeBufferSize = _xjus_API.getFreeBufferSize

def addPVT(*args):
  return _xjus_API.addPVT(*args)
addPVT = _xjus_API.addPVT

def addPvtFrame(*args):
  return _xjus_API.addPvtFrame(*args)
addPvtFrame = _xjus_API.addPvtFrame

def startIPM(*args):
  return _xjus_API.startIPM(*args)
startIPM = _xjus_API.startIPM

def stopIPM(*args):
  return _xjus_API.stopIPM(*args)
stopIPM = _xjus_API.stopIPM

def printIpmStatus(*args):
  return _xjus_API.printIpmStatus(*args)
printIpmStatus = _xjus_API.printIpmStatus

def clearIpmBuffer(*args):
  return _xjus_API.clearIpmBuffer(*args)
clearIpmBuffer = _xjus_API.clearIpmBuffer

def setMaxFollowingError(*args):
  return _xjus_API.setMaxFollowingError(*args)
setMaxFollowingError = _xjus_API.setMaxFollowingError

def setMaxVelocity(*args):
  return _xjus_API.setMaxVelocity(*args)
setMaxVelocity = _xjus_API.setMaxVelocity

def setMaxAcceleration(*args):
  return _xjus_API.setMaxAcceleration(*args)
setMaxAcceleration = _xjus_API.setMaxAcceleration

def getMaxFollowingError(*args):
  return _xjus_API.getMaxFollowingError(*args)
getMaxFollowingError = _xjus_API.getMaxFollowingError

def getMaxVelocity(*args):
  return _xjus_API.getMaxVelocity(*args)
getMaxVelocity = _xjus_API.getMaxVelocity

def getMaxAcceleration(*args):
  return _xjus_API.getMaxAcceleration(*args)
getMaxAcceleration = _xjus_API.getMaxAcceleration

def homingMode(*args):
  return _xjus_API.homingMode(*args)
homingMode = _xjus_API.homingMode

def zeroPosition(*args):
  return _xjus_API.zeroPosition(*args)
zeroPosition = _xjus_API.zeroPosition

def getPosition(*args):
  return _xjus_API.getPosition(*args)
getPosition = _xjus_API.getPosition

def isFinished(*args):
  return _xjus_API.isFinished(*args)
isFinished = _xjus_API.isFinished

def getNodeAvgCurrent(*args):
  return _xjus_API.getNodeAvgCurrent(*args)
getNodeAvgCurrent = _xjus_API.getNodeAvgCurrent

def printError():
  return _xjus_API.printError()
printError = _xjus_API.printError
# This file is compatible with both classic and new-style classes.


