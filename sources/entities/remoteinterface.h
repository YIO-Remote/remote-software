#ifndef REMOTEINTERFACE_H
#define REMOTEINTERFACE_H

#include <QObject>

/// This interface allows integrations to access the specific attributes
/// You get this interface from the generic entity using getSpecificInterface
class RemoteInterface
{
public:
    virtual ~RemoteInterface  ();

    virtual QVariantList      commands()   = 0;
};

QT_BEGIN_NAMESPACE
#define RemoteInterface_iid "YIO.RemoteInterface"
Q_DECLARE_INTERFACE(RemoteInterface, RemoteInterface_iid)
QT_END_NAMESPACE


#endif // REMOTEINTERFACE_H
