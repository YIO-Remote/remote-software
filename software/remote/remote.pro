QT += qml quick websockets quickcontrols2
CONFIG += c++11 disable-desktop qtquickcompiler

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
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
    sources/notificationsinterface.h

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
    sources/notifications.cpp

equals(QT_ARCH, arm): {
    HEADERS += \
        sources/hardware/apds9960.h \
        sources/hardware/mcp23017.h \

    SOURCES += \
        sources/hardware/apds9960.cpp \
}

RESOURCES += qml.qrc \
    images.qrc \
    translations.qrc

# TRANSLATION
lupdate_only{
SOURCES = main.qml \
          MainContainer.qml \
          StandbyControl.qml \
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

# Additional import path used to resolve QML modules in Qt Creator's code model
#QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
#QML_DESIGNER_IMPORT_PATH =

# Wiringpi config
linux {
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
    DESTDIR = $$OUT_PWD/debug
    }
    CONFIG(release, debug|release) {
    DESTDIR = $$OUT_PWD/release
    }
}
macx {
    APP_QML_FILES.files = $$PWD/config.json $$PWD/translations.json
    APP_QML_FILES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += APP_QML_FILES

#    INTEGRATIONS.files = $$files($$PWD/plugins/*.*)
#    INTEGRATIONS.path = Contents/Resources/plugins
#    QMAKE_BUNDLE_DATA += INTEGRATIONS

} else {
    CONFIG += file_copies
    COPIES += extraData
    extraData.files = $$PWD/config.json $$PWD/translations.json
    extraData.path = $$DESTDIR
}

DISTFILES +=
