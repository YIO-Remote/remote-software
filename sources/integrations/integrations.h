#ifndef INTEGRATIONS_H
#define INTEGRATIONS_H

#include <QObject>
#include <QMap>
#include <QQmlApplicationEngine>
#include <QtDebug>

#include "integrationsinterface.h"

class Integrations : public QObject, IntegrationsInterface
{
    Q_OBJECT

public:
    // list of all integrations
    Q_PROPERTY  (QList<QObject *>       list                    READ    list         NOTIFY     listChanged          CONSTANT)

    // load all integrations from config file
    Q_INVOKABLE bool                    load                    ();

    // get all integrations
    QList<QObject *>                    list                    ();

    // get an integration object by id
    Q_INVOKABLE QObject*                get                     (const QString& id);

    // add an integration
    Q_INVOKABLE void                    add                     (const QVariantMap& config, QObject *obj, const QString& type);

    // remove an integraiton
    Q_INVOKABLE void                    remove                  (const QString& id);

    // get friendly name
    Q_INVOKABLE QString                 getFriendlyName         (const QString& id);
    Q_INVOKABLE QString                 getFriendlyName         (QObject* obj);

    // get mdns _tcp name
    Q_INVOKABLE QString                 getMDNS                 (const QString& id);
    Q_INVOKABLE QStringList             getMDNSList             ();

    // get the type of integration by id
    Q_INVOKABLE QString                 getType                 (const QString& id);

    explicit Integrations(QQmlApplicationEngine *engine = NULL, const QString& appPath = "");
    virtual ~Integrations();

    static Integrations*       getInstance     ()
    { return s_instance; }

signals:
    void listChanged();

private:
    QMap<QString, QObject*>     m_plugins;
    QMap<QString, QObject*>     m_integrations;
    QMap<QString, QString>      m_integrations_friendly_names;
    QMap<QString, QString>      m_integrations_mdns;
    QMap<QString, QString>      m_integrations_types;
    QString                     m_appPath;

    static Integrations*        s_instance;
    QQmlApplicationEngine*      m_engine;


};

#endif // INTEGRATIONS_H
