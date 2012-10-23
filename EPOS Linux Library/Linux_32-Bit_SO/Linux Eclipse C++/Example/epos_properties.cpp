/*
 * EposSettings.cpp
 *
 *  Created on: Dec 16, 2010
 *      Author: DAWS
 *      Copyright: maxonmotor ag (c) 2010
 */

#include "epos_properties.h"

epos_settings::epos_settings(QObject* parent)
: QObject(parent)
{
	timeout_ = 500;
	baudrate_ = 38400;
}

epos_settings::~epos_settings()
{

}

epos_settings& epos_settings::operator=(const epos_settings& source)
{
	timeout_				= source.timeout_;
	baudrate_			= source.baudrate_;
	deviceName_			= source.deviceName_;
	protocolStackName_	= source.protocolStackName_;
	interfaceName_		= source.interfaceName_;
	portName_			= source.portName_;

	return *this;
}

QString epos_settings::getDeviceName()
{
	return deviceName_;
}

QString epos_settings::getProtocolStackName()
{
	return protocolStackName_;
}

QString epos_settings::getInterfaceName()
{
	return interfaceName_;
}

QString epos_settings::getPortName()
{
	return portName_;
}

unsigned long & epos_settings::getTimeout()
{
	return timeout_;
}

unsigned long epos_settings::getBaudrate()
{
	return baudrate_;
}

void epos_settings::setDeviceName(QString newVal)
{
	deviceName_ = newVal;
}

void epos_settings::setProtocolStackName(QString newVal)
{
	protocolStackName_ = newVal;
}

void epos_settings::setInterfaceName(QString newVal)
{
	interfaceName_ = newVal;
}

void epos_settings::setPortName(QString newVal)
{
	portName_ = newVal;
}

void epos_settings::setBaudrate(QString newVal)
{
	baudrate_ = newVal.toULong();
}

void epos_settings::setTimeout(QString newVal)
{
	timeout_ = newVal.toULong();
}
