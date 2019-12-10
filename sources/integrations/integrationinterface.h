#ifndef INTEGRATIONINTERFACE_H
#define INTEGRATIONINTERFACE_H

#include <QVariant>

class IntegrationInterface
{
public:
    virtual ~IntegrationInterface();

    virtual void        connect()       = 0;            // Must be implemented by integration
    virtual void        disconnect()    = 0;            // Must be implemented by integration
    virtual void        enterStandby()  = 0;            // Can be overriden by integration
    virtual void        leaveStandby()  = 0;            // Can be overriden by integration
    virtual void        sendCommand     (const QString& type, const QString& entity_id, int command, const QVariant& param) = 0;

    virtual int         state           () = 0;
    virtual QString     integrationId   () = 0;
    virtual QString     friendlyName    () = 0;
};

QT_BEGIN_NAMESPACE
#define IntegrationInterface_iid "YIO.IntegrationInterface"
Q_DECLARE_INTERFACE(IntegrationInterface, IntegrationInterface_iid)
QT_END_NAMESPACE
#endif // INTEGRATIONINTERFACE_H
