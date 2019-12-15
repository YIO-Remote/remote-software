/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 *
 * This file is part of the YIO-Remote software project.
 *
 * YIO-Remote software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YIO-Remote software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

#include <QtDebug>

#include "notifications.h"

NotificationsInterface::~NotificationsInterface()
{
}

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
    QVariant map = m_notifications.value(id).toMap();

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
    return m_notifications;
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

    m_notifications.append(map);

    emit listChanged();
    emit errorChanged();

    // show the notification
    show(m_id);

    m_id++;
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

    m_notifications.append(map);

    emit listChanged();
    emit errorChanged();

    // show the notification
    show(m_id);

    m_id++;
}

void Notifications::add(const QString &text)
{
    QVariantMap map;

    map["id"] = m_id;
    map["error"] = false;
    map["text"] = text;
    map["actionlabel"] = "";
    map["action"] = "";
    map["timestamp"] = QDateTime::currentDateTime();

    m_notifications.append(map);

    emit listChanged();
    emit errorChanged();

    // show the notification
    show(m_id);

    m_id++;
}

void Notifications::remove(const int id)
{
    for (int i=0; i<m_notifications.count(); i++) {
        QVariantMap map = m_notifications.value(i).toMap();
        if (map["id"].toInt() == id) {
            m_notifications.removeAt(i);
        }
    }
    emit listChanged();
    emit errorChanged();
}

void Notifications::remove(const QString &text)
{
    for (int i=0; i<m_notifications.count(); i++) {
        QVariantMap map = m_notifications.value(i).toMap();
        if (map["text"].toString() == text) {
            m_notifications.removeAt(i);
        }
    }
    emit listChanged();
    emit errorChanged();
}

bool Notifications::isThereError()
{
    bool r = false;

    for (int i=0; i<m_notifications.count(); i++)
    {
        QVariantMap map = m_notifications.value(i).toMap();
        if (map["error"].toBool() == true) {
            r = true;
        }
    }

    return r;
}
