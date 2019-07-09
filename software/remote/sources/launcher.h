#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QObject>
#include <QProcess>

class Launcher : public QObject
{
    Q_OBJECT
public:
    explicit Launcher(QObject *parent = 0);
    Q_INVOKABLE QString launch(const QString &program);
    Q_INVOKABLE QObject* loadPlugin(const QString& path, const QString &pluginName);
    Q_INVOKABLE QObject* loadIntegration(const QString& path, const QString &pluginName, int integrationId, const QVariantMap& configuration);

private:
    QProcess *m_process;
};

#endif // LAUNCHER_H
