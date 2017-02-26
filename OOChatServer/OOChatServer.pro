#-------------------------------------------------
#
# Project created by QtCreator 2016-10-27T16:05:48
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OOChatServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Control/IMServerMessageCtrl.cpp \
    Model/IMTcpServer.cpp \
    Control/IMClientSocketCtrl.cpp \
    Control/IMDatabaseCtrl.cpp \
    View/IMServerWindow.cpp

HEADERS  += mainwindow.h \
    Control/IMServerMessageCtrl.h \
    View/IMServerWindow.h \
    Model/IMTcpServer.h \
    Control/IMClientSocketCtrl.h \
    Control/IMDatabaseCtrl.h \
    Model/IMConstant.h

FORMS    +=
