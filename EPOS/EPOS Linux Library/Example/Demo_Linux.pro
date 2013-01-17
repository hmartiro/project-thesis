TEMPLATE = app
TARGET = Demo_Linux
QT += core \
    gui \
    xml \
    xmlpatterns
HEADERS += epos_properties.h \
    settings_dialog.h \
    main_dialog.h \
    Definitions.h
SOURCES += epos_properties.cpp \
    settings_dialog.cpp \
    main_dialog.cpp \
    main.cpp
FORMS += settings_dialog.ui \
    main_dialog.ui
RESOURCES += Demo_linux.qrc
LIBS += -L. \
    -lEposCmd
RC_FILE = Demo_Linux.rc