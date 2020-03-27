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

#include "notifications.h"

#include <QtDebug>

NotificationsInterface::~NotificationsInterface() {}

Notifications *Notifications::s_instance = nullptr;

Notifications::Notifications(QQmlApplicationEngine *engine) : m_engine(engine), m_log("NOTIFICATIONS") {
    s_instance = this;
}

QDebug operator<<(QDebug debug, const Notification *notification) {
    QDebugStateSaver saver(debug);
    debug.nospace() << "("
                    << "id: " << notification->m_id << ", "
                    << "error: " << notification->m_error << ", "
                    << "label: " << notification->m_actionLabel << ", "
                    << "text: " << notification->m_text << ")";
    return debug;
}

void Notifications::show(const int id) {
    // get the notification data to show
    QVariantMap map;
    map["id"] = m_notifications.value(id)->m_id;
    map["error"] = m_notifications.value(id)->m_error;
    map["text"] = m_notifications.value(id)->m_text;
    map["actionlabel"] = m_notifications.value(id)->m_actionLabel;
    map["timestamp"] = m_notifications.value(id)->m_timestamp;

    // let's check if the qml files are loaded first
    if (m_engine->rootObjects().size() > 0) {
        // find the root object
        QObject *root = m_engine->rootObjects().at(0);  // ->findChild<QObject*>("notificationsRow")

        // create q qml object in qml space
        // this way it knows all the color and other global qml variables

        QMetaObject::invokeMethod(root, "showNotification", Qt::AutoConnection, Q_ARG(QVariant, map));
    }
}

QVariantList Notifications::list() {
    QVariantList list;

    for (QMap<int, Notification *>::const_iterator iter = m_notifications.begin(); iter != m_notifications.end();
         ++iter) {
        QVariantMap map;
        map["id"] = iter.key();
        map["error"] = iter.value()->m_error;
        map["text"] = iter.value()->m_text;
        map["actionlabel"] = iter.value()->m_actionLabel;
        map["timestamp"] = iter.value()->m_timestamp;

        list.append(map);
    }

    return list;
}

void Notifications::add(bool error, const QString &text, const QString &actionlabel, void (*f)(QObject *),
                        QObject *param) {
    Notification *n = new Notification(m_id, error, text, actionlabel, QDateTime::currentDateTime(), f, param);
    qCDebug(m_log) << "Notification object created";

    // check if the same notification exists, then remove it
    for (QMap<int, Notification *>::const_iterator iter = m_notifications.begin(); iter != m_notifications.end();
         ++iter) {
        if (iter.value()->m_text.contains(text) && m_notifications.contains(iter.key())) {
            qCDebug(m_log) << "Notification with same text found";
            m_notifications.remove(iter.key());
            qCDebug(m_log) << "Notification object removed" << iter.key();
            break;
        }
    }

    m_notifications.insert(m_id, n);
    qCDebug(m_log) << "New notification added" << n;

    emit listChanged();
    emit errorChanged();
    qCDebug(m_log) << "Signals emitted";

    show(m_id);
    qCDebug(m_log) << "Notification shown";

    m_id++;
}

void Notifications::add(bool error, const QString &text) { add(error, text, "", nullptr, nullptr); }

void Notifications::add(const QString &text) { add(false, text); }

void Notifications::remove(int id) {
    m_notifications.remove(id);
    emit listChanged();
    emit errorChanged();
    if (m_notifications.size() == 0) emit listIsEmpty();
}

void Notifications::remove(const QString &text) {
    for (QMap<int, Notification *>::const_iterator iter = m_notifications.begin(); iter != m_notifications.end();
         ++iter) {
        if (iter.value()->m_text == text && m_notifications.contains(iter.key())) {
            m_notifications.remove(iter.key());
            break;
        }
    }
    emit listChanged();
    emit errorChanged();
    if (m_notifications.size() == 0) emit listIsEmpty();
}

void Notifications::execute(int id) {
    qCDebug(m_log) << "Executing function" << m_notifications.value(id)->m_text
                   << m_notifications.value(id)->m_actionLabel;
    qCDebug(m_log) << "Creating pointer";
    void (*f)(QObject *) = m_notifications.value(id)->m_action;
    qCDebug(m_log) << "Creating param";
    QObject *param = m_notifications.value(id)->m_param;
    qCDebug(m_log) << "Executing function";
    f(param);
}

bool Notifications::isThereError() {
    bool r = false;

    for (QMap<int, Notification *>::const_iterator iter = m_notifications.begin(); iter != m_notifications.end();
         ++iter) {
        if (iter.value()->m_error) {
            r = true;
        }
    }

    return r;
}

Notification::Notification(int id, bool error, QString text, QString actionLabel, QVariant timestamp,
                           void (*action)(QObject *), QObject *param)
    : m_id(id), m_error(error), m_text(text), m_actionLabel(actionLabel), m_timestamp(timestamp), m_param(param) {
    m_action = action;
}
