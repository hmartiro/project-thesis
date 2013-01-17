/********************************************************************************
** Form generated from reading UI file 'demo_linux.ui'
**
** Created: Thu Dec 16 15:19:29 2010
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEMO_LINUX_H
#define UI_DEMO_LINUX_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Demo_LinuxClass
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_5;
    QVBoxLayout *verticalLayout_4;
    QSpacerItem *verticalSpacer_4;
    QPushButton *pushButtonDeviceSettings;
    QSpacerItem *verticalSpacer_2;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pushButtonEnable;
    QPushButton *pushButtonDisable;
    QSpacerItem *verticalSpacer_3;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButtonMove;
    QPushButton *pushButtonHalt;
    QSpacerItem *verticalSpacer;
    QVBoxLayout *verticalLayout_3;
    QPushButton *pushButtonExit;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout_6;
    QGroupBox *groupBox;
    QLineEdit *lineEditNodeId;
    QLabel *label;
    QGroupBox *groupBox_2;
    QLabel *label_3;
    QLineEdit *lineEditTargetPosition;
    QRadioButton *radioButtonRelativeMove;
    QRadioButton *radioButtonAbsoluteMove;
    QLabel *label_2;
    QGroupBox *groupBox_3;
    QLabel *label_4;
    QLineEdit *lineEditPositionStart;
    QLabel *label_5;
    QLabel *label_6;
    QLineEdit *lineEditPositionActualValue;
    QLabel *label_7;

    void setupUi(QDialog *Demo_LinuxClass)
    {
        if (Demo_LinuxClass->objectName().isEmpty())
            Demo_LinuxClass->setObjectName(QString::fromUtf8("Demo_LinuxClass"));
        Demo_LinuxClass->resize(526, 462);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Demo_LinuxClass->sizePolicy().hasHeightForWidth());
        Demo_LinuxClass->setSizePolicy(sizePolicy);
        QFont font;
        font.setFamily(QString::fromUtf8("Ubuntu"));
        Demo_LinuxClass->setFont(font);
        Demo_LinuxClass->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"     background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                       stop: 0 #E0E0E0, stop: 1 #FFFFFF);\n"
"     border: 2px solid gray;\n"
"     border-radius: 5px;\n"
"     margin-top: 1ex; /* leave space at the top for the title */\n"
" }\n"
"\n"
" QGroupBox::title {\n"
"     subcontrol-origin: margQGroupBox {\n"
"     background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                       stop: 0 #E0E0E0, stop: 1 #FFFFFF);\n"
"     border: 2px solid gray;\n"
"     border-radius: 5px;\n"
"     margin-top: 1ex; /* leave space at the top for the title */\n"
" }\n"
"\n"
" QGroupBox::title {\n"
"     subcontrol-origin: margin;\n"
"     subcontrol-position: top center; /* position at the top center */\n"
"     padding: 0 3px;\n"
"     background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                       stop: 0 #FFOECE, stop: 1 #FFFFFF);\n"
" }\n"
"in;\n"
"     subcontrol-position: top cen"
                        "ter; /* position at the top center */\n"
