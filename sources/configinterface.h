#ifndef CONFIGINTERFACE_H
#define CONFIGINTERFACE_H

#include <QVariant>

/// This interface is implemented by the Entities object and used by integration DLLs to access the entities
class ConfigInterface
{
public:
    virtual ~ConfigInterface() {}

    virtual QVariantMap             read                    () = 0;
    virtual void                    readWrite               (QVariantMap config) = 0;
    virtual QVariant                getContextProperty      (const QString& name) = 0;
    virtual QObject*                getQMLObject            (const QString &name) = 0;

private:
    QVariantMap                 m_config;
};

QT_BEGIN_NAMESPACE
#define ConfigInterface_iid "YIO.ConfigInterface"
Q_DECLARE_INTERFACE(ConfigInterface, ConfigInterface_iid)
QT_END_NAMESPACE

#endif // CONFIGINTERFACE_H
