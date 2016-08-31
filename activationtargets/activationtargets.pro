QT += widgets
TARGET = qtactivationfilter

SOURCES      = main.cpp \
    activationfilter.cpp \
    form.cpp

HEADERS += \
    activationfilter.h \
    form.h

FORMS += \
    form.ui

WINRT_MANIFEST = $$PWD/AppxManifest.xml.in

