TEMPLATE        = lib
CONFIG         += plugin
QT             += websockets core quick
HEADERS         = homeassistant.h \
                  ../integration.h \
                  ../integrationinterface.h
SOURCES         = homeassistant.cpp
TARGET          = homeassistant
DESTDIR         = ../../../plugins

#DISTFILES += homeassistant.json

# install
unix {
    target.path = /usr/lib
    INSTALLS += target
}
