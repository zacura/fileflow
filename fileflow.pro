#-------------------------------------------------
#
# Project created by QtCreator 2015-01-30T21:40:00
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fileflow
TEMPLATE = app

FLAGS = -g -rdynamic -fstack-protector -Wl,-z,relro -Wl,-z,now -Wformat-security -Wall -pedantic

QMAKE_CXXFLAGS += -std=c++11 $$FLAGS
QMAKE_LFLAGS += $$FLAGS

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
        src/p2pclient.cpp \
        src/p2pnode.cpp \
    src/p2pfile.cpp \
    src/p2pgroup.cpp \
    src/groupmodel.cpp \
    src/nodemodel.cpp

HEADERS  += src/mainwindow.h \
        src/p2pclient.h \
        src/p2pnode.h \
    src/p2pfile.h \
    src/p2pgroup.h \
    src/groupmodel.h \
    src/nodemodel.h

FORMS    += forms/mainwindow.ui
