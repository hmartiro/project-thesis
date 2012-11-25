/*
 * demo_linux.cpp
 *
 *  Created on: Dec 16, 2010
 *      Author: DAWS
 *      Copyright: maxonmotor ag (c) 2010
 */


#include "main_dialog.h"
#include <QMessageBox>
#include "settings_dialog.h"
#include "Definitions.h"

main_dialog::main_dialog(QWidget *parent, epos_settings* settings)
    : QDialog(parent)
{
	keyHandle_ = 0;
	settings_ = settings;
	timer_ = new QTimer(this);

	ui.setupUi(this);

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	setFixedSize(526,462);

	QObject::connect(ui.pushButtonExit, SIGNAL(clicked()), SLOT(onExit()));
	QObject::connect(ui.pushButtonDeviceSettings, SIGNAL(clicked()), SLOT(onOpenDevice()));
	QObject::connect(ui.pushButtonEnable, SIGNAL(clicked()), SLOT(onEnableDevice()));
	QObject::connect(ui.pushButtonDisable, SIGNAL(clicked()), SLOT(onDisableDevice()));
	QObject::connect(ui.pushButtonMove, SIGNAL(clicked()), SLOT(onMove()));
	QObject::connect(ui.pushButtonHalt, SIGNAL(clicked()), SLOT(onHalt()));

	QObject::connect(timer_, SIGNAL(timeout()), this, SLOT(updateView()));

	if( settings == 0 || (settings !=0 && !openDevice()) )
		enableControls(false);

	timer_->setInterval(100);
	timer_->start();
}

main_dialog::~main_dialog()
{
	delete settings_;
	delete timer_;
}

void main_dialog::updateView()
{
	if( keyHandle_ )
	{
		unsigned long ErrorCode = 0;
		long PositionIs = 0;

		if( VCS_GetPositionIs(keyHandle_,
				ui.lineEditNodeId->text().toUInt(),
				&PositionIs, &ErrorCode) )
			ui.lineEditPositionActualValue->setText(QString("%1").arg(PositionIs));

		if( ui.lineEditPositionStart->text().toULong() == 0 )
			ui.lineEditPositionStart->setText(QString("%1").arg(PositionIs));

		int IsEnabled;

		if( VCS_GetEnableState(keyHandle_, ui.lineEditNodeId->text().toUInt(), &IsEnabled, &ErrorCode) )
		{
			ui.pushButtonEnable->setEnabled(IsEnabled!=TRUE);
			ui.pushButtonDisable->setEnabled(IsEnabled==TRUE);
			ui.pushButtonMove->setEnabled(IsEnabled==TRUE);
			ui.pushButtonHalt->setEnabled(IsEnabled==TRUE);
		}
	}
}

void main_dialog::onEnableDevice()
{
	unsigned long ErrorCode = 0;
	unsigned short nodeId = ui.lineEditNodeId->text().toUInt();
	int IsInFault = FALSE;

	if( VCS_GetFaultState(keyHandle_, nodeId, &IsInFault, &ErrorCode) )
	{
		if( IsInFault && !VCS_ClearFault(keyHandle_, nodeId, &ErrorCode) )
		{
			QString message = QString("Clear fault failed!, error code=0x%1").arg(ErrorCode,0,16);

			QMessageBox::critical(this, "QT Demo using EPOS Linux shared library",
							message);
			return;
		}

		int IsEnabled = FALSE;
		if( VCS_GetEnableState(keyHandle_, nodeId, &IsEnabled, &ErrorCode) )
		{
			if( !IsEnabled && !VCS_SetEnableState(keyHandle_, nodeId, &ErrorCode) )
			{
				QString message = QString("Set enable state failed!, error code=0x%1").arg(ErrorCode,0,16);

				QMessageBox::critical(this, "QT Demo using EPOS Linux shared library",
								message);
			}
			else
			{
				ui.pushButtonEnable->setEnabled(false);
				ui.pushButtonDisable->setEnabled(true);
				ui.pushButtonMove->setEnabled(true);
				ui.pushButtonHalt->setEnabled(true);
			}
		}
	}
	else
	{
		QString message = QString("Get fault state failed!, error code=0x%1").arg(ErrorCode,0,16);

		QMessageBox::critical(this, "QT Demo using EPOS Linux shared library",
						message);

	}
}

