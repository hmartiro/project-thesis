/********************************************************************************
** Form generated from reading UI file 'devicesettings.ui'
**
** Created: Thu Dec 16 10:40:27 2010
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEVICESETTINGS_H
#define UI_DEVICESETTINGS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_DeviceSettingsClass
{
public:
    QLabel *label;
    QComboBox *comboBoxDeviceName;
    QLabel *label_2;
    QComboBox *comboBoxProtocolStackName;
    QComboBox *comboBoxInterfaceName;
    QLabel *label_3;
    QLabel *label_4;
    QComboBox *comboBoxPortName;
    QLabel *label_5;
    QComboBox *comboBoxBaudrate;
    QLabel *label_6;
    QLineEdit *lineEditTimeout;
    QLabel *label_7;
    QPushButton *pushButtonOpen;
    QPushButton *pushButtonCancel;

    void setupUi(QDialog *DeviceSettingsClass)
    {
        if (DeviceSettingsClass->objectName().isEmpty())
            DeviceSettingsClass->setObjectName(QString::fromUtf8("DeviceSettingsClass"));
        DeviceSettingsClass->setWindowModality(Qt::WindowModal);
        DeviceSettingsClass->resize(394, 345);
        QFont font;
        font.setFamily(QString::fromUtf8("Ubuntu"));
        DeviceSettingsClass->setFont(font);
        label = new QLabel(DeviceSettingsClass);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(40, 44, 81, 17));
        comboBoxDeviceName = new QComboBox(DeviceSettingsClass);
        comboBoxDeviceName->setObjectName(QString::fromUtf8("comboBoxDeviceName"));
        comboBoxDeviceName->setGeometry(QRect(170, 40, 201, 27));
        label_2 = new QLabel(DeviceSettingsClass);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(40, 84, 126, 16));
        comboBoxProtocolStackName = new QComboBox(DeviceSettingsClass);
        comboBoxProtocolStackName->setObjectName(QString::fromUtf8("comboBoxProtocolStackName"));
        comboBoxProtocolStackName->setGeometry(QRect(170, 80, 201, 27));
        comboBoxInterfaceName = new QComboBox(DeviceSettingsClass);
        comboBoxInterfaceName->setObjectName(QString::fromUtf8("comboBoxInterfaceName"));
        comboBoxInterfaceName->setGeometry(QRect(170, 120, 201, 27));
        label_3 = new QLabel(DeviceSettingsClass);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(40, 124, 93, 16));
        label_4 = new QLabel(DeviceSettingsClass);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(40, 164, 65, 16));
        comboBoxPortName = new QComboBox(DeviceSettingsClass);
        comboBoxPortName->setObjectName(QString::fromUtf8("comboBoxPortName"));
        comboBoxPortName->setGeometry(QRect(170, 160, 201, 27));
        label_5 = new QLabel(DeviceSettingsClass);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(40, 204, 56, 16));
        comboBoxBaudrate = new QComboBox(DeviceSettingsClass);
        comboBoxBaudrate->setObjectName(QString::fromUtf8("comboBoxBaudrate"));
        comboBoxBaudrate->setGeometry(QRect(170, 200, 201, 27));
        label_6 = new QLabel(DeviceSettingsClass);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(40, 244, 52, 16));
        lineEditTimeout = new QLineEdit(DeviceSettingsClass);
        lineEditTimeout->setObjectName(QString::fromUtf8("lineEditTimeout"));
        lineEditTimeout->setGeometry(QRect(170, 240, 113, 27));
        label_7 = new QLabel(DeviceSettingsClass);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(290, 244, 17, 17));
        pushButtonOpen = new QPushButton(DeviceSettingsClass);
        pushButtonOpen->setObjectName(QString::fromUtf8("pushButtonOpen"));
        pushButtonOpen->setGeometry(QRect(190, 300, 91, 27));
        pushButtonCancel = new QPushButton(DeviceSettingsClass);
        pushButtonCancel->setObjectName(QString::fromUtf8("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(290, 300, 91, 27));

        retranslateUi(DeviceSettingsClass);

        comboBoxDeviceName->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(DeviceSettingsClass);
    } // setupUi

    void retranslateUi(QDialog *DeviceSettingsClass)
    {
        DeviceSettingsClass->setWindowTitle(QApplication::translate("DeviceSettingsClass", "Open Device", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DeviceSettingsClass", "Device Name", 0, QApplication::UnicodeUTF8));
        comboBoxDeviceName->clear();
        comboBoxDeviceName->insertItems(0, QStringList()
         << QApplication::translate("DeviceSettingsClass", "EPOS", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DeviceSettingsClass", "EPOS2", 0, QApplication::UnicodeUTF8)
        );
        label_2->setText(QApplication::translate("DeviceSettingsClass", "Protocol Stack Name", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("DeviceSettingsClass", "Interface Name", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("DeviceSettingsClass", "Port Name", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("DeviceSettingsClass", "Baudrate", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("DeviceSettingsClass", "Timeout", 0, QApplication::UnicodeUTF8));
        lineEditTimeout->setText(QApplication::translate("DeviceSettingsClass", "500", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("DeviceSettingsClass", "ms", 0, QApplication::UnicodeUTF8));
        pushButtonOpen->setText(QApplication::translate("DeviceSettingsClass", "Open", 0, QApplication::UnicodeUTF8));
        pushButtonCancel->setText(QApplication::translate("DeviceSettingsClass", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DeviceSettingsClass: public Ui_DeviceSettingsClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEVICESETTINGS_H
