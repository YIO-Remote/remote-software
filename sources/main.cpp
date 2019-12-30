/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 *
 * This file is part of the YIO-Remote software project.
 *
 * YIO-Remote software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YIO-Remote software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

#include <QFile>
#include <QFileInfo>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtDebug>

#include "jsonfile.h"
#include "launcher.h"
#include "translation.h"

#include "hardware/hardwarefactory.h"
#include "hardware/displaycontrol.h"
#include "hardware/touchdetect.h"
#include "hardware/drv2605.h"
#include "hardware/proximity_gesture_control.h"
#include "hardware/touchdetect.h"
#include "hardware/wifi_control.h"

#include "entities/entities.h"
#include "integrations/integrations.h"

#include "bluetootharea.h"
#include "notifications.h"

#include "components/media_player/sources/utils_mediaplayer.h"
#include "config.h"
#include "fileio.h"
#include "logger.h"
#include "yioapi.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "main");

int main(int argc, char* argv[]) {
    qputenv("QML2_IMPORT_PATH", "/keyboard");
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    qputenv("QT_VIRTUALKEYBOARD_LAYOUT_PATH", "qrc:/keyboard/layouts");
    qputenv("QT_VIRTUALKEYBOARD_STYLE", "remotestyle");

    //    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication       app(argc, argv);
    QQmlApplicationEngine engine;

    // Get the applications dir path and expose it to QML (prevents setting the JSON config variable)
    QString appPath = app.applicationDirPath();
    QString macPath = QFileInfo(appPath + "/../").canonicalPath() + QString("/Contents/Resources");

    // In case the translation file does not exist at root level (which is copied via the .pro file)
    // we are probably running on a mac, which gives a different result for the applicationDirPath
    // (actually points to '.../remote.app/Contents/MacOS/remote')
    if ((!QFile::exists(appPath + "/translations.json")) && (QFile::exists(macPath + "/translations.json"))) {
        appPath = macPath;
    }
    engine.rootContext()->setContextProperty("appPath", appPath);

    // Determine configuration path
    QString configPath = appPath;
    if (QFile::exists("/boot/config.json")) {
        configPath = "/boot";
    }
    engine.rootContext()->setContextProperty("configPath", configPath);

    // LOAD CONFIG
    QString schemaPath = appPath + "/config-schema.json";
    if (!QFile::exists(schemaPath)) {
        qCWarning(CLASS_LC) << "Configuration schema not found, configuration file will not be validated! Missing file:"
                            << schemaPath;
    }
    Config config(&engine, configPath, schemaPath);
    if (!config.isValid()) {
        qCCritical(CLASS_LC).noquote() << "Invalid configuration!" << endl << config.getError();
        // TODO(marton) show error screen with shutdon / reboot / web-configurator option
    }
    engine.rootContext()->setContextProperty("config", &config);

    // LOGGER
    QVariantMap logCfg = config.getSettings().value("logging").toMap();
    // "path" cfg logic:
    //   - key not set or "." => <application_path>/log
    //   - "" (empty)         => no log file
    //   - <otherwise>        => absolute directory path (e.g. "/var/log")
    QString path = logCfg.value("path", ".").toString();
    if (path == ".") {
        path = appPath + "/log";
    }
    Logger logger(path, logCfg.value("level", "WARN").toString(), logCfg.value("console", true).toBool(),
                  logCfg.value("showSource", true).toBool(), logCfg.value("queueSize", 100).toInt(),
                  logCfg.value("purgeHours", 72).toInt());
    engine.rootContext()->setContextProperty("logger", &logger);
    Logger::getInstance()->write("Logging started");

    // LOAD FONTS
    QFontDatabase::addApplicationFont(appPath + "/fonts/OpenSans-Light.ttf");
    QFontDatabase::addApplicationFont(appPath + "/fonts/OpenSans-Regular.ttf");
    QFontDatabase::addApplicationFont(appPath + "/fonts/OpenSans-SemiBold.ttf");
    QFontDatabase::addApplicationFont(appPath + "/fonts/OpenSans-Bold.ttf");

    // LOAD ICONS
    QFontDatabase::addApplicationFont(appPath + "/icons/icons.ttf");

    // LOADING CUSTOM COMPONENTS
    qmlRegisterType<Launcher>("Launcher", 1, 0, "Launcher");
    qmlRegisterType<JsonFile>("JsonFile", 1, 0, "JsonFile");

    qmlRegisterType<TouchEventFilter>("TouchEventFilter", 1, 0, "TouchEventFilter");
    qmlRegisterType<drv2605>("Haptic", 1, 0, "Haptic");
    qmlRegisterType<ProximityGestureControl>("Proximity", 1, 0, "Proximity");

    qmlRegisterUncreatableType<SystemServiceNameEnum>("SystemService", 1, 0, "SystemServiceNameEnum",
                                                      "Not creatable as it is an enum type");
    qRegisterMetaType<SystemServiceName>("SystemServiceName");
    qmlRegisterUncreatableType<WifiNetwork>("WifiControl", 1, 0, "WifiNetwork",
                                            "Not creatable as it is an information object only");
    qmlRegisterUncreatableType<WifiSecurityEnum>("WifiControl", 1, 0, "WifiSecurityEnum",
                                                 "Not creatable as it is an enum type");
    qRegisterMetaType<WifiSecurity>("WifiSecurity");
    qmlRegisterUncreatableType<SignalStrengthEnum>("WifiControl", 1, 0, "SignalStrengthEnum",
                                                   "Not creatable as it is an enum type");
    qRegisterMetaType<SignalStrength>("SignalStrength");
    qmlRegisterUncreatableType<WifiStatus>("WifiControl", 1, 0, "WifiStatus",
                                           "Not creatable as it is an information object only");
    qRegisterMetaType<WifiStatus>("WifiStatus");

    // DRIVERS
    QString hwConfigPath = appPath;
    if (QFile::exists("/boot/hardware.json")) {
        hwConfigPath = "/boot";
    }
    HardwareFactory* hwFactory =
        HardwareFactory::build(hwConfigPath + "/hardware.json", hwConfigPath + "/hardware-schema.json");
    WifiControl* wifiControl = hwFactory->getWifiControl();
    engine.rootContext()->setContextProperty("wifi", wifiControl);
    WebServerControl* webServerControl = hwFactory->getWebServerControl();
    engine.rootContext()->setContextProperty("webserver", webServerControl);
    DisplayControl* displayControl = hwFactory->getDisplayControl();
    engine.rootContext()->setContextProperty("displayControl", displayControl);

    qmlRegisterSingletonType<BatteryFuelGauge>("Battery", 1, 0, "Battery", &HardwareFactory::batteryFuelGaugeProvider);
    qmlRegisterSingletonType<DisplayControl>("DisplayControl", 1, 0, "DisplayControl", &HardwareFactory::displayControlProvider);
    qmlRegisterSingletonType<InterruptHandler>("InterruptHandler", 1, 0, "InterruptHandler", &HardwareFactory::interruptHandlerProvider);

    // BLUETOOTH AREA
    BluetoothArea bluetoothArea;
    engine.rootContext()->setContextProperty("bluetoothArea", &bluetoothArea);

    // TRANSLATION
    TranslationHandler transHndl(&engine);
    engine.rootContext()->setContextProperty("translateHandler", &transHndl);

    // INTEGRATIONS
    Integrations integrations(&engine, appPath);
    // Make integration state available in QML
    qmlRegisterUncreatableType<Integrations>("Integrations", 1, 0, "Integrations",
                                             "Not creatable, only used for enum.");
    engine.rootContext()->setContextProperty("integrations", &integrations);

    // ENTITIES
    Entities entities;
    engine.rootContext()->setContextProperty("entities", &entities);

    // NOTIFICATIONS
    Notifications notifications(&engine);
    engine.rootContext()->setContextProperty("notifications", &notifications);

    // TODO(zehnm) put initialization into factory
    if (!wifiControl->init()) {
        notifications.add(true, QObject::tr("WiFi device was not found."));
    }

    // FILE IO
    FileIO fileIO;
    engine.rootContext()->setContextProperty("fileio", &fileIO);

    // YIO API
    YioAPI yioapi(&engine);
    engine.rootContext()->setContextProperty("api", &yioapi);

    // UTILS
    qmlRegisterType<MediaPlayerUtils>("MediaPlayerUtils", 1, 0, "MediaPlayerUtils");

    engine.addImportPath("qrc:/");

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    // FIXME move initialization code to a device driver factory
    QObject* standbyControl = config.getQMLObject("standbyControl");
    if (standbyControl == nullptr) {
        qCritical() << "Error looking up QML object:"
                    << "standbyControl";
    } else {
        QObject::connect(standbyControl, SIGNAL(standByOn()), wifiControl, SLOT(stopSignalStrengthScanning()));
        QObject::connect(standbyControl, SIGNAL(standByOn()), wifiControl, SLOT(stopWifiStatusScanning()));
        QObject::connect(standbyControl, SIGNAL(standByOff()), wifiControl, SLOT(startSignalStrengthScanning()));
        QObject::connect(standbyControl, SIGNAL(standByOff()), wifiControl, SLOT(startWifiStatusScanning()));
    }

    return app.exec();
}
