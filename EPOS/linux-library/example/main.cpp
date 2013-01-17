/*
 * main.cpp
 *
 *  Created on: Dec 16, 2010
 *      Author: DAWS
 *      Copyright: maxonmotor ag (c) 2010
 */


#include "main_dialog.h"
#include "settings_dialog.h"
#include <QtGui>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    settings_dialog deviceSettingsDialog;
    main_dialog* w;

	if( deviceSettingsDialog.exec() == QDialog::Accepted )
		w = new main_dialog(0, deviceSettingsDialog.getSettings());
	else
		w = new main_dialog(0);

	w->show();

	int retVal = a.exec();

	delete w;

    return retVal;
}
