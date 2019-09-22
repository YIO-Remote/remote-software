TEMPLATE        = lib
CONFIG         += plugin
QT             += websockets core quick
HEADERS         = homey.h \
                  ../integration.h \
                  ../integrationinterface.h
SOURCES         = homey.cpp
TARGET          = homey
DESTDIR         = ../../../plugins

# install
unix {
    target.path = /usr/lib
    INSTALLS += target
}