void main_dialog::onDisableDevice()
{
	unsigned long ErrorCode = 0;
	unsigned short nodeId = ui.lineEditNodeId->text().toUInt();
	int IsInFault = FALSE;

	if( VCS_GetFaultState(keyHandle_, nodeId, &IsInFault, &ErrorCode) )
	{
		if( IsInFault && !VCS_ClearFault(keyHandle_, nodeId, &ErrorCode) )
		{
			QString message = QString("Clear fault failed!, error code=0x%1").arg(ErrorCode,0,16);

			QMessageBox::critical(this, "QT Demo using EPOS Linux shared library",
							message);
			return;
		}

		int IsEnabled = FALSE;
		if( VCS_GetEnableState(keyHandle_, nodeId, &IsEnabled, &ErrorCode) )
		{
			if( IsEnabled && !VCS_SetDisableState(keyHandle_, nodeId, &ErrorCode) )
			{
				QString message = QString("Set enable state failed!, error code=0x%1").arg(ErrorCode,0,16);

				QMessageBox::critical(this, "QT Demo using EPOS Linux shared library",
								message);
			}
			else
			{
				ui.pushButtonEnable->setEnabled(true);
				ui.pushButtonDisable->setEnabled(false);
				ui.pushButtonMove->setEnabled(false);
				ui.pushButtonHalt->setEnabled(false);
			}
		}
	}
	else
	{
		QString message = QString("Get fault state failed!, error code=0x%1").arg(ErrorCode,0,16);

		QMessageBox::critical(this, "QT Demo using EPOS Linux shared library",
						message);

	}
}

void main_dialog::enableControls(bool enable)
{
	ui.pushButtonEnable->setEnabled(enable);
	ui.pushButtonDisable->setEnabled(enable);
	ui.pushButtonMove->setEnabled(enable);
	ui.pushButtonHalt->setEnabled(enable);
}

void main_dialog::closeDevice()
{
    unsigned long ErrorCode = 0;
    if(keyHandle_ != 0)
        VCS_CloseDevice(keyHandle_, &ErrorCode);

    VCS_CloseAllDevices(&ErrorCode);
}
void main_dialog::onExit()
{
    closeDevice();

	close();
}

bool main_dialog::openDevice()
{
	unsigned long ErrorCode = 0;

	closeDevice();

	keyHandle_ = VCS_OpenDevice(	(char*) settings_->getDeviceName().toStdString().c_str(),
									(char*) settings_->getProtocolStackName().toStdString().c_str(),
									(char*) settings_->getInterfaceName().toStdString().c_str(),
									(char*) settings_->getPortName().toStdString().c_str(), &ErrorCode);

	if( keyHandle_ == 0 )
	{
		QString message = QString("Open device failure, error code=0x%1").arg(ErrorCode,0,16);

		QMessageBox::critical(this, "QT Demo using EPOS Linux shared library",
						message);

		return false;
	}

	if( !VCS_SetProtocolStackSettings(keyHandle_,
			settings_->getBaudrate(),
			settings_->getTimeout(), &ErrorCode) )
	{
		QString message = QString("Set protocol stack settings failed!, error code=0x%1").arg(ErrorCode,0,16);

		QMessageBox::critical(this, "QT Demo using EPOS Linux shared library",
						message);

		return false;
	}

	enableControls(true);

	return true;
}

void main_dialog::onOpenDevice()
{
	settings_dialog deviceSettingsDialog(this);

	if( deviceSettingsDialog.exec() == QDialog::Accepted )
	{
		delete settings_;

		settings_ = deviceSettingsDialog.getSettings();

		openDevice();
	}
}

void main_dialog::onMove()
{
	unsigned short nodeId = ui.lineEditNodeId->text().toUInt();
	unsigned long errorCode = 0;

    if( VCS_ActivateProfilePositionMode(keyHandle_, nodeId, &errorCode) )
    {
    	long TargetPosition = ui.lineEditTargetPosition->text().toLong();
    	int Absolute = ui.radioButtonRelativeMove->isChecked() ? FALSE : TRUE;
    	int Immediately = TRUE;

    	if( !Absolute )
    	{
    		long PositionIs = 0;

			if( VCS_GetPositionIs(keyHandle_,
					ui.lineEditNodeId->text().toUInt(),
					&PositionIs, &errorCode) )
				ui.lineEditPositionStart->setText(QString("%1").arg(PositionIs));
    	}

    	if( !VCS_MoveToPosition(keyHandle_, nodeId, TargetPosition, Absolute, Immediately, &errorCode) )
    	{
    		QString message = QString("Move to position failed!, error code=0x%1").arg(errorCode, 0, 16);

			QMessageBox::critical(this, "QT Demo using EPOS Linux shared library",
									message);

    	}
    }
    else
    {
    	QString message = QString("Activate profile position mode failed!, error code=0x%1").arg(errorCode, 0, 16);

    	QMessageBox::critical(this, "QT Demo using EPOS Linux shared library",
    							message);
    }
}

void main_dialog::onHalt()
{
	unsigned short nodeId = ui.lineEditNodeId->text().toUInt();
	unsigned long errorCode = 0;

	if( !VCS_HaltPositionMovement(keyHandle_, nodeId, &errorCode) )
	{
		QString message = QString("Halt position movement failed!, error code=0x%1").arg(errorCode, 0, 16);

		QMessageBox::critical(this, "QT Demo using EPOS Linux shared library",
						message);
	}
}
