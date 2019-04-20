#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFontDatabase>
#include <QDir>

#include "launcher.h"
#include "jsonfile.h"
#include "display_control.h"

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    //    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    // LOADING FONTS
    QFontDatabase::addApplicationFont(QStringLiteral("/usr/lib/fonts/OpenSans-Regular.ttf"));
    QFontDatabase::addApplicationFont(QStringLiteral("/usr/lib/fonts/OpenSans-Light.ttf"));
    QFontDatabase::addApplicationFont(QStringLiteral("/usr/lib/fonts/OpenSans-Bold.ttf"));

    // LOADING CUSTOM SCRIPT LAUNCHER AND JSON READER
    qmlRegisterType<Launcher>("Launcher", 1, 0, "Launcher");
    qmlRegisterType<JsonFile>("JsonFile", 1, 0, "JsonFile");
    qmlRegisterType<DisplayControl>("DisplayControl", 1, 0, "DisplayControl");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
