#ifndef DEVICESETTINGS_H
#define DEVICESETTINGS_H

/*
 * devicesettings.h
 *
 *  Created on: Dec 16, 2010
 *      Author: DAWS
 *      Copyright: maxonmotor ag (c) 2010
 */

#include <QtGui/QDialog>
#include "ui_devicesettings.h"
#include "epos_properties.h"

class settings_dialog : public QDialog
{
    Q_OBJECT

public:
    settings_dialog(QWidget *parent = 0);
    virtual ~settings_dialog();

public:
    epos_settings* getSettings();

public slots:
    void onCancel();
    void onOpen();
    void onDeviceNameChange(QString);
    void onProtocolChange(QString);
    void onInterfaceChange(QString);
    void onPortChange(QString);
    void onTimeoutChanged(QString);
    void onBaudrateChange(QString);

private:
    Ui::DeviceSettingsClass ui;
    epos_settings settings_;
};

#endif // DEVICESETTINGS_H
