/******************************************************************************
 *
 * Copyright (C) 2020 Markus Zehnder <business@markuszehnder.ch>
 * Copyright (C) 2018-2020 Marton Borzak <hello@martonborzak.com>
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
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>

#include "bluetooth.h"
#include "commandlinehandler.h"
#include "components/media_player/sources/utils_mediaplayer.h"
#include "config.h"
#include "entities/entities.h"
#include "environment.h"
#include "factoryreset.h"
#include "fileio.h"
#include "hardware/buttonhandler.h"
#include "hardware/hardwarefactory.h"
#include "hardware/touchdetect.h"
#include "integrations/integrations.h"
#include "jsonfile.h"
#include "launcher.h"
#include "logger.h"
#include "logging.h"
#include "notifications.h"
#include "softwareupdate.h"
#include "standbycontrol.h"
#include "translation.h"
#include "yioapi.h"

int main(int argc, char* argv[]) {
    bool initializing = true;

    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    qputenv("QT_VIRTUALKEYBOARD_LAYOUT_PATH", "qrc:/keyboard/layouts");
    qputenv("QT_VIRTUALKEYBOARD_STYLE", "remotestyle");

    // must be set before creating QCoreApplication, therefore not possible to set in CommandLineHelper
    // QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    QString version(APP_VERSION);
    if (version.startsWith('v')) {
        version.remove(0, 1);
    }
    QGuiApplication::setApplicationVersion(version);

    QGuiApplication       app(argc, argv);
    QQmlApplicationEngine engine;

    engine.addImportPath("qrc:/keyboard");

    Environment* environment = new Environment(&app);

    // Get the application's resource dir path and expose it to QML (prevents setting the JSON config variable)
    QString resourcePath = environment->getResourcePath();
    engine.rootContext()->setContextProperty("appPath", resourcePath);

    // Process command line arguments
    CommandLineHandler cmdLineHandler;
    // Attention: app might terminate within process depending on cmd line arguments!
    cmdLineHandler.process(app, resourcePath, environment->getConfigurationPath());

    Logger *logger = Logger::instance();

    // LOAD CONFIG
    bool configError = false;
    if (!QFile::exists(cmdLineHandler.configFile())) {
        qFatal("App configuration file not found: %s", cmdLineHandler.configFile().toLatin1().constData());
    }

    Config* config = new Config(&engine, cmdLineHandler.configFile(), cmdLineHandler.configSchemaFile(), environment);
    if (!config->readConfig()) {
        qCCritical(lcApp).noquote() << "Invalid configuration!" << endl << config->getError();
        configError = true;
    }

    qmlRegisterUncreatableType<Config>("Config", 1, 0, "Config",
                                       "Not creatable as it is a global object managed from cpp");
    qmlRegisterUncreatableType<UnitSystem>("Config", 1, 0, "UnitSystem", "Not creatable as it is an enum type");

    engine.rootContext()->setContextProperty("config", config);
    engine.rootContext()->setContextProperty("configError", configError);

    // NOTIFICATIONS
    Notifications notifications(&engine);
    engine.rootContext()->setContextProperty("notifications", &notifications);

    // LOGGER
    QVariantMap logCfg = config->getSettings().value("logging").toMap();
    // "path" cfg logic:
    //   - key not set or "." => <application_path>/log
    //   - "" (empty)         => no log file
    //   - <otherwise>        => absolute directory path (e.g. "/var/log")
    QString path = logCfg.value("path", ".").toString();
    if (path == ".") {
        path = resourcePath + "/log";
    }

    QObject::connect(logger, &Logger::userError, [&notifications](const QString& error) {
        notifications.add(true, error);
    });

    logger->setDefaultHandlerEnabled(logCfg.value("console", true).toBool());
    logger->setLogDirectory(path);
    logger->setLogLevel(logCfg.value("level", "WARN").toString());
    logger->setShowSourcePos(logCfg.value("showSource", true).toBool());
    logger->setQueueSize(logCfg.value("queueSize", 100).toInt());
    logger->setLogFilePurgeHours(logCfg.value("purgeHours", 72).toInt());

    qCInfo(lcApp) << "YIO App" << version;

    // LOADING CUSTOM COMPONENTS
    qmlRegisterType<Launcher>("Launcher", 1, 0, "Launcher");
    qmlRegisterType<JsonFile>("JsonFile", 1, 0, "JsonFile");

    TouchEventFilter* touchEventFilter = new TouchEventFilter();
    qmlRegisterSingletonType<TouchEventFilter>("TouchEventFilter", 1, 0, "TouchEventFilter",
                                               &TouchEventFilter::getInstance);

    qmlRegisterUncreatableType<WifiNetwork>("WifiControl", 1, 0, "WifiNetwork",
                                            "Not creatable as it is an information object only");
    qmlRegisterUncreatableType<SystemServiceName>("SystemService", 1, 0, "SystemServiceName",
                                                  "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<WifiSecurity>("WifiControl", 1, 0, "WifiSecurity",
                                             "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<SignalStrength>("WifiControl", 1, 0, "SignalStrength",
                                               "Not creatable as it is an enum type");
    qRegisterMetaType<WifiStatus>("WifiStatus");
    qmlRegisterUncreatableType<WifiStatus>("WifiControl", 1, 0, "WifiStatus",
                                           "Not creatable as it is an information object only");

    // DRIVERS
    HardwareFactory* hwFactory = HardwareFactory::build(
        cmdLineHandler.hardwareConfigFile(), cmdLineHandler.hardwareConfigSchemaFile(), cmdLineHandler.getProfile());
    WifiControl* wifiControl = hwFactory->getWifiControl();
    engine.rootContext()->setContextProperty("wifi", wifiControl);
    WebServerControl* webServerControl = hwFactory->getWebServerControl();
    engine.rootContext()->setContextProperty("webserver", webServerControl);

    DisplayControl* displayControl = hwFactory->getDisplayControl();
    engine.rootContext()->setContextProperty("displayControl", displayControl);

    qmlRegisterSingletonType<BatteryCharger>("BatteryCharger", 1, 0, "BatteryCharger",
                                             &HardwareFactory::batteryChargerProvider);
    qmlRegisterSingletonType<BatteryFuelGauge>("Battery", 1, 0, "Battery", &HardwareFactory::batteryFuelGaugeProvider);
    qmlRegisterSingletonType<DisplayControl>("DisplayControl", 1, 0, "DisplayControl",
                                             &HardwareFactory::displayControlProvider);
    qmlRegisterSingletonType<InterruptHandler>("InterruptHandler", 1, 0, "InterruptHandler",
                                               &HardwareFactory::interruptHandlerProvider);
    qmlRegisterSingletonType<HapticMotor>("Haptic", 1, 0, "Haptic", &HardwareFactory::hapticMotorProvider);
    qmlRegisterSingletonType<ProximitySensor>("Proximity", 1, 0, "Proximity",
                                              &HardwareFactory::proximitySensorProvider);
    qmlRegisterSingletonType<LightSensor>("LightSensor", 1, 0, "LightSensor", &HardwareFactory::lightSensorProvider);
    qmlRegisterSingletonType<SystemInfo>("SystemInformation", 1, 0, "SystemInformation",
                                         &HardwareFactory::systemInformationProvider);

    // BLUETOOTH AREA
    BluetoothControl bluetooth;
    engine.rootContext()->setContextProperty("bluetooth", &bluetooth);

    // TRANSLATION
    TranslationHandler transHndl(&engine);
    engine.rootContext()->setContextProperty("translateHandler", &transHndl);

    // INTEGRATIONS
    Integrations* integrations =
        new Integrations(qEnvironmentVariable(Environment::ENV_YIO_PLUGIN_DIR, resourcePath + "/plugins"));
    // Make integration state available in QML
    qmlRegisterUncreatableType<Integrations>("Integrations", 1, 0, "Integrations",
                                             "Not creatable, only used for enum.");
    engine.rootContext()->setContextProperty("integrations", integrations);

    // ENTITIES
    Entities entities;
    engine.rootContext()->setContextProperty("entities", &entities);

    // Ready for device startup!
    hwFactory->initialize();

    // YIO API
    YioAPI* yioapi = YioAPI::getInstance();
    bool ok;
    int webSocketPort = qEnvironmentVariableIntValue("YIO_WEBSOCKET_PORT", &ok);
    yioapi->setWebSocketPort(ok ? webSocketPort : 946);
    engine.rootContext()->setContextProperty("api", yioapi);

    // FACTORY RESET HANDLER
    FactoryReset factoryReset(environment);
    QObject::connect(&factoryReset, &FactoryReset::factoryResetCompleted, [environment, &initializing]() {
        if (environment->isYioRemote()) {
            Launcher().launch("reboot");
        }
        initializing ? exit(0) : QCoreApplication::exit(0);
    });
    QObject::connect(&factoryReset, &FactoryReset::factoryResetFailed, [&notifications](const QString& error) {
        notifications.add(true, QGuiApplication::tr("Factory reset failed.\n %1").arg(error));
    });
    engine.rootContext()->setContextProperty("factoryReset", &factoryReset);

    // BUTTON HANDLER
    ButtonHandler* buttonHandler = new ButtonHandler(hwFactory->getInterruptHandler(), yioapi);
    qmlRegisterSingletonType<ButtonHandler>("ButtonHandler", 1, 0, "ButtonHandler", &ButtonHandler::getQMLInstance);

    // reset combination was pressed on startup (only on the remote hardware)
    if (environment->isYioRemote() && buttonHandler->resetButtonsPressed()) {
        factoryReset.performReset();
    }

    // STANDBY CONTROL
    StandbyControl* standbyControl =
        new StandbyControl(displayControl, hwFactory->getProximitySensor(), hwFactory->getLightSensor(),
                           touchEventFilter, hwFactory->getInterruptHandler(), buttonHandler, wifiControl,
                           hwFactory->getBatteryFuelGauge(), config, yioapi, integrations);
    Q_UNUSED(standbyControl);
    qmlRegisterSingletonType<StandbyControl>("StandbyControl", 1, 0, "StandbyControl", &StandbyControl::getQMLInstance);
    QObject::connect(standbyControl, &StandbyControl::standByOff, hwFactory->getBatteryFuelGauge(),
                     &BatteryFuelGauge::updateBatteryValues);

    // SOFTWARE UPDATE
    QVariantMap     appUpdCfg = config->getSettings().value("softwareupdate").toMap();
    SoftwareUpdate* softwareUpdate = new SoftwareUpdate(appUpdCfg, hwFactory->getBatteryFuelGauge());
    qmlRegisterSingletonType<SoftwareUpdate>("SoftwareUpdate", 1, 0, "SoftwareUpdate", &SoftwareUpdate::getQMLInstance);

    // UTILS
    qmlRegisterType<MediaPlayerUtils>("MediaPlayerUtils", 1, 0, "MediaPlayerUtils");

    // LOAD FONTS
    QFontDatabase::addApplicationFont(resourcePath + "/fonts/OpenSans-Light.ttf");
    QFontDatabase::addApplicationFont(resourcePath + "/fonts/OpenSans-Regular.ttf");
    QFontDatabase::addApplicationFont(resourcePath + "/fonts/OpenSans-SemiBold.ttf");
    QFontDatabase::addApplicationFont(resourcePath + "/fonts/OpenSans-Bold.ttf");

    // LOAD STYLES
    qmlRegisterSingletonType(QUrl(QStringLiteral("qrc:/Style.qml")), "Style", 1, 0, "Style");

    // LOAD ICONS
    QFontDatabase::addApplicationFont(resourcePath + "/icons/icons.ttf");

    // set rending of text
    QQuickWindow::setTextRenderType(QQuickWindow::NativeTextRendering);

    engine.addImportPath("qrc:/");

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    softwareUpdate->start();

    QObject* mainApplicationWindow = config->getQMLObject("applicationWindow");
    touchEventFilter->setSource(mainApplicationWindow);
    initializing = false;

    return app.exec();
}
