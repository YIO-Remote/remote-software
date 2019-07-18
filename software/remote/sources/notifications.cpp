#include <QtDebug>

#include "notifications.h"

Notifications* Notifications::s_instance = NULL;

Notifications::Notifications(QQmlApplicationEngine *engine) :
    m_engine(engine)
{
    s_instance = this;
}

Notifications::~Notifications()
{
    s_instance = NULL;
}

void Notifications::show(const int id)
{
    // get the notification data to show
    QVariant map = m_notifications.value(id);

    // let's check if the qml files are loaded first
    if (m_engine->rootObjects().size() > 0) {
        // find the root object
        QObject *root = m_engine->rootObjects().at(0); //->findChild<QObject*>("notificationsRow")

        // create q qml object in qml space
        // this way it knows all the color and other global qml variables

        QMetaObject::invokeMethod(root, "showNotification", Qt::AutoConnection,
                                  Q_ARG(QVariant, map));
    }
}

QVariantList Notifications::list()
{
    QVariantList list;
    for (int i=0; i<m_notifications.count(); i++)
    {
        list.append(m_notifications.value(i));
    }
    return list;
}

void Notifications::add(const bool &type, const QString &text, const QString &actionlabel, const QVariant &action)
{   
    QVariantMap map;

    map["id"] = m_id;
    map["error"] = type;
    map["text"] = text;
    map["actionlabel"] = actionlabel;
    map["action"] = action;
    map["timestamp"] = QDateTime::currentDateTime();

    m_notifications.insert(m_id, map);

    emit listChanged();
    emit errorChanged();

    // show the notification
    show(m_id);

    m_id ++;
}

void Notifications::add(const bool &type, const QString &text)
{
    QVariantMap map;

    map["id"] = m_id;
    map["error"] = type;
    map["text"] = text;
    map["actionlabel"] = "";
    map["action"] = "";
    map["timestamp"] = QDateTime::currentDateTime();

    m_notifications.insert(m_id, map);

    emit listChanged();
    emit errorChanged();

    // show the notification
    show(m_id);

    m_id ++;
}

void Notifications::remove(const int id)
{
    m_notifications.remove(id);
    emit listChanged();
    emit errorChanged();
}

bool Notifications::isThereError()
{
    bool r = false;

    for (int i=0; i<m_notifications.count(); i++)
    {
        QVariantMap map = m_notifications.value(i);
        if (map["error"].toBool() == true) {
            r = true;
        }
    }

    return r;
}
