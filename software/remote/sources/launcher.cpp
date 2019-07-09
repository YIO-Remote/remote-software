#include <QPluginLoader>

#include "launcher.h"
#include "./integrations/integrationinterface.h"

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
    QPluginLoader pluginLoader(path + "/plugins/" + pluginName);
    QObject *plugin = pluginLoader.instance();
    QString err = pluginLoader.errorString();
    return plugin;
}

QObject* Launcher::loadIntegration(const QString& path, const QString &pluginName, int integrationId, const QVariantMap& config)
{
    QObject *plugin = loadPlugin(path, pluginName);
    if (plugin != NULL) {
        IntegrationInterface* interface = qobject_cast<IntegrationInterface *>(plugin);
        if (interface != NULL) {
            interface->initialize (integrationId, config);
        }
    }
    return plugin;
}
