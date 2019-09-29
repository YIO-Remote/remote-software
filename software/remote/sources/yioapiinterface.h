#ifndef YIOAPIINTERFACE_H
#define YIOAPIINTERFACE_H

#include <QVariant>

/// This interface is implemented by the Entities object and used by integration DLLs to access the entities
class YioAPIInterface
{
public:
    virtual ~YioAPIInterface() {}

    virtual void start() = 0;
    virtual void stop() = 0;

    virtual QVariantMap getConfig() = 0;
};

QT_BEGIN_NAMESPACE
#define YioAPIInterface_iid "YIO.YioAPIInterface"
Q_DECLARE_INTERFACE(YioAPIInterface, YioAPIInterface_iid)
QT_END_NAMESPACE

#endif // YIOAPIINTERFACE_H
