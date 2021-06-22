#-------------------------------------------------
#
# Project created by QtCreator 2018-05-18T03:36:29
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sshout-qt
TEMPLATE = app

SRC = src
DESTDIR = build
OBJECTS_DIR = $$DESTDIR
MOC_DIR = $$DESTDIR
UI_DIR += $$DESTDIR
RCC_DIR += $$DESTDIR


SOURCES += \
    $$SRC/main.cpp\
    $$SRC/connectionwindow.cpp \
    $$SRC/settingsdialog.cpp \
    $$SRC/mainwindow.cpp \
    $$SRC/sshclient.cpp \
    $$SRC/externalsshclient.cpp \
    $$SRC/packet.cpp \
    $$SRC/messagelog.cpp

HEADERS  += \
    $$SRC/connectionwindow.h \
    $$SRC/settingsdialog.h \
    $$SRC/mainwindow.h \
    $$SRC/serverinformation.h \
    $$SRC/sshclient.h \
    $$SRC/externalsshclient.h \
    $$SRC/common.h \
    $$SRC/packet.h \
    $$SRC/messagelog.h

FORMS    += \
    $$SRC/connectionwindow.ui \
    $$SRC/settingsdialog.ui \
    $$SRC/mainwindow.ui

#DEFINES += __STDC__

RESOURCES += \
    resources/icons.qrc

QMAKE_CXXFLAGS += -Wno-empty-body -Wno-write-strings

TRANSLATIONS = resources/translations/sshout.zh-cn.ts

CODECFORTR = UTF-8
