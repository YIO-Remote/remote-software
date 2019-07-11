TEMPLATE        = lib
CONFIG         += plugin
QT             += core quick
HEADERS         = openhab.h \
                  ../integration.h \
                  ../integrationinterface.h
SOURCES         = openhab.cpp
TARGET          = openhab
DESTDIR         = ../../../plugins

# install
unix {
    target.path = /usr/lib
    INSTALLS += target
}
