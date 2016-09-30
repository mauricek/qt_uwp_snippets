SOURCES += $$PWD/loginmanager.cpp

HEADERS += $$PWD/loginmanager.h

QT += core-private

#QMAKE_CXXFLAGS += /ZW
# Find platform.winmd
#LIBS += /LIBPATH:\"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\lib\store\references\"

QMAKE_CXXFLAGS += /ZW \
                  /AI\"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\lib\store\references\" \
                  /FU\"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\lib\store\references\platform.winmd\"
