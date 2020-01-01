###############################################################################
 #
 # Copyright (C) 2019 Markus Zehnder <business@markuszehnder.ch>
 # Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 #
 # This file is part of the YIO-Remote software project.
 #
 # YIO-Remote software is free software: you can redistribute it and/or modify
 # it under the terms of the GNU General Public License as published by
 # the Free Software Foundation, either version 3 of the License, or
 # (at your option) any later version.
 #
 # YIO-Remote software is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 #
 # You should have received a copy of the GNU General Public License
 # along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 #
 # SPDX-License-Identifier: GPL-3.0-or-later
 #############################################################################/

QT += qml quick websockets quickcontrols2 bluetooth
CONFIG += c++14 disable-desktop
# disable qtquickcompiler for QML debugging!
CONFIG += qtquickcompiler

DEFINES += QT_DEPRECATED_WARNINGS

include(qmake-target-platform.pri)
include(qmake-destination-path.pri)

HEADERS += \
    components/media_player/sources/albummodel_mediaplayer.h \
    components/media_player/sources/searchmodel_mediaplayer.h \
    components/media_player/sources/utils_mediaplayer.h \
    sources/config.h \
    sources/configinterface.h \
    sources/entities/blindinterface.h \
    sources/entities/climate.h \
    sources/entities/climateinterface.h \
    sources/entities/entityinterface.h \
    sources/entities/lightinterface.h \
    sources/entities/mediaplayerinterface.h \
    sources/entities/remote.h \
    sources/entities/weather.h \
    sources/entities/weatherinterface.h \
    sources/fileio.h \
    sources/integrations/plugininterface.h \
    sources/integrations/integrations.h \
    sources/integrations/integrationsinterface.h \
    sources/integrations/integrationinterface.h \
    sources/jsonfile.h \
    sources/launcher.h \
    sources/hardware/display_control.h \
    sources/logger.h \
    sources/standbycontrol.h \
    sources/translation.h \
    sources/hardware/touchdetect.h \
    sources/hardware/proximity_gesture_control.h \
    sources/hardware/interrupt_handler.h \
    sources/hardware/drv2605.h \
    sources/hardware/bq27441.h \
    sources/hardware/hardwarefactory.h \
    sources/hardware/hardwarefactory_mock.h \
    sources/hardware/hw_config.h \
    sources/hardware/systemd.h \
    sources/hardware/systemservice.h \
    sources/hardware/systemservice_mock.h \
    sources/hardware/systemservice_name.h \
    sources/hardware/webserver_control.h \
    sources/hardware/webserver_lighttpd.h \
    sources/hardware/webserver_mock.h \
    sources/hardware/wifi_control.h \
    sources/hardware/wifi_mock.h \
    sources/hardware/wifi_network.h \
    sources/hardware/wifi_security.h \
    sources/hardware/wifi_signal.h \
    sources/hardware/wifi_status.h \
    sources/entities/entities.h \
    sources/entities/entity.h \
    sources/integrations/plugininterface.h \
    sources/entities/entitiesinterface.h \
    sources/entities/entityinterface.h \
    sources/entities/blindinterface.h \
    sources/entities/lightinterface.h \
    sources/entities/mediaplayerinterface.h \
    sources/entities/remoteinterface.h \
    sources/entities/light.h \
    sources/entities/blind.h \
    sources/notifications.h \
    sources/notificationsinterface.h \
    sources/entities/mediaplayer.h \
    sources/bluetootharea.h \
    sources/utils.h \
    sources/yioapi.h \
    sources/yioapiinterface.h

