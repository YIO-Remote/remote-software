#ifndef INTEGRATIONS_H
#define INTEGRATIONS_H

#include <QObject>
#include <QMap>

class Integrations : public QObject
{
    Q_OBJECT

public:
    // list of all integrations
    Q_PROPERTY  (QList<QObject *>       list                    READ    list         NOTIFY     listChanged          CONSTANT)

    // load all integrations from config file
    Q_INVOKABLE bool                    load                    (const QString& appPath);

    // get all integrations
    QList<QObject *>                    list                    ();

    // get an integration by type
    Q_INVOKABLE QObject*                getByType               (const QString& type);

    // add an integration
    Q_INVOKABLE void                    add                     (const QString& type, const QString& friendlyName, QObject* obj);

    // remove an integraiton
    Q_INVOKABLE void                    remove                  (const QString& type);

    // get friendly name
    Q_INVOKABLE QString                 getFriendlyName         (const QString& type);
    Q_INVOKABLE QString                 getFriendlyName         (QObject* obj);

    explicit Integrations(QObject *parent = nullptr);
    virtual ~Integrations();

    static Integrations*       getInstance     ()
    { return s_instance; }

signals:
    void listChanged();


private:
    QMap<QString, QObject*>     m_integrations;
    QMap<QString, QString>      m_integrations_friendly_names;

    static Integrations*        s_instance;


};

#endif // INTEGRATIONS_H
