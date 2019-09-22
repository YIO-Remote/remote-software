QT+= core gui widgets network
TARGET = test_app
HEADERS= window.h
SOURCES= main.cpp window.cpp
DEFINES= QZEROCONF_STATIC

include(../qtzeroconf.pri)
