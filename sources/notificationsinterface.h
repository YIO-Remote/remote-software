#ifndef NOTIFICATIONSINTERFACE_H
#define NOTIFICATIONSINTERFACE_H

#include <QVariant>

/// This interface is implemented by the Entities object and used by integration DLLs to access the entities
class NotificationsInterface
{
public:
    virtual ~NotificationsInterface() {}

   // add a notification
    virtual void        add      (const bool &type, const QString &text, const QString &actionlabel, const QVariant &action) = 0;
    virtual void        add      (const bool &type, const QString &text) = 0;
    virtual void        remove   (const int id) = 0;
    virtual void        remove   (const QString &text) = 0;

};

QT_BEGIN_NAMESPACE
#define NotificationsInterface_iid "YIO.NotificationsInterface"
Q_DECLARE_INTERFACE(NotificationsInterface, NotificationsInterface_iid)
QT_END_NAMESPACE

#endif // NOTIFICATIONSINTERFACE_H
