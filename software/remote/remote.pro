QT += qml quick websockets quickcontrols2 bluetooth
CONFIG += c++11 disable-desktop qtquickcompiler

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    sources/fileio.h \
    sources/jsonfile.h \
    sources/launcher.h \
    sources/hardware/display_control.h \
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
    sources/yioapi.h

SOURCES += \
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
          basic_ui/settings/*.qml \
          components/light/ui/*.qml \
          components/blind/ui/*.qml \
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

# include zeroconf
include(qtzeroconf/qtzeroconf.pri)
DEFINES = QZEROCONF_STATIC

# Wiringpi config, only on raspberry pi
unix {
    INCLUDEPATH += /buildroot/buildroot-remote/output/target/usr/lib/
    LIBS += -L"/buildroot/buildroot-remote/output/target/usr/lib"
    LIBS += -lwiringPi
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Configure default JSON destination path
DESTDIR = $$OUT_PWD

win32 {
    CONFIG(debug, debug|release) {
    DESTDIR = $$DESTDIR/debug
    }
    CONFIG(release, debug|release) {
    DESTDIR = $$DESTDIR/release
    }

    # copy plugin files
    CONFIG += file_copies
    COPIES += plugins
    plugins.files = $$files($$PWD/plugins/*.*)
    plugins.path = $$DESTDIR/release/plugins
}
macx {
    APP_QML_FILES.files = $$PWD/config.json $$PWD/translations.json
    APP_QML_FILES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += APP_QML_FILES

    # copy plugin files
    INTEGRATIONS.files = $$files($$PWD/plugins/*.*)
    INTEGRATIONS.path = Contents/Resources/plugins
    QMAKE_BUNDLE_DATA += INTEGRATIONS

} else {
    CONFIG += file_copies
    COPIES += extraData
    extraData.files = $$PWD/config.json $$PWD/translations.json
    extraData.path = $$DESTDIR

    # copy plugin files
    COPIES += plugins
    plugins.files = $$files($$PWD/plugins/*.*)
    plugins.path = $$DESTDIR/plugins
}
