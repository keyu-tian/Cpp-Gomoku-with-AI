#-------------------------------------------------
#
# Project created by QtCreator 2018-08-26T01:03:04
#
#                                  By Kevin
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gobang_by_Kevin
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    chessboard.cpp \
    base.cpp \
    mypainter.cpp \
    mypushbutton.cpp \
    aithread_base.cpp \
    aithread_algo.cpp \
    aithread_eval.cpp \
    aithread_opening.cpp

HEADERS += \
        mainwindow.h \
    chessboard.h \
    base.h \
    mypainter.h \
    my_style.h \
    mypushbutton.h \
    aithread.h \
    aithread_macro.h

QMAKE_CXXFLAGS +=  -Wno-unused-parameter

CONFIG += C++11
