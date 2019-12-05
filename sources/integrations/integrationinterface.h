#ifndef INTEGRATIONINTERFACE_H
#define INTEGRATIONINTERFACE_H

#include <QString>
#include <QVariantMap>

// This interface is implemented by the integration .so files, it is used by the entities to operate the integration
class IntegrationInterface : public QObject
{
    Q_OBJECT

public:
    virtual ~IntegrationInterface() {}

    // create an integration and return the object
    virtual void create         (const QVariantMap& configurations, QObject *entities, QObject *notifications, QObject* api, QObject *configObj) = 0;

    // enable log category
    virtual void setLogEnabled  (QtMsgType msgType, bool enable) = 0;

signals:
    void createDone(QMap<QObject *, QVariant> map);

};

QT_BEGIN_NAMESPACE
#define IntegrationInterface_iid "YIO.IntegrationInterface"
Q_DECLARE_INTERFACE(IntegrationInterface, IntegrationInterface_iid)
QT_END_NAMESPACE

#endif // INTEGRATIONINTERFACE_H
