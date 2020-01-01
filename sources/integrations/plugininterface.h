#pragma once

#include <QString>
#include <QTranslator>
#include <QVariantMap>

// This interface is implemented by the integration .so files, it is used by the entities to operate the integration
class PluginInterface : public QObject {
    Q_OBJECT

 public:
    virtual ~PluginInterface() {}

    // create an integration and return the object
    virtual void create(const QVariantMap &configurations, QObject *entities, QObject *notifications, QObject *api,
                        QObject *configObj) = 0;

    // enable log category
    virtual void setLogEnabled(QtMsgType msgType, bool enable) = 0;

 signals:
    void createDone(QMap<QObject *, QVariant> map);
};

QT_BEGIN_NAMESPACE
#define PluginInterface_iid "YIO.PluginInterfaceInterface"
Q_DECLARE_INTERFACE(PluginInterface, PluginInterface_iid)
QT_END_NAMESPACE
