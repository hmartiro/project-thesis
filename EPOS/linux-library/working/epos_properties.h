/*
 * EposSettings.h
 *
 *  Created on: Dec 16, 2010
 *      Author: DAWS
 *      Copyright: maxonmotor ag (c) 2010
 */

#ifndef EPOSSETTINGS_H_
#define EPOSSETTINGS_H_

#include <QObject>

class epos_settings : public QObject
{
	Q_OBJECT

public:
	epos_settings(QObject* parent=0);
	virtual ~epos_settings();

public:
	epos_settings& operator=(const epos_settings&);

public:
	QString getDeviceName();
	void setDeviceName(QString);
	QString getProtocolStackName();
	void setProtocolStackName(QString);
	QString getInterfaceName();
	void setInterfaceName(QString);
	QString getPortName();
	void setPortName(QString);
	unsigned long getBaudrate();
	void setBaudrate(QString);
	unsigned long & getTimeout();
	void setTimeout(QString);

protected:
	unsigned long	timeout_;
	unsigned long	baudrate_;
	QString deviceName_;
	QString protocolStackName_;
	QString interfaceName_;
	QString portName_;
};

#endif /* EPOSSETTINGS_H_ */
