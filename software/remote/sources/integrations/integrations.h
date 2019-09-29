#ifndef INTEGRATIONS_H
#define INTEGRATIONS_H

#include <QObject>
#include <QMap>

class Integrations : public QObject
{
    Q_OBJECT

public:
    // list of all integrations
    Q_PROPERTY  (QList<QObject *>       list                          READ    list                      NOTIFY     listChanged                         CONSTANT)

    // get all integrations
    QList<QObject *>                    list                    ();

    // add an integration
    Q_INVOKABLE void                    add                     (QString name, QObject* obj, QString friendlyName);

    // remove an integraiton
    Q_INVOKABLE void                    remove                  (QString name);

    // get friendly name
    Q_INVOKABLE QString                 getFriendlyName         (QString name);


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
