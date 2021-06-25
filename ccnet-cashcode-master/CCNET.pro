TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    CCNET.cpp \
    cashcodeprotocol.cpp \
    serialport.cpp \
    ccpackage.cpp

HEADERS += cashcodeprotocol.h \
    CCNET.h \
    cashcodeerrors.h \
    serialport.h \
    ccpackage.h \
    commands.h

LIBS += -lpthread -lboost_system -lboost_thread
