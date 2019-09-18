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
#ifdef __arm__
    QPluginLoader pluginLoader(path + "/plugins/lib" + pluginName);
    QObject *plugin = pluginLoader.instance();
    QString err = pluginLoader.errorString();
    qDebug() << err;
    return plugin;
#elif __APPLE__
    QPluginLoader pluginLoader(path + "/plugins/lib" + pluginName + ".dylib");
    QObject *plugin = pluginLoader.instance();
    QString err = pluginLoader.errorString();
    qDebug() << err;
    return plugin;
#else
    QPluginLoader pluginLoader(path + "/plugins/lib" + pluginName);
    QObject *plugin = pluginLoader.instance();
    QString err = pluginLoader.errorString();
    qDebug() << err;
    return plugin;
#endif
}

QObject* Launcher::loadIntegration(const QString& path, const QString &pluginName, int integrationId, const QVariantMap& config, QObject* entities, QObject* notifications)
{
    QObject *plugin = loadPlugin(path, pluginName);
    if (plugin) {
        IntegrationInterface *interface = qobject_cast<IntegrationInterface *>(plugin);
        if (interface) {
            interface->initialize (integrationId, config, entities, notifications);
        }
    }
    return plugin;
}