"     padding: 0 3px;\n"
"     background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                       stop: 0 #FFOECE, stop: 1 #FFFFFF);\n"
" }"));
        layoutWidget = new QWidget(Demo_LinuxClass);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(400, 20, 118, 431));
        verticalLayout_5 = new QVBoxLayout(layoutWidget);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalSpacer_4 = new QSpacerItem(20, 6, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_4->addItem(verticalSpacer_4);

        pushButtonDeviceSettings = new QPushButton(layoutWidget);
        pushButtonDeviceSettings->setObjectName(QString::fromUtf8("pushButtonDeviceSettings"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pushButtonDeviceSettings->sizePolicy().hasHeightForWidth());
        pushButtonDeviceSettings->setSizePolicy(sizePolicy1);
        pushButtonDeviceSettings->setFont(font);

        verticalLayout_4->addWidget(pushButtonDeviceSettings);


        verticalLayout_5->addLayout(verticalLayout_4);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_5->addItem(verticalSpacer_2);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        pushButtonEnable = new QPushButton(layoutWidget);
        pushButtonEnable->setObjectName(QString::fromUtf8("pushButtonEnable"));
        pushButtonEnable->setFont(font);

        verticalLayout_2->addWidget(pushButtonEnable);

        pushButtonDisable = new QPushButton(layoutWidget);
        pushButtonDisable->setObjectName(QString::fromUtf8("pushButtonDisable"));
        pushButtonDisable->setFont(font);

        verticalLayout_2->addWidget(pushButtonDisable);


        verticalLayout_5->addLayout(verticalLayout_2);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_5->addItem(verticalSpacer_3);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        pushButtonMove = new QPushButton(layoutWidget);
        pushButtonMove->setObjectName(QString::fromUtf8("pushButtonMove"));
        pushButtonMove->setFont(font);

        verticalLayout->addWidget(pushButtonMove);

        pushButtonHalt = new QPushButton(layoutWidget);
        pushButtonHalt->setObjectName(QString::fromUtf8("pushButtonHalt"));
        pushButtonHalt->setFont(font);

        verticalLayout->addWidget(pushButtonHalt);


        verticalLayout_5->addLayout(verticalLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_5->addItem(verticalSpacer);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        pushButtonExit = new QPushButton(layoutWidget);
        pushButtonExit->setObjectName(QString::fromUtf8("pushButtonExit"));
        pushButtonExit->setFont(font);

        verticalLayout_3->addWidget(pushButtonExit);


        verticalLayout_5->addLayout(verticalLayout_3);

        layoutWidget1 = new QWidget(Demo_LinuxClass);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(20, 20, 371, 431));
        verticalLayout_6 = new QVBoxLayout(layoutWidget1);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        groupBox = new QGroupBox(layoutWidget1);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setFont(font);
        groupBox->setAutoFillBackground(false);
        groupBox->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"     border: 2px solid gray;\n"
"     border-radius: 5px;\n"
"     margin-top: 1ex; /* leave space at the top for the title */\n"
" }\n"
"\n"
" QGroupBox::title {\n"
"     subcontrol-origin: margin;\n"
"     subcontrol-position: top left; /* position at the top center */\n"
"     padding: 0 3px;\n"
"	 left: 5px;\n"
" }"));
        groupBox->setFlat(false);
        lineEditNodeId = new QLineEdit(groupBox);
        lineEditNodeId->setObjectName(QString::fromUtf8("lineEditNodeId"));
        lineEditNodeId->setGeometry(QRect(168, 60, 120, 27));
        lineEditNodeId->setFont(font);
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(30, 64, 49, 16));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy2);
        label->setFont(font);

        verticalLayout_6->addWidget(groupBox);

        groupBox_2 = new QGroupBox(layoutWidget1);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setFont(font);
        groupBox_2->setAutoFillBackground(false);
        groupBox_2->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"     border: 2px solid gray;\n"
"     border-radius: 5px;\n"
"     margin-top: 1ex; /* leave space at the top for the title */\n"
" }\n"
"\n"
" QGroupBox::title {\n"
"     subcontrol-origin: margin;\n"
"     subcontrol-position: top left; /* position at the top center */\n"
"     padding: 0 3px;\n"
"	left: 5px;\n"
" }\n"
""));
        groupBox_2->setFlat(false);
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(300, 44, 16, 16));
        label_3->setFont(font);
        lineEditTargetPosition = new QLineEdit(groupBox_2);
        lineEditTargetPosition->setObjectName(QString::fromUtf8("lineEditTargetPosition"));
        lineEditTargetPosition->setGeometry(QRect(168, 40, 120, 26));
        lineEditTargetPosition->setFont(font);
        radioButtonRelativeMove = new QRadioButton(groupBox_2);
        radioButtonRelativeMove->setObjectName(QString::fromUtf8("radioButtonRelativeMove"));
        radioButtonRelativeMove->setGeometry(QRect(170, 80, 117, 21));
        radioButtonRelativeMove->setFont(font);
        radioButtonRelativeMove->setChecked(true);
        radioButtonAbsoluteMove = new QRadioButton(groupBox_2);
        radioButtonAbsoluteMove->setObjectName(QString::fromUtf8("radioButtonAbsoluteMove"));
        radioButtonAbsoluteMove->setGeometry(QRect(170, 110, 123, 21));
        radioButtonAbsoluteMove->setFont(font);
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(30, 44, 93, 16));
        sizePolicy2.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy2);
        label_2->setFont(font);

        verticalLayout_6->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(layoutWidget1);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setFont(font);
        groupBox_3->setAutoFillBackground(false);
        groupBox_3->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"     background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                       stop: 0 #E0E0E0, stop: 1 #FFFFFF);\n"
