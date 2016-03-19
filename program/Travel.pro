#-------------------------------------------------
#
# Project created by QtCreator 2016-03-10T12:38:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Travel
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    schedule.cpp \
    attribute.cpp \
    traveler.cpp \
    mapwidget.cpp \
    point.cpp

HEADERS  += widget.h \
    schedule.h \
    attribute.h \
    traveler.h \
    mapwidget.h \
    point.h

FORMS    += widget.ui

RESOURCES += resource.qrc \
    vehicle-icons.qrc


