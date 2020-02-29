###############################################################################
 #
 # Copyright (C) 2019-2020 Markus Zehnder <business@markuszehnder.ch>
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

# SOFTWARE VERSION
VERSION = 0.2.3
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

QT += qml quick websockets quickcontrols2 bluetooth
CONFIG += c++14 disable-desktop
#disable qtquickcompiler for QML debugging!
CONFIG += qtquickcompiler

DEFINES += QT_DEPRECATED_WARNINGS

# === Load integration interface library ======================================
# Workaround for 'lupdate remote.pro' bug when using variables for including a .pri file:
# "Project ERROR: COPIES entry extraData defines no .path"
# This only happens if the variable is assigned more then once, even in an else block!
# WHY is lupdate integration such a major pain?
INTG_LIB_PATH = $$(YIO_SRC)
isEmpty(INTG_LIB_PATH) {
    message("Environment variables YIO_SRC not defined! Using '$$clean_path($$PWD/..)' for integrations.library project.")
    # === Integration interface library: all plugin headers and shared data models
    ! include($$clean_path($$PWD/../integrations.library/yio-interfaces.pri)) {
        error( "Couldn't find the yio-interfaces.pri file!" )
    }
    ! include($$clean_path($$PWD/../integrations.library/yio-model-mediaplayer.pri)) {
        error( "Couldn't find the yio-model-mediaplayer.pri file!" )
    }
    ! include($$clean_path($$PWD/../integrations.library/yio-model-weather.pri)) {
        error( "Couldn't find the yio-model-weather.pri file!" )
    }
    # === QMake helper functions for output path based on platform & compiler
    ! include($$clean_path($$PWD/../integrations.library/qmake-destination-path.pri)) {
        error( "Couldn't find the qmake-destination-path.pri file!" )
    }
} else {
    message("YIO_SRC is set: using '$$(YIO_SRC)/integrations.library' for integrations.library project.")
    ! include($$(YIO_SRC)/integrations.library/yio-interfaces.pri) {
        error( "Couldn't find the yio-interfaces.pri file!" )
    }
    ! include($$(YIO_SRC)/integrations.library/yio-model-mediaplayer.pri) {
        error( "Couldn't find the yio-model-mediaplayer.pri file!" )
    }
    ! include($$(YIO_SRC)/integrations.library/yio-model-weather.pri) {
        error( "Couldn't find the yio-model-weather.pri file!" )
    }
    ! include($$(YIO_SRC)/integrations.library/qmake-destination-path.pri) {
        error( "Couldn't find the qmake-destination-path.pri file!" )
    }
}
# =============================================================================

HEADERS += \
    components/media_player/sources/utils_mediaplayer.h \
    sources/commandlinehandler.h \
    sources/config.h \
    sources/configutil.h \
    sources/entities/climate.h \
    sources/entities/remote.h \
    sources/entities/switch.h \
    sources/entities/weather.h \
    sources/fileio.h \
    sources/hardware/batterycharger.h \
    sources/hardware/batteryfuelgauge.h \
    sources/hardware/buttonhandler.h \
    sources/hardware/hardwarefactory_default.h \
    sources/hardware/mock/batterycharger_mock.h \
    sources/hardware/mock/batteryfuelgauge_mock.h \
    sources/hardware/displaycontrol.h \
    sources/hardware/mock/displaycontrol_mock.h \
    sources/hardware/gesturesensor.h \
    sources/hardware/mock/gesturesensor_mock.h \
    sources/hardware/hapticmotor.h \
    sources/hardware/mock/hapticmotor_mock.h \
    sources/hardware/mock/interrupthandler_mock.h \
    sources/hardware/lightsensor.h \
    sources/hardware/mock/lightsensor_mock.h \
    sources/hardware/proximitysensor.h \
    sources/hardware/mock/proximitysensor_mock.h \
    sources/integrations/integrations.h \
    sources/integrations/integrationsinterface.h \
    sources/jsonfile.h \
    sources/launcher.h \
    sources/logger.h \
    sources/softwareupdate.h \
    sources/standbycontrol.h \
    sources/translation.h \
    sources/hardware/device.h \
    sources/hardware/touchdetect.h \
    sources/hardware/hardwarefactory.h \
    sources/hardware/hw_config.h \
    sources/hardware/interrupthandler.h \
    sources/hardware/systemservice.h \
    sources/hardware/mock/systemservice_mock.h \
    sources/hardware/systemservice_name.h \
    sources/hardware/webserver_control.h \
    sources/hardware/mock/webserver_mock.h \
    sources/hardware/wifi_control.h \
    sources/hardware/mock/wifi_mock.h \
    sources/hardware/wifi_network.h \
    sources/hardware/wifi_security.h \
    sources/hardware/wifi_signal.h \
    sources/hardware/wifi_status.h \
    sources/entities/entities.h \
    sources/entities/entity.h \
    sources/entities/light.h \
    sources/entities/blind.h \
    sources/notifications.h \
    sources/entities/mediaplayer.h \
    sources/bluetootharea.h \
    sources/utils.h \
    sources/yioapi.h

