#-------------------------------------------------
#
# Project created by QtCreator 2018-07-12T20:47:15
#
#-------------------------------------------------

QT       -= core gui

TARGET = UtilPro
TEMPLATE = lib
CONFIG += staticlib
DEFINES += _linux_

INCLUDEPATH += ../../code/public

SOURCES += utilprostaic.cpp \
    ../../code/UtilProc/UtilityProc.c \
    ../../code/UtilProc/ProcTlv.c \
    ../../code/UtilProc/ConvertData.c

HEADERS += utilprostaic.h \
    ../../code/UtilProc/UtilityProc.h \
    ../../code/UtilProc/UtilityDef.h \
    ../../code/UtilProc/ConvertData.h \
    ../../code/public/TypeDef.h \
    ../../code/public/TransmitData.h \
    ../../code/public/ManageThread.h \
    ../../code/public/Macro_Proj.h \
    ../../code/public/debugOut.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
