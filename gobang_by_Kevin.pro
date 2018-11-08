#-------------------------------------------------
#
# Project created by QtCreator 2018-08-10T02:03:04
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
    base.cpp \
    aithread_base.cpp \
    aithread_algo.cpp \
    aithread_eval.cpp \
    chess_board.cpp \
    main_window_game.cpp \
    main_window_init.cpp \
    main_window_ui.cpp \
    aithread_special.cpp \
    game_painter.cpp \
    ui_pushbutton.cpp

HEADERS += \
    base.h \
    aithread.h \
    aithread_macro.h \
    main_window.h \
    chess_board.h \
    ui_size_param.h \
    game_painter.h \
    ui_pushbutton.h \
    ui_style.h

QMAKE_CXXFLAGS +=  -Wno-unused-parameter

CONFIG += C++11
