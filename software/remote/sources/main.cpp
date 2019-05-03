#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFontDatabase>
#include <QQmlContext>
#include <QFile>
#include <QFileInfo>

#include "launcher.h"
#include "jsonfile.h"
#include "display_control.h"
#include "translation.h"
#include "touchdetect.h"

int main(int argc, char *argv[])
{
//    uint16_t ambient_light = 0;

//    APDS9960 apds = APDS9960();

//    qDebug() << endl;
//    qDebug() << "------------------------------------" << endl;
//    qDebug() << "APDS-9960 - ColorSensor  " << endl;
//    qDebug() << "------------------------------------" << endl;

//    // Initialize APDS-9960 (configure I2C and initial values)
//    if ( (false == apds.init()) || (false == apds.enableLightSensor(false)) )
//    {
//        qDebug() << "APDS-9960 init failed!" << endl;
//    }

//    delay(500);

//    if ( !apds.readAmbientLight(ambient_light)) {
//        qDebug() << "Error reading light values" << endl;
//    } else {
//        qDebug() << "Ambient: ";
//        qDebug() << int(ambient_light);
//    }

    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    //    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
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
    if (QFile::exists("/mnt/boot/config.json")) {
        configPath = "/mnt/boot";
    }
    engine.rootContext()->setContextProperty("configPath", configPath);

    // LOADING FONTS
    QFontDatabase::addApplicationFont(QStringLiteral("/usr/lib/fonts/OpenSans-Regular.ttf"));
    QFontDatabase::addApplicationFont(QStringLiteral("/usr/lib/fonts/OpenSans-Light.ttf"));
    QFontDatabase::addApplicationFont(QStringLiteral("/usr/lib/fonts/OpenSans-Bold.ttf"));

    // LOADING CUSTOM SCRIPT LAUNCHER AND JSON READER
    qmlRegisterType<Launcher>("Launcher", 1, 0, "Launcher");
    qmlRegisterType<JsonFile>("JsonFile", 1, 0, "JsonFile");
    qmlRegisterType<DisplayControl>("DisplayControl", 1, 0, "DisplayControl");
    qmlRegisterType<TouchEventFilter>("TouchEventFilter", 1, 0, "TouchEventFilter");

    // TRANSLATION
    TranslationHandler transHndl(&engine);
    engine.rootContext()->setContextProperty("translateHandler", &transHndl);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
