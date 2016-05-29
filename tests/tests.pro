#-------------------------------------------------
#
# Project created by QtCreator 2016-05-30T00:27:04
#
#-------------------------------------------------

QT       += testlib core gui widgets



TARGET = tst_assignment_3
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_assignment_3.cpp \
    ../universebody.cpp \
    ../universecomposite.cpp \
    ../config.cpp \
    ../zodiac.cpp \
    ../universecomponentfactory.cpp \
    ../renderer2d.cpp \
    ../eventhandler.cpp \
    ../renderer2deventhandlers.cpp

HEADERS  += ../universecomponent.h \
    ../universecomposite.h \
    ../universebody.h \
    ../config.h \
    ../zodiac.h \
    ../universecomponentfactory.h \
    ../renderer.h \
    ../renderer2d.h \
    ../eventhandler.h \
    ../renderer2deventhandlers.h
DEFINES += SRCDIR=\\\"$$PWD/\\\"
