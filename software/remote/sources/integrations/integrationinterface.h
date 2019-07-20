#ifndef INTEGRATIONINTERFACE_H
#define INTEGRATIONINTERFACE_H

#include <QString>
#include <QVariantMap>

#include "../entities/entities.h"
#include "../entities/entity.h"
#include "../notifications.h"

// This interface is implemented by the integration .so files, it is used by the entities to operate the integration
class IntegrationInterface
{
public:
    virtual ~IntegrationInterface() {}

    // initialize integration hub with data and entities
    virtual void    initialize	    (int integrationId, const QVariantMap& configurations, QObject *entities, QObject *notifications) = 0;

    // connects the integration Hub
    virtual void    connect	    () = 0;

    // disconnects the integration Hub
    virtual void    disconnect() = 0;

public slots:
    virtual void    sendCommand     (const QString& type, const QString& id, const QString& command, const QVariant& param) = 0;

private:
    virtual void    updateEntity    (const QString& entity_id, const QVariantMap& attr) = 0;
};

QT_BEGIN_NAMESPACE
#define IntegrationInterface_iid "YIO.IntegrationInterface"
Q_DECLARE_INTERFACE(IntegrationInterface, IntegrationInterface_iid)
QT_END_NAMESPACE

#endif // INTEGRATIONINTERFACE_H
