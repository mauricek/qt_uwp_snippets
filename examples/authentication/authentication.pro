######################################################################
# Automatically generated by qmake (3.0) Thu Sep 29 11:20:15 2016
######################################################################

TEMPLATE = app
TARGET = authentication
INCLUDEPATH += .
INCLUDEPATH += ../../authentication
QT += widgets

# Input
SOURCES += main.cpp \
    form.cpp

#include(../../authentication/authentication.pri)
#include(../../authentication_async/authentication_async.pri)
include(../../authentication_cxx/authentication_cxx.pri)

FORMS += \
    form.ui

HEADERS += \
    form.h