"     border: 2px solid gray;\n"
"     border-radius: 5px;\n"
"     margin-top: 1ex; /* leave space at the top for the title */\n"
" }\n"
"\n"
" QGroupBox::title {\n"
"     subcontrol-origin: margin;\n"
"     subcontrol-position: top left; /* position at the top center */\n"
"     padding: 0 3px;\n"
"	left: 5px;\n"
"     background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                       stop: 0 #FFOECE, stop: 1 #FFFFFF);\n"
" }\n"
""));
        groupBox_3->setFlat(false);
        label_4 = new QLabel(groupBox_3);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(30, 44, 83, 16));
        label_4->setFont(font);
        lineEditPositionStart = new QLineEdit(groupBox_3);
        lineEditPositionStart->setObjectName(QString::fromUtf8("lineEditPositionStart"));
        lineEditPositionStart->setGeometry(QRect(168, 41, 120, 26));
        lineEditPositionStart->setFont(font);
        lineEditPositionStart->setReadOnly(true);
        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(300, 44, 16, 16));
        label_5->setFont(font);
        label_6 = new QLabel(groupBox_3);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(30, 84, 127, 16));
        label_6->setFont(font);
        lineEditPositionActualValue = new QLineEdit(groupBox_3);
        lineEditPositionActualValue->setObjectName(QString::fromUtf8("lineEditPositionActualValue"));
        lineEditPositionActualValue->setGeometry(QRect(169, 81, 120, 26));
        lineEditPositionActualValue->setFont(font);
        lineEditPositionActualValue->setReadOnly(true);
        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(300, 84, 16, 16));
        label_7->setFont(font);

        verticalLayout_6->addWidget(groupBox_3);


        retranslateUi(Demo_LinuxClass);

        QMetaObject::connectSlotsByName(Demo_LinuxClass);
    } // setupUi

    void retranslateUi(QDialog *Demo_LinuxClass)
    {
        Demo_LinuxClass->setWindowTitle(QApplication::translate("Demo_LinuxClass", "QT Demo using EPOS Linux shared library", 0, QApplication::UnicodeUTF8));
        pushButtonDeviceSettings->setText(QApplication::translate("Demo_LinuxClass", "Device Settings", 0, QApplication::UnicodeUTF8));
        pushButtonEnable->setText(QApplication::translate("Demo_LinuxClass", "Enable", 0, QApplication::UnicodeUTF8));
        pushButtonDisable->setText(QApplication::translate("Demo_LinuxClass", "Disable", 0, QApplication::UnicodeUTF8));
        pushButtonMove->setText(QApplication::translate("Demo_LinuxClass", "Move", 0, QApplication::UnicodeUTF8));
        pushButtonHalt->setText(QApplication::translate("Demo_LinuxClass", "Halt", 0, QApplication::UnicodeUTF8));
        pushButtonExit->setText(QApplication::translate("Demo_LinuxClass", "Exit", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("Demo_LinuxClass", "Active Operation Mode/NodeID", 0, QApplication::UnicodeUTF8));
        lineEditNodeId->setText(QApplication::translate("Demo_LinuxClass", "1", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Demo_LinuxClass", "Node ID", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("Demo_LinuxClass", "Profile", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Demo_LinuxClass", "qc", 0, QApplication::UnicodeUTF8));
        lineEditTargetPosition->setText(QApplication::translate("Demo_LinuxClass", "2000", 0, QApplication::UnicodeUTF8));
        radioButtonRelativeMove->setText(QApplication::translate("Demo_LinuxClass", "Relative Move", 0, QApplication::UnicodeUTF8));
        radioButtonAbsoluteMove->setText(QApplication::translate("Demo_LinuxClass", "Absolute Move", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Demo_LinuxClass", "Target Position", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("Demo_LinuxClass", "Actual Values", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Demo_LinuxClass", "Position Start", 0, QApplication::UnicodeUTF8));
        lineEditPositionStart->setText(QApplication::translate("Demo_LinuxClass", "0", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Demo_LinuxClass", "qc", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Demo_LinuxClass", "Position Actual Value", 0, QApplication::UnicodeUTF8));
        lineEditPositionActualValue->setText(QApplication::translate("Demo_LinuxClass", "0", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("Demo_LinuxClass", "qc", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Demo_LinuxClass: public Ui_Demo_LinuxClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEMO_LINUX_H