SOURCES += \
    components/media_player/sources/utils_mediaplayer.cpp \
    sources/commandlinehandler.cpp \
    sources/config.cpp \
    sources/configutil.cpp \
    sources/entities/climate.cpp \
    sources/entities/remote.cpp \
    sources/entities/switch.cpp \
    sources/entities/weather.cpp \
    sources/hardware/buttonhandler.cpp \
    sources/hardware/device.cpp \
    sources/hardware/hardwarefactory_default.cpp \
    sources/hardware/touchdetect.cpp \
    sources/integrations/integrations.cpp \
    sources/logger.cpp \
    sources/main.cpp \
    sources/jsonfile.cpp \
    sources/launcher.cpp \
    sources/hardware/hardwarefactory.cpp \
    sources/hardware/systemservice.cpp \
    sources/hardware/mock/systemservice_mock.cpp \
    sources/hardware/webserver_control.cpp \
    sources/hardware/mock/webserver_mock.cpp \
    sources/hardware/wifi_control.cpp \
    sources/hardware/mock/wifi_mock.cpp \
    sources/entities/entities.cpp \
    sources/entities/entity.cpp \
    sources/entities/light.cpp \
    sources/entities/blind.cpp \
    sources/notifications.cpp \
    sources/entities/mediaplayer.cpp \
    sources/bluetootharea.cpp \
    sources/softwareupdate.cpp \
    sources/standbycontrol.cpp \
    sources/utils.cpp \
    sources/yioapi.cpp

RESOURCES += qml.qrc \
    images.qrc \
    keyboard.qrc \
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
            sources/hardware/linux/wifi_wpasupplicant.h

        SOURCES += \
            sources/hardware/linux/wifi_wpasupplicant.cpp \
            wpa_supplicant/src/common/wpa_ctrl.c \
            wpa_supplicant/src/utils/os_unix.c

    }

    HEADERS += \
        sources/hardware/linux/hw_factory_linux.h \
        sources/hardware/linux/systemd.h \
        sources/hardware/linux/webserver_lighttpd.h \
        sources/hardware/linux/wifi_shellscripts.h
    SOURCES += \
        sources/hardware/linux/hw_factory_linux.cpp \
        sources/hardware/linux/systemd.cpp \
        sources/hardware/linux/webserver_lighttpd.cpp \
        sources/hardware/linux/wifi_shellscripts.cpp

    equals(QT_ARCH, arm): {
        HEADERS += \
            sources/hardware/linux/arm/hw_factory_yio.h \
            sources/hardware/linux/arm/apds9960.h \
            sources/hardware/linux/arm/apds9960gesture.h \
            sources/hardware/linux/arm/apds9960light.h \
            sources/hardware/linux/arm/apds9960proximity.h \
            sources/hardware/linux/arm/batterycharger_yio.h \
            sources/hardware/linux/arm/bq27441.h \
            sources/hardware/linux/arm/displaycontrol_yio.h \
            sources/hardware/linux/arm/drv2605.h \
            sources/hardware/linux/arm/mcp23017_interrupt.h \
            sources/hardware/linux/arm/mcp23017_handler.h

        SOURCES += \
            sources/hardware/linux/arm/hw_factory_yio.cpp \
            sources/hardware/linux/arm/apds9960.cpp \
            sources/hardware/linux/arm/apds9960light.cpp \
            sources/hardware/linux/arm/apds9960proximity.cpp \
            sources/hardware/linux/arm/batterycharger_yio.cpp \
            sources/hardware/linux/arm/bq27441.cpp \
            sources/hardware/linux/arm/displaycontrol_yio.cpp \
            sources/hardware/linux/arm/drv2605.cpp \
            sources/hardware/linux/arm/mcp23017_interrupt.cpp
    }

}

