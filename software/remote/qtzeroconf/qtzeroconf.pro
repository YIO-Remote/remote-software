QT = core network

include($$PWD/qtzeroconf.pri)

#VERSION = 1.1

TEMPLATE = lib
TARGET = $$qtLibraryTarget(QtZeroConf$$QT_LIBINFIX)
CONFIG += module create_prl
DEFINES+= QT_BUILD_ZEROCONF_LIB
mac:QMAKE_FRAMEWORK_BUNDLE_NAME = $$TARGET

headersDataFiles.files = $$PWD/qzeroconf.h $$PWD/qzeroconfservice.h $$PWD/qzeroconfglobal.h

# install to Qt installation directory if no PREFIX specified
_PREFIX = $$PREFIX
isEmpty(_PREFIX) {
	INSTALL_HEADER_PATH = $$[QT_INSTALL_HEADERS]/QtZeroConf/
	INSTALL_LIB_PATH = $$[QT_INSTALL_LIBS]
} else {

	INSTALL_HEADER_PATH = $$PREFIX/include/QtZeroConf/
	INSTALL_LIB_PATH = $$PREFIX/lib
}
message(install to: $$INSTALL_LIB_PATH)
headersDataFiles.path = $$INSTALL_HEADER_PATH
target.path = $$INSTALL_LIB_PATH

INSTALLS+= target headersDataFiles
