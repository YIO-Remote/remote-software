TEMPLATE        = lib
CONFIG         += plugin
QT             += core quick
HEADERS         = homeassistant.h \
                  ../integration.h \
                  ../integrationinterface.h \
                  ../../entities/entitiesinterface.h \
                  ../../entities/entity.h
SOURCES         = homeassistant.cpp \
                  ../../entities/entity.cpp
TARGET          = homeassistant
DESTDIR         = ../../../plugins

DISTFILES += homeassistant.json

# install
target.path = /usr/bin/yio-remote/plugins/
INSTALLS += target

