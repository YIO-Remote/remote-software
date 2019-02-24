#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFontDatabase>
#include "launcher.h"
#include "filehelper.h"

int main(int argc, char *argv[])
{
    qputenv("QML2_IMPORT_PATH", "/keyboard");
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    qputenv("QT_VIRTUALKEYBOARD_LAYOUT_PATH", "qrc:/keyboard/layouts");
    qputenv("QT_VIRTUALKEYBOARD_STYLE", "remotestyle");

//    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QFontDatabase::addApplicationFont(QStringLiteral("/usr/lib/fonts/NeuzeitGro-Reg.otf"));
    QFontDatabase::addApplicationFont(QStringLiteral("/usr/lib/fonts/NeuzeitGro-Lig.otf"));
    QFontDatabase::addApplicationFont(QStringLiteral("/usr/lib/fonts/NeuzeitGro-Bol.otf"));

    // LOADING CUSTOM SCRIPT LAUNCHER AND JSON READER
    qmlRegisterType<Launcher>("Launcher", 1, 0, "Launcher");
    qmlRegisterType<FileHelper>("FileHelpers", 1, 0, "FileHelper");

    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
//    engine.addImportPath("qrc:/keyboard");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
