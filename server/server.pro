#-------------------------------------------------
#
# Project created by QtCreator 2014-02-06T22:31:44
#
#-------------------------------------------------
TARGET = dnsproxyserver
QT -= gui core
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    servercore.cpp \
    event_handlers/messidentconteiner.cpp \
    event_handlers/ClientHandler.cpp \
    event_handlers/HandlerFactory.cpp \
    event_handlers/NewConnectionHandler.cpp \
    event_handlers/UDPServerHandler.cpp \
    event_handlers/eventhandler.cpp \
    ../common/OptionReader.cpp

HEADERS += \
    servercore.h \
    event_handlers/eventhandler.h \
    event_handlers/messidentconteiner.h \
    logger.h \
    event_handlers/NewConnectionHandler.h \
    event_handlers/UDPServerHandler.h \
    event_handlers/ClientHandler.h \
    event_handlers/HandlerFactory.h \
    ../common/OptionReader.h
DEFINES +=DEBUGSERV
