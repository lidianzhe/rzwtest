#-------------------------------------------------
#
# Project created by QtCreator 2013-06-17T11:07:50
#
#-------------------------------------------------

QT       += core gui

TARGET = rzwtest
TEMPLATE = app

# Change the following version number
EMA_VERSION = 1.0.1
# End of Change the following version number

#DEFINES += QT_NO_DEBUG_OUTPUT
CONFIG += _arm_

SOURCES += main.cpp\
        dialog.cpp \
    eventthread.cpp

HEADERS  += dialog.h \
    eventthread.h

FORMS    += dialog.ui

linux-g++ {
    LINUX_LIB_DIR = /usr/local/lib
    LINUX_INCLUDE_DIR = ../include



    _arm_ {
        message(_arm_)
        INCLUDEPATH += $$LINUX_INCLUDE_DIR
        LIBS  += -L$$LINUX_LIB_DIR -lemaLib_arm_$$EMA_VERSION

        ematest.files = $${TARGET}
        ematest.path = /usr/local/bin
        ematest.extra = strip $${TARGET};
        INSTALLS += rzwtest
    }
}