SOURCES += \
    components/media_player/sources/albummodel_mediaplayer.cpp \
    components/media_player/sources/searchmodel_mediaplayer.cpp \
    components/media_player/sources/utils_mediaplayer.cpp \
    sources/config.cpp \
    sources/entities/climate.cpp \
    sources/entities/remote.cpp \
    sources/entities/weather.cpp \
    sources/integrations/integrations.cpp \
    sources/logger.cpp \
    sources/main.cpp \
    sources/jsonfile.cpp \
    sources/launcher.cpp \
    sources/hardware/display_control.cpp \
    sources/hardware/drv2605.cpp \
    sources/hardware/bq27441.cpp \
    sources/hardware/hardwarefactory.cpp \
    sources/hardware/hardwarefactory_mock.cpp \
    sources/hardware/systemd.cpp \
    sources/hardware/systemservice.cpp \
    sources/hardware/systemservice_mock.cpp \
    sources/hardware/webserver_control.cpp \
    sources/hardware/webserver_lighttpd.cpp \
    sources/hardware/webserver_mock.cpp \
    sources/hardware/wifi_control.cpp \
    sources/hardware/wifi_mock.cpp \
    sources/entities/entities.cpp \
    sources/entities/entity.cpp \
    sources/entities/light.cpp \
    sources/entities/blind.cpp \
    sources/notifications.cpp \
    sources/entities/mediaplayer.cpp \
    sources/bluetootharea.cpp \
    sources/standbycontrol.cpp \
    sources/utils.cpp \
    sources/yioapi.cpp

RESOURCES += qml.qrc \
    images.qrc \
    keyboard.qrc \
    style.qrc \
    translations.qrc

# === platform specific devices =======================================
linux {
    USE_WPA_SUPPLICANT = y

    # TODO simplify defines
    equals(USE_WPA_SUPPLICANT, y): {
        DEFINES += CONFIG_WPA_SUPPLICANT \
            CONFIG_CTRL_IFACE=1 \
            CONFIG_CTRL_IFACE_UNIX=1

        INCLUDEPATH += wpa_supplicant/src wpa_supplicant/src/utils

        HEADERS += \
            sources/hardware/wifi_wpasupplicant.h

        SOURCES += \
            sources/hardware/wifi_wpasupplicant.cpp \
            wpa_supplicant/src/common/wpa_ctrl.c \
            wpa_supplicant/src/utils/os_unix.c

    } else {
        HEADERS +=
            sources/hardware/wifi_mock.h
        SOURCES +=
            sources/hardware/wifi_mock.cpp
    }

    HEADERS += \
        sources/hardware/wifi_shellscripts.h \
        sources/hardware/hardwarefactory_rpi0.h
    SOURCES += \
        sources/hardware/wifi_shellscripts.cpp \
        sources/hardware/hardwarefactory_rpi0.cpp

    equals(QT_ARCH, arm): {
        HEADERS += \
            sources/hardware/apds9960.h \
            sources/hardware/mcp23017.h

        SOURCES += \
            sources/hardware/apds9960.cpp
    }

}

