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
#include <QLoggingCategory>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QtDebug>
#if defined(Q_PROCESSOR_ARM)
#include <filesystem>
#endif

#include "bluetooth.h"
#include "commandlinehandler.h"
#include "components/media_player/sources/utils_mediaplayer.h"
#include "config.h"
#include "entities/entities.h"
#include "environment.h"
#include "fileio.h"
#include "hardware/buttonhandler.h"
#include "hardware/hardwarefactory.h"
#include "hardware/touchdetect.h"
#include "integrations/integrations.h"
#include "jsonfile.h"
#include "launcher.h"
#include "logger.h"
#include "notifications.h"
#include "softwareupdate.h"
#include "standbycontrol.h"
#include "translation.h"
#include "yioapi.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "main");

int main(int argc, char* argv[]) {
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

    // Get the applications dir path and expose it to QML (prevents setting the JSON config variable)
    QString appPath = app.applicationDirPath();
    QString macPath = QFileInfo(appPath + "/../").canonicalPath() + "/Contents/Resources";

    // In case the translation file does not exist at root level (which is copied via the .pro file)
    // we are probably running on a mac, which gives a different result for the applicationDirPath
    // (actually points to '.../remote.app/Contents/MacOS/remote')
    if ((!QFile::exists(appPath + "/translations.json")) && (QFile::exists(macPath + "/translations.json"))) {
        appPath = macPath;
    }
    engine.rootContext()->setContextProperty("appPath", appPath);

    // Process command line arguments
    CommandLineHandler cmdLineHandler;
    // Attention: app might terminate within process depending on cmd line arguments!
    cmdLineHandler.process(app, appPath);

    // LOAD CONFIG
    bool configError = false;
    if (!QFile::exists(cmdLineHandler.configFile())) {
        qFatal("App configuration file not found: %s", cmdLineHandler.configFile().toLatin1().constData());
    }

    Config* config = new Config(&engine, cmdLineHandler.configFile(), cmdLineHandler.configSchemaFile(), appPath);
    if (!config->readConfig()) {
        qCCritical(CLASS_LC).noquote() << "Invalid configuration!" << endl << config->getError();
        configError = true;
    }

    qmlRegisterUncreatableType<Config>("Config", 1, 0, "Config",
                                       "Not creatable as it is a global object managed from cpp");
    qmlRegisterUncreatableType<UnitSystem>("Config", 1, 0, "UnitSystem", "Not creatable as it is an enum type");

    engine.rootContext()->setContextProperty("config", config);
    engine.rootContext()->setContextProperty("configError", configError);

    // LOGGER
    QVariantMap logCfg = config->getSettings().value("logging").toMap();
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
    Logger::getInstance()->write(QString("YIO App %1").arg(version));

    // NOTIFICATIONS
    Notifications notifications(&engine);
    engine.rootContext()->setContextProperty("notifications", &notifications);

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

    // BLUETOOTH AREA
    BluetoothControl bluetooth;
    engine.rootContext()->setContextProperty("bluetooth", &bluetooth);

    // TRANSLATION
    TranslationHandler transHndl(&engine);
    engine.rootContext()->setContextProperty("translateHandler", &transHndl);

    // INTEGRATIONS
    Integrations* integrations =
        new Integrations(qEnvironmentVariable(Environment::ENV_YIO_PLUGIN_DIR, appPath + "/plugins"));
    // Make integration state available in QML
    qmlRegisterUncreatableType<Integrations>("Integrations", 1, 0, "Integrations",
                                             "Not creatable, only used for enum.");
    engine.rootContext()->setContextProperty("integrations", integrations);

    // ENTITIES
    Entities entities;
    engine.rootContext()->setContextProperty("entities", &entities);

    // Ready for device startup!
    hwFactory->initialize();

    // FILE IO
    FileIO fileIO;
    engine.rootContext()->setContextProperty("fileio", &fileIO);

    // YIO API
    YioAPI* yioapi = new YioAPI(&engine);
    engine.rootContext()->setContextProperty("api", yioapi);

    // BUTTON HANDLER
    ButtonHandler* buttonHandler = new ButtonHandler(hwFactory->getInterruptHandler(), yioapi);
    qmlRegisterSingletonType<ButtonHandler>("ButtonHandler", 1, 0, "ButtonHandler", &ButtonHandler::getQMLInstance);

#if defined(Q_PROCESSOR_ARM)
    // reset combination was pressed on startup (only on the remote hardware)
    if (buttonHandler->resetButtonsPressed()) {
        QString defaultConfigPath = qEnvironmentVariable(Environment::ENV_YIO_APP_DIR, appPath) + "/config.json";

        if (QFile::exists(defaultConfigPath)) {
            // create marker file
            QFile file("/firstrun");
            if (file.open(QIODevice::WriteOnly)) {
                file.close();

                // make a copy of existing configuration
                if (!std::filesystem::copy_file("/boot/config.json", "/boot/config.json.old",
                                                std::filesystem::copy_options::overwrite_existing)) {
                    qCCritical(CLASS_LC) << "Error backing up existing configuration.";
                } else {
                    if (!std::filesystem::copy_file(defaultConfigPath.toStdString(), "/boot/config.json",
                                                    std::filesystem::copy_options::overwrite_existing)) {
                        qCCritical(CLASS_LC) << "Error copying default configuration.";
                    } else {
                        // reset wifi settings
                        wifiControl->clearConfiguredNetworks();

                        // reboot
                        Launcher().launch("reboot");

                        return -1;
                    }
                }
            } else {
                qCCritical(CLASS_LC) << "Error writing firstrun marker file";
                notifications.add(
                    true, QGuiApplication::tr("An error occured while restoring to defaults. Please try again."));
            }
        } else {
            qCCritical(CLASS_LC) << "Default config file not found. Cannot restore to defaults.";
            notifications.add(true, QGuiApplication::tr("Default config file not found. Cannot restore to defaults."));
        }
    }
#endif

    // STANDBY CONTROL
    StandbyControl* standbyControl =
        new StandbyControl(displayControl, hwFactory->getProximitySensor(), hwFactory->getLightSensor(),
                           touchEventFilter, hwFactory->getInterruptHandler(), buttonHandler, wifiControl,
                           hwFactory->getBatteryFuelGauge(), config, yioapi, integrations);
    Q_UNUSED(standbyControl);
    qmlRegisterSingletonType<StandbyControl>("StandbyControl", 1, 0, "StandbyControl", &StandbyControl::getQMLInstance);

    // SOFTWARE UPDATE
    QVariantMap     appUpdCfg      = config->getSettings().value("softwareupdate").toMap();
    SoftwareUpdate* softwareUpdate = new SoftwareUpdate(appUpdCfg, hwFactory->getBatteryFuelGauge());
    qmlRegisterSingletonType<SoftwareUpdate>("SoftwareUpdate", 1, 0, "SoftwareUpdate", &SoftwareUpdate::getQMLInstance);

    // FIXME move initialization code to a device driver factory
    QObject::connect(standbyControl, SIGNAL(standByOn()), wifiControl, SLOT(stopSignalStrengthScanning()));
    QObject::connect(standbyControl, SIGNAL(standByOn()), wifiControl, SLOT(stopWifiStatusScanning()));
    QObject::connect(standbyControl, SIGNAL(standByOff()), wifiControl, SLOT(startSignalStrengthScanning()));
    QObject::connect(standbyControl, SIGNAL(standByOff()), wifiControl, SLOT(startWifiStatusScanning()));

    // UTILS
    qmlRegisterType<MediaPlayerUtils>("MediaPlayerUtils", 1, 0, "MediaPlayerUtils");

    // LOAD FONTS
    QFontDatabase::addApplicationFont(appPath + "/fonts/OpenSans-Light.ttf");
    QFontDatabase::addApplicationFont(appPath + "/fonts/OpenSans-Regular.ttf");
    QFontDatabase::addApplicationFont(appPath + "/fonts/OpenSans-SemiBold.ttf");
    QFontDatabase::addApplicationFont(appPath + "/fonts/OpenSans-Bold.ttf");

    // LOAD STYLES
    qmlRegisterSingletonType(QUrl(QStringLiteral("qrc:/Style.qml")), "Style", 1, 0, "Style");

    // LOAD ICONS
    QFontDatabase::addApplicationFont(appPath + "/icons/icons.ttf");

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

    return app.exec();
}
