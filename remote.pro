QT += qml quick websockets quickcontrols2 bluetooth
CONFIG += c++11 disable-desktop qtquickcompiler

DEFINES += QT_DEPRECATED_WARNINGS

include(qmake-target-platform.pri)
include(qmake-destination-path.pri)

HEADERS += \
    components/media_player/sources/utils_mediaplayer.h \
    sources/config.h \
    sources/configinterface.h \
    sources/entities/remote.h \
    sources/fileio.h \
    sources/integrations/integrations.h \
    sources/integrations/integrationsinterface.h \
    sources/jsonfile.h \
    sources/launcher.h \
    sources/hardware/display_control.h \
    sources/logger.h \
    sources/translation.h \
    sources/hardware/touchdetect.h \
    sources/hardware/proximity_gesture_control.h \
    sources/hardware/interrupt_handler.h \
    sources/hardware/drv2605.h \
    sources/hardware/bq27441.h \
    sources/integrations/integration.h \
    sources/entities/entities.h \
    sources/entities/entity.h \
    sources/integrations/integrationinterface.h \
    sources/entities/entitiesinterface.h \
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
    components/media_player/sources/utils_mediaplayer.cpp \
    sources/config.cpp \
    sources/entities/remote.cpp \
    sources/integrations/integrations.cpp \
    sources/logger.cpp \
    sources/main.cpp \
    sources/jsonfile.cpp \
    sources/launcher.cpp \
    sources/hardware/display_control.cpp \
    sources/hardware/drv2605.cpp \
    sources/hardware/bq27441.cpp \
    sources/entities/entities.cpp \
    sources/entities/entity.cpp \
    sources/entities/light.cpp \
    sources/entities/blind.cpp \
    sources/notifications.cpp \
    sources/entities/mediaplayer.cpp \
    sources/bluetootharea.cpp \
    sources/utils.cpp \
    sources/yioapi.cpp

equals(QT_ARCH, arm): {
    HEADERS += \
        sources/hardware/apds9960.h \
        sources/hardware/mcp23017.h

    SOURCES += \
        sources/hardware/apds9960.cpp
}

RESOURCES += qml.qrc \
    images.qrc \
    keyboard.qrc \
    translations.qrc

# TRANSLATION
lupdate_only{
SOURCES = main.qml \
          MainContainer.qml \
          StandbyControl.qml \
          wifiSetup.qml \
          basic_ui/*.qml \
          basic_ui/pages/*.qml \
          basic_ui/settings/*.qml \
          components/light/ui/*.qml \
          components/blind/ui/*.qml \
          components/media_player/ui/*.qml \
          components/remote/ui/*.qml \
          components/media_player/ui/*.qml \
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

#command = lupdate remote.pro
#system($$command) | error("Failed to run: $$command")

#command = lrelease remote.pro
#system($$command) | error("Failed to run: $$command")

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

# Configure destination path by "Operating System/Compiler/Processor Architecture/Build Configuration"
DESTDIR = $$PWD/../binaries/$$DESTINATION_PATH
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
    extraData.files = $$PWD/config.json $$PWD/translations.json
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
    extraData.files = $$PWD/config.json $$PWD/translations.json
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
        tarball.path = $$PWD/../binaries
        message( Storing tarball in $$tarball.path/$${TARGET}-$${platform_path}_$${processor_path}_$${build_path}.tar )
        tarball.extra = mkdir -p $$tarball.path; tar -cvf $$tarball.path/$${TARGET}-$${platform_path}_$${processor_path}_$${build_path}.tar -C $$DESTDIR .
        INSTALLS += tarball
    }

} else:macx {
    APP_QML_FILES.files = $$PWD/config.json $$PWD/translations.json
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
    error(unknown platform! Platform must be configured in remote.pro)
}

DISTFILES += \
    basic_ui/CustomImageLoader.qml
