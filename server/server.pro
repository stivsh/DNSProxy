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
    ../common/eventhandler.cpp \
    ../common/OptionReader.cpp \
    ../common/logger.cpp \
    ../common/serverrunenvironment.cpp \
    ../common/watchdogtimer.cpp \
    ../common/buffer.cpp \
    ../common/dnspack.cpp \
    ../common/pool.cpp \
    ../common/HANDLER.cpp

HEADERS += \
    servercore.h \
    ../common/eventhandler.h \
    event_handlers/messidentconteiner.h \
    event_handlers/NewConnectionHandler.h \
    event_handlers/UDPServerHandler.h \
    event_handlers/ClientHandler.h \
    event_handlers/HandlerFactory.h \
    ../common/OptionReader.h \
    ../common/logger.h \
    ../common/commondefines.h \
    ../common/serverrunenvironment.h \
    ../common/watchdogtimer.h \
    ../common/buffer.h \
    ../common/dnspack.h \
    ../common/pool.h \
    ../common/HANDLER.h \
    ../common/eventstoreact.h \
    ../common/AbstractEventHandlerFactory.h
LIBS +=-lrt
debug {
  DEFINES += DEBUG_VERSION
}
release {
  DEFINES += RELEASE_VERSION
}
