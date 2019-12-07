#ifndef INTEGRATIONSINTERFACE_H
#define INTEGRATIONSINTERFACE_H

#include <QVariantMap>

class IntegrationsInterface
{
public:
    virtual ~IntegrationsInterface();

    virtual void                    add                     (const QVariantMap& config, QObject *obj, const QString& type) = 0;
};

QT_BEGIN_NAMESPACE
#define IntegrationsInterface_iid "YIO.IntegrationsInterface"
Q_DECLARE_INTERFACE(IntegrationsInterface, IntegrationsInterface_iid)
QT_END_NAMESPACE

#endif // INTEGRATIONSINTERFACE_H
