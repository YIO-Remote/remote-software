#include <QPluginLoader>
#include <QtDebug>

#include "launcher.h"

Launcher::Launcher(QObject *parent) :
    QObject(parent),
    m_process(new QProcess(this))
{
}

QString Launcher::launch(const QString &program)
{
    m_process->start(program);
    m_process->waitForFinished(-1);
    QByteArray bytes = m_process->readAllStandardOutput();
    QString output = QString::fromLocal8Bit(bytes);
    return output;
}

QObject* Launcher::loadPlugin(const QString& path, const QString &pluginName)
{
    QString pluginPath;
    #ifdef __arm__
        pluginPath = path + "/plugins/lib" + pluginName;
    #elif __APPLE__
        pluginPath = path + "/plugins/lib" + pluginName + ".dylib";
    #elif _WIN32
        pluginPath = path + "/plugins/" + pluginName + ".dll";
    #else
        pluginPath = path + "/plugins/lib" + pluginName;
    #endif
    qDebug() << "LOADING PLUGIN:" << pluginPath;
    QPluginLoader pluginLoader(pluginPath, this);
    QObject *plugin = pluginLoader.instance();

    if (!plugin) {
        qDebug() << "FAILED TO LOAD PLUGIN: " <<  pluginLoader.errorString();

        Notifications::getInstance()->add(true, "Failed to load " + QString(pluginName));
    }

    return plugin;
}
