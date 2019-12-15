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

#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QDateTime>
#include <QMap>

#include "notificationsinterface.h"

class Notifications : public QObject, NotificationsInterface
{
    Q_OBJECT
//    Q_PLUGIN_METADATA(IID "YIO.NotificationsInterface")
    Q_INTERFACES(NotificationsInterface)

    // list of all notifications
    Q_PROPERTY  (QVariantList            list            READ    list           NOTIFY listChanged)
    Q_PROPERTY  (bool                    isThereError    READ    isThereError   NOTIFY errorChanged)

public:
    // get all notifications
    QVariantList                list            ();

    // get if error
    bool                        isThereError    ();

    // add notification
    Q_INVOKABLE void            add             (const bool &type, const QString &text, const QString &actionlabel, const QVariant &action);
    Q_INVOKABLE void            add             (const bool &type, const QString &text);
    Q_INVOKABLE void            add             (const QString &text);

    // remove notification
    Q_INVOKABLE void            remove          (const int id);
    Q_INVOKABLE void            remove          (const QString &text);

    // check if there's an error notification
//    Q_INVOKABLE bool            isThereError    ();


    explicit Notifications(QQmlApplicationEngine *engine = nullptr);
    virtual ~Notifications();

    static Notifications*       getInstance     ()
    { return s_instance; }

signals:
    void listChanged();
    void errorChanged();

private:
    QVariantList                m_notifications;
    int                         m_id = 0;

    static Notifications*       s_instance;
    QQmlApplicationEngine*      m_engine;

    void                        show            (const int id);

};

#endif // NOTIFICATIONS_H