# === start TRANSLATION section =======================================
lupdate_only{
SOURCES = main.qml \
          MainContainer.qml \
          StandbyControl.qml \
          ButtonHandler.qml \
          wifiSetup.qml \
          basic_ui/*.qml \
          basic_ui/pages/*.qml \
          basic_ui/settings/*.qml \
          components/light/ui/*.qml \
          components/blind/ui/*.qml \
          components/media_player/ui/*.qml \
          components/weather/ui/*.qml \
          components/remote/ui/*.qml \
          components/media_player/ui/*.qml \
          components/climate/ui/*.qml \
          sources/proximity_gesture_control.h
}

TRANSLATIONS = translations/bg_BG.ts \
               translations/cs_CZ.ts \
               translations/da_DK.ts \
               translations/de_DE.ts \
               translations/el_GR.ts \
               translations/en_US.ts \
               translations/es_ES.ts \
               translations/et_EE.ts \
               translations/fi_FI.ts \
               translations/fr_CA.ts \
               translations/fr_FR.ts \
               translations/ga_IE.ts \
               translations/hr_HR.ts \
               translations/hu_HU.ts \
               translations/it_IT.ts \
               translations/lt_LT.ts \
               translations/lv_LV.ts \
               translations/mt_MT.ts \
               translations/nl_NL.ts \
               translations/no_NO.ts \
               translations/pl_PL.ts \
               translations/pt_BR.ts \
               translations/pt_PT.ts \
               translations/ro_RO.ts \
               translations/sk_SK.ts \
               translations/sl_SI.ts \
               translations/sv_SE.ts

# lupdate & lrelease integration in qmake is a major pain to get working on Linux, macOS, Windows PLUS Linux arm cross compile PLUS qmake / make cmd line!
# There are so many different ways and each one works great on SOME platform(s) only :-(
# So this here might look excessive but I found no other reliable way to make it work on as many environments as possible...
# 1.) Check if we get the linguist cmd line tools from the QT installation (works in Qt Creator on Linux, macOS and Win but not with Buildroot / Linux crosscompile)
exists($$[QT_INSTALL_BINS]/lupdate):QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate
exists($$[QT_INSTALL_BINS]/lrelease):QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
  # think about our Windows friends
exists($$[QT_INSTALL_BINS]/lupdate.exe):QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate.exe
exists($$[QT_INSTALL_BINS]/lrelease.exe):QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease.exe
# 2.) Check if it's available from $HOST_DIR env var which is set during Buildroot. Only use it if it's not already defined (*=).
exists($$(HOST_DIR)/bin/lupdate):QMAKE_LUPDATE *= $$(HOST_DIR)/bin/lupdate
exists($$(HOST_DIR)/bin/lrelease):QMAKE_LRELEASE *= $$(HOST_DIR)/bin/lrelease
# 3.) Linux Qt Creator arm cross compile: QT_INSTALL_BINS is NOT available, but host tools should be available in QTDIR
exists($$(QTDIR)/bin/lupdate):QMAKE_LUPDATE *= $$(QTDIR)/bin/lupdate
exists($$(QTDIR)/bin/lrelease):QMAKE_LRELEASE *= $$(QTDIR)/bin/lrelease
# 4.) Fallback: custom env var QT_LINGUIST_DIR (which can also be used to override the tools found in the path)
exists($$(QT_LINGUIST_DIR)/lupdate):QMAKE_LUPDATE *= $$(QT_LINGUIST_DIR)/lupdate
exists($$(QT_LINGUIST_DIR)/lrelease):QMAKE_LRELEASE *= $$(QT_LINGUIST_DIR)/lrelease
# 5.) Last option: check path, plain and simple. (Would most likely be enough on most systems...)
if(isEmpty(QMAKE_LUPDATE)) {
    win32:QMAKE_LUPDATE    = $$system(where lupdate)
    unix|mac:QMAKE_LUPDATE = $$system(which lupdate)
}
if(isEmpty(QMAKE_LRELEASE)) {
    win32:QMAKE_LRELEASE    = $$system(where lrelease)
    unix|mac:QMAKE_LRELEASE = $$system(which lrelease)
}

!isEmpty(QMAKE_LUPDATE):exists("$$QMAKE_LUPDATE") {
    message("Using Qt linguist tools: '$$QMAKE_LUPDATE', '$$QMAKE_LRELEASE'")
    command = $$QMAKE_LUPDATE remote.pro
    system($$command) | error("Failed to run: $$command")
    command = $$QMAKE_LRELEASE remote.pro
    system($$command) | error("Failed to run: $$command")
} else {
    warning("Qt linguist cmd line tools lupdate / lrelease not found: translations will NOT be compiled and build will most likely fail due to missing .qm files!")
}

# === end TRANSLATION section =========================================

# include zeroconf
include(qtzeroconf/qtzeroconf.pri)
DEFINES += QZEROCONF_STATIC

# Wiringpi config, only on raspberry pi
equals(QT_ARCH, arm): {
    message(Cross compiling for arm system: including Wiringpi config on RPi)

    # FIXME hard coded directory path!
    INCLUDEPATH += /buildroot/buildroot-remote/output/target/usr/lib/
    LIBS += -L"/buildroot/buildroot-remote/output/target/usr/lib"
    LIBS += -lwiringPi
}

# include valijson
include(3rdparty/valijson.pri)

# Configure destination path. DESTDIR is set in qmake-destination-path.pri
OBJECTS_DIR = $$PWD/build/$$DESTINATION_PATH/obj
MOC_DIR = $$PWD/build/$$DESTINATION_PATH/moc
RCC_DIR = $$PWD/build/$$DESTINATION_PATH/qrc
UI_DIR = $$PWD/build/$$DESTINATION_PATH/ui

# Default rules for deployment.
# This enables RPi device deployment in Qt Creator
qnx: target.path = /tmp/$${TARGET}/bin
else: linux: target.path = /opt/$${TARGET}/bin  # A different target path is used on purpose to leave the installed app untouched

# Wildcards or directories just don't work: One has to know the magic $$files() command!
targetConfig.files = *.json
targetConfig.path = $$target.path
targetFonts.files = $$files($$PWD/fonts/*.*)
targetFonts.path = $$target.path/fonts
targetIcons.files = $$files($$PWD/icons/*.*)
targetIcons.path = $$target.path/icons
# The integration projects have to store the binary plugin in the destination folder.
# Note: restart Qt Creator if it doesn't pick up new plugins!
targetPlugins.files = $$files($$DESTDIR/plugins/*.*)
targetPlugins.path = $$target.path/plugins

!isEmpty(target.path): INSTALLS += target
!isEmpty(target.path): INSTALLS += targetConfig
!isEmpty(target.path): INSTALLS += targetFonts
!isEmpty(target.path): INSTALLS += targetIcons
!isEmpty(target.path): INSTALLS += targetPlugins

win32 {
    CONFIG += file_copies
    COPIES += extraData
    extraData.files = $$PWD/config.json $$PWD/config-schema.json $$PWD/hardware.json $$PWD/hardware-schema.json $$PWD/translations.json
    extraData.path = $$DESTDIR

    #copy fonts
    COPIES += fonts
    fonts.files = $$files($$PWD/fonts/*.*)
    fonts.path = $$DESTDIR/fonts

    #copy icons
    COPIES += icons
    icons.files = $$files($$PWD/icons/*.*)
    icons.path = $$DESTDIR/icons

    #plugins are already stored in DESTDIR by the integration projects

    # TODO Windows application icon
    #RC_ICONS = icons/windows.ico
} else:linux {
    CONFIG += file_copies
    COPIES += extraData
    extraData.files = $$PWD/config.json $$PWD/config-schema.json $$PWD/hardware.json $$PWD/hardware-schema.json $$PWD/translations.json
    extraData.path = $$DESTDIR

    #copy fonts
    COPIES += fonts
    fonts.files = $$files($$PWD/fonts/*.*)
    fonts.path = $$DESTDIR/fonts

    #copy icons
    COPIES += icons
    icons.files = $$files($$PWD/icons/*.*)
    icons.path = $$DESTDIR/icons

    #plugins are already stored in DESTDIR by the integration projects

    # create deployment archive for RPi image build only
    # usage: make install_tarball
    equals(QT_ARCH, arm): {
        tarball.path = $$DESTDIR_BIN
        message( Storing tarball in $$tarball.path/$${TARGET}-$${platform_path}_$${processor_path}_$${build_path}.tar )
        tarball.extra = mkdir -p $$tarball.path; tar -cvf $$tarball.path/$${TARGET}-$${platform_path}_$${processor_path}_$${build_path}.tar -C $$DESTDIR .
        INSTALLS += tarball
    }

} else:macx {
    APP_QML_FILES.files = $$PWD/config.json $$PWD/config-schema.json $$PWD/hardware.json $$PWD/hardware-schema.json $$PWD/translations.json
    APP_QML_FILES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += APP_QML_FILES

    # re-package plugin files into app bundle
    INTEGRATIONS.files = $$files($$DESTDIR/plugins/*.*)
    INTEGRATIONS.path = Contents/Resources/plugins
    QMAKE_BUNDLE_DATA += INTEGRATIONS

    #copy fonts
    FONTS.files = $$files($$PWD/fonts/*.*)
    FONTS.path = Contents/Resources/fonts
    QMAKE_BUNDLE_DATA += FONTS

    #copy icons
    ICONS.files = $$files($$PWD/icons/*.*)
    ICONS.path = Contents/Resources/icons
    QMAKE_BUNDLE_DATA += ICONS

    # TODO macOS application icon
    #ICON=icons/macos.icns
} else {
    error(unknown platform! Platform must be configured in remote.pro project file)
}

DISTFILES +=

