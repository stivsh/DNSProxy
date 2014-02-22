#-------------------------------------------------
#
# Project created by QtCreator 2014-02-06T22:31:44
#
#-------------------------------------------------
TARGET = dnsproxyclient
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    servercore.cpp \
    ../common/OptionReader.cpp \
    ../common/logger.cpp \
    ../common/serverrunenvironment.cpp \
    ../common/watchdogtimer.cpp

HEADERS += \
    servercore.h\
    ../common/OptionReader.h \
    ../common/logger.h \
    ../common/commondefines.h \
    ../common/serverrunenvironment.h \
    ../common/watchdogtimer.h
LIBS +=-lrt
