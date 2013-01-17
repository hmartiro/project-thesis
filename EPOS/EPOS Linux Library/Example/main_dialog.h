#ifndef DEMO_LINUX_H
#define DEMO_LINUX_H

/*
 * demo_linux.h
 *
 *  Created on: Dec 16, 2010
 *      Author: DAWS
 *      Copyright: maxonmotor ag (c) 2010
 */

#include <QtGui/QDialog>
#include "ui_demo_linux.h"
#include "epos_properties.h"
#include <QTimer>

class main_dialog : public QDialog
{
    Q_OBJECT

public:
    main_dialog(QWidget *parent, epos_settings* settings=0);
    virtual ~main_dialog();

public slots:
    void onExit();
    void onOpenDevice();
    void onEnableDevice();
    void onDisableDevice();
    void updateView();
    void onMove();
    void onHalt();

private:
    bool openDevice();
    void closeDevice();
    void enableControls(bool);

private:
    Ui::Demo_LinuxClass ui;
    epos_settings* settings_;
    void* keyHandle_;
    QTimer* timer_;
};

#endif // DEMO_LINUX_H