# Android specific files (empty template for now)
android {
    HEADERS += \
        sources/hardware/android/hw_factory_android.h
    SOURCES += \
        sources/hardware/android/hw_factory_android.cpp
}
# macOS specific files (empty template for now)
macx {
    HEADERS += \
        sources/hardware/macos/hw_factory_mac.h
    SOURCES += \
        sources/hardware/macos/hw_factory_mac.cpp
}
# Windows specific files (empty template for now)
win32 {
    HEADERS += \
        sources/hardware/windows/hw_factory_win.h
    SOURCES += \
        sources/hardware/windows/hw_factory_win.cpp
}

# === start TRANSLATION section =======================================
lupdate_only{
SOURCES += $$OTHER_FILES

OTHER_FILES += main.qml \
          MainContainer.qml \
          basic_ui/*.qml \
          basic_ui/pages/*.qml \
          basic_ui/settings/*.qml \
          components/light/ui/*.qml \
          components/blind/ui/*.qml \
          components/media_player/ui/*.qml \
          components/weather/ui/*.qml \
          components/remote/ui/*.qml \
          components/media_player/ui/*.qml \
          components/climate/ui/*.qml
}

TRANSLATIONS = translations/bg_BG.ts \
               translations/ca_ES.ts \
               translations/cs_CZ.ts \
               translations/da_DK.ts \
               translations/de_DE.ts \
               translations/el_GR.ts \
               translations/en_US.ts \
               translations/es_ES.ts \
               translations/et_EE.ts \
               translations/fi_FI.ts \
               translations/fr_FR.ts \
               translations/ga_IE.ts \
               translations/hr_HR.ts \
               translations/hu_HU.ts \
               translations/is_IS.ts \
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
               translations/ru_BY.ts \
               translations/ru_MD.ts \
               translations/ru_RU.ts \
               translations/ru_UA.ts \
               translations/sk_SK.ts \
               translations/sl_SI.ts \
               translations/sv_SE.ts \
               translations/zh_CN.ts \
               translations/zh_TW.ts

#QMAKE_LUPDATE & _LRELEASE vars are set in qmake-destiation-path.pri
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
    INCLUDEPATH += /home/yio/projects/yio/remote-os/rpi0/output/host/linux/arm-buildroot-linux-gnueabihf/sysroot/usr/include
    LIBS += -L"/home/yio/projects/yio/remote-os/rpi0/output/target/usr/lib"
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
    extraData.files = $$PWD/config.json $$PWD/config-schema.json $$PWD/hardware.json $$PWD/hardware-schema.json $$PWD/translations.json $$PWD/locale.json
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
    extraData.files = $$PWD/config.json $$PWD/config-schema.json $$PWD/hardware.json $$PWD/hardware-schema.json $$PWD/translations.json $$PWD/locale.json
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
    APP_QML_FILES.files = $$PWD/config.json $$PWD/config-schema.json $$PWD/hardware.json $$PWD/hardware-schema.json $$PWD/translations.json $$PWD/locale.json
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

