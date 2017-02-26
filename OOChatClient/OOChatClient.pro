#-------------------------------------------------
#
# Project created by QtCreator 2016-11-02T18:37:15
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OOChatClient
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    View/IMLoginWidget.cpp \
    Control/IMLoginCtrl.cpp \
    Model/IMTcpSocket.cpp \
    Model/IMEncryption.cpp \
    View/IMClickLabel.cpp \
    View/IMRegisterWidget.cpp \
    Control/IMRegisterCtrl.cpp \
    Control/IMClientFileCtrl.cpp \
    View/CalendarDialog.cpp \
    View/CustomCalendar.cpp \
    View/CustomComboBox.cpp \
    View/CustomLineEdit.cpp \
    View/IMMainWidget.cpp \
    Control/IMMainCtrl.cpp \
    Control/IMDatabaseCtrl.cpp \
    View/IMChatWidget.cpp \
    View/IMFlockChatWidget.cpp \
    View/IMDiscussionChatWidget.cpp \
    View/IMInputTextEdit.cpp \
    View/IMFriendButton.cpp \
    View/IMInformationWidget.cpp \
    View/IMToolBox.cpp \
    View/IMFriendListWidget.cpp \
    View/IMLatestMessageListWidget.cpp \
    View/IMLinkButton.cpp \
    View/IMLinkFriendButton.cpp \
    View/IMLinkFlockButton.cpp \
    View/IMLinkDiscussionButton.cpp \
    View/IMFlockButton.cpp \
    View/IMDiscussionButton.cpp \
    View/IMMessageWidget.cpp \
    View/IMLocalMessageWidget.cpp \
    View/IMNetworkMessageWidget.cpp \
    View/IMFlockMemberButton.cpp \
    View/IMDiscussionMemberButton.cpp \
    View/IMDiscussionInformationWidget.cpp \
    View/IMFlockInformationWidget.cpp \
    View/IMFlockListWidget.cpp \
    View/IMDiscussionListWidget.cpp \
    View/IMDiscussionMemberListWidget.cpp \
    View/IMFlockMemberListWidget.cpp \
    View/IMLittleWidget.cpp \
    View/IMMessageManageWidget.cpp \
    View/IMSearchListWidget.cpp \
    View/IMMailListWidget.cpp \
    View/IMMailWidget.cpp \
    View/IMMailButton.cpp \
    View/IMMailInWidget.cpp \
    View/IMMailOutWidget.cpp \
    View/IMMailWriteWidget.cpp \
    View/IMMailInformationWidget.cpp \
    View/IMFileReceiverWidget.cpp \
    View/IMFileSenderWidget.cpp \
    Control/IMMailCtrl.cpp

HEADERS  += mainwindow.h \
    View/IMLoginWidget.h \
    Control/IMLoginCtrl.h \
    Model/IMContant.h \
    Model/IMTcpSocket.h \
    Model/IMEncryption.h \
    View/IMClickLabel.h \
    View/IMRegisterWidget.h \
    Control/IMRegisterCtrl.h \
    Control/IMClientFileCtrl.h \
    View/CalendarDialog.h \
    View/CustomLineEdit.h \
    View/CustomCalendar.h \
    View/CustomComboBox.h \
    View/IMMainWidget.h \
    Control/IMMainCtrl.h \
    Control/IMDatabaseCtrl.h \
    View/IMChatWidget.h \
    View/IMFlockChatWidget.h \
    View/IMInputTextEdit.h \
    View/IMFriendButton.h \
    View/IMInformationWidget.h \
    View/IMToolBox.h \
    View/IMFriendListWidget.h \
    View/IMLatestMessageListWidget.h \
    View/IMLinkButton.h \
    View/IMLinkFriendButton.h \
    View/IMLinkFlockButton.h \
    View/IMLinkDiscussionButton.h \
    View/IMFlockButton.h \
    View/IMDiscussionButton.h \
    View/IMMessageWidget.h \
    View/IMLocalMessageWidget.h \
    View/IMNetworkMessageWidget.h \
    View/IMDiscussionChatWidget.h \
    View/IMFlockMemberButton.h \
    View/IMDiscussionMemberButton.h \
    View/IMDiscussionInformationWidget.h \
    View/IMFlockInformationWidget.h \
    View/IMFlockListWidget.h \
    View/IMDiscussionListWidget.h \
    View/IMDiscussionMemberListWidget.h \
    View/IMFlockMemberListWidget.h \
    View/IMLittleWidget.h \
    View/IMMessageManageWidget.h \
    View/IMSearchListWidget.h \
    View/IMMailListWidget.h \
    View/IMMailWidget.h \
    View/IMMailButton.h \
    View/IMMailInWidget.h \
    View/IMMailOutWidget.h \
    View/IMMailWriteWidget.h \
    View/IMMailInformationWidget.h \
    View/IMFileReceiverWidget.h \
    View/IMFileSenderWidget.h \
    Control/IMMailCtrl.h

RESOURCES += \
    resource.qrc

