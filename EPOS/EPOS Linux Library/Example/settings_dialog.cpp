/*
 * devicesettings.cpp
 *
 *  Created on: Dec 16, 2010
 *      Author: DAWS
 *      Copyright: maxonmotor ag (c) 2010
 */


#include "settings_dialog.h"

settings_dialog::settings_dialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	setFixedSize(394,345);

	QObject::connect(ui.pushButtonCancel, SIGNAL(clicked()), SLOT(onCancel()));
	QObject::connect(ui.pushButtonOpen, SIGNAL(clicked()), SLOT(onOpen()));

	QObject::connect(ui.comboBoxDeviceName, SIGNAL(currentIndexChanged(QString)), SLOT(onDeviceNameChange(QString)));
	QObject::connect(ui.comboBoxProtocolStackName, SIGNAL(currentIndexChanged(QString)), SLOT(onProtocolChange(QString)));
	QObject::connect(ui.comboBoxInterfaceName, SIGNAL(currentIndexChanged(QString)), SLOT(onInterfaceChange(QString)));
	QObject::connect(ui.comboBoxPortName, SIGNAL(currentIndexChanged(QString)), SLOT(onPortChange(QString)));
	QObject::connect(ui.comboBoxBaudrate, SIGNAL(currentIndexChanged(QString)), SLOT(onBaudrateChange(QString)));

	QObject::connect(ui.lineEditTimeout, SIGNAL(textChanged(QString)), SLOT(onTimeoutChanged(QString)));

	onDeviceNameChange("EPOS");

	ui.lineEditTimeout->setText(QString("%1").arg(settings_.getTimeout()));
}

settings_dialog::~settings_dialog()
{

}

void settings_dialog::onOpen()
{
	accept();
}

void settings_dialog::onCancel()
{
	reject();
}

void settings_dialog::onDeviceNameChange(QString deviceName)
{
	if( deviceName.compare("EPOS", Qt::CaseInsensitive) == 0)
	{
		ui.comboBoxProtocolStackName->clear();
		QStringList items;
		items.append("MAXON_RS232");
		ui.comboBoxProtocolStackName->addItems(items);
		ui.comboBoxProtocolStackName->setCurrentIndex(0);
	}
	else if( deviceName.compare("EPOS2", Qt::CaseInsensitive) ==0 )
	{
		ui.comboBoxProtocolStackName->clear();
		QStringList items;
		items.append("MAXON_RS232");
		items.append("MAXON SERIAL V2");
		ui.comboBoxProtocolStackName->addItems(items);
		ui.comboBoxProtocolStackName->setCurrentIndex(0);
	}

	settings_.setDeviceName(deviceName);
}

void settings_dialog::onProtocolChange(QString protocolName)
{
	if( protocolName.compare("MAXON_RS232", Qt::CaseInsensitive) == 0)
	{
		ui.comboBoxInterfaceName->clear();
		QStringList items;
		items.append("RS232");
		ui.comboBoxInterfaceName->addItems(items);
		ui.comboBoxInterfaceName->setCurrentIndex(0);
	}
	else if( protocolName.compare("MAXON SERIAL V2", Qt::CaseInsensitive) ==0 )
	{
		ui.comboBoxInterfaceName->clear();
		QStringList items;
		items.append("USB");
		ui.comboBoxInterfaceName->addItems(items);
		ui.comboBoxInterfaceName->setCurrentIndex(0);
	}

	settings_.setProtocolStackName(protocolName);
}

void settings_dialog::onInterfaceChange(QString interfaceName)
{
	if( interfaceName.compare("RS232", Qt::CaseInsensitive) == 0)
	{
		ui.comboBoxPortName->clear();
		QStringList items;
		for(int i=0; i<4; i++)
			items.append(QString("/dev/ttyS%1").arg(i));
		ui.comboBoxPortName->addItems(items);
		ui.comboBoxPortName->setCurrentIndex(0);

		items.clear();

		items.append("9600");
		items.append("14400");
		items.append("19200");
		items.append("38400");
		items.append("57600");
		items.append("115200");

		ui.comboBoxBaudrate->clear();
		ui.comboBoxBaudrate->addItems(items);

		QString deviceName = ui.comboBoxDeviceName->itemText(ui.comboBoxDeviceName->currentIndex());

		if( deviceName.compare("EPOS", Qt::CaseInsensitive) == 0)
			ui.comboBoxBaudrate->setCurrentIndex(3); //38400
		if( deviceName.compare("EPOS2", Qt::CaseInsensitive) == 0)
			ui.comboBoxBaudrate->setCurrentIndex(5); //115200
	}
	else if( interfaceName.compare("USB", Qt::CaseInsensitive) ==0 )
	{
		ui.comboBoxPortName->clear();
		QStringList items;
		for(int i=0; i<4; i++)
			items.append(QString("USB%1").arg(i));
		ui.comboBoxPortName->addItems(items);
		ui.comboBoxPortName->setCurrentIndex(0);

		items.clear();

		items.append("1000000");

		ui.comboBoxBaudrate->clear();
		ui.comboBoxBaudrate->addItems(items);
		ui.comboBoxBaudrate->setCurrentIndex(0);
	}

	settings_.setInterfaceName(interfaceName);
}

void settings_dialog::onBaudrateChange(QString baudrate)
{
	settings_.setBaudrate(baudrate);
}

void settings_dialog::onPortChange(QString portName)
{
	settings_.setPortName(portName);
}

void settings_dialog::onTimeoutChanged(QString timeout)
{
	settings_.setTimeout(timeout);
}

epos_settings* settings_dialog::getSettings()
{
	epos_settings* settings = new epos_settings;
	*settings = settings_;
	return settings;
}
