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

#pragma once

#include <QDateTime>
#include <QMap>
#include <QObject>
#include <QQmlApplicationEngine>

#include "logger.h"
#include "yio-interface/notificationsinterface.h"

class Notification {
 public:
    explicit Notification(int id, bool type, QString text, QString actionLabel, QVariant timestamp,
                          void (*action)(QObject *), QObject *param);

    int     m_id;
    bool    m_error;
    QString m_text;
    QString m_actionLabel;
    void (*m_action)(QObject *);
    QVariant m_timestamp;
    QObject *m_param;
};

class Notifications : public QObject, public NotificationsInterface {
    Q_OBJECT
    Q_INTERFACES(NotificationsInterface)

    // list of all notifications
    Q_PROPERTY(QVariantList list READ list NOTIFY listChanged)
    Q_PROPERTY(bool isThereError READ isThereError NOTIFY errorChanged)

 public:
    explicit Notifications(QQmlApplicationEngine *engine = nullptr);

    // get all notifications
    QVariantList list();

    // get if error
    bool isThereError();

    // add notification
    Q_INVOKABLE void add(bool type, const QString &text, const QString &actionlabel, void (*f)(QObject *),
                         QObject *param) override;
    Q_INVOKABLE void add(bool type, const QString &text) override;
    Q_INVOKABLE void add(const QString &text) override;

    // remove notification
    Q_INVOKABLE void remove(int id) override;
    Q_INVOKABLE void remove(const QString &text) override;

    // execute function
    Q_INVOKABLE void execute(int id);

    static Notifications *getInstance() { return s_instance; }

 signals:
    void listChanged();
    void listIsEmpty();
    void errorChanged();

 private:
    int                       m_id = 0;
    QMap<int, Notification *> m_notifications;

    static Notifications * s_instance;
    QQmlApplicationEngine *m_engine;
    QLoggingCategory       m_log;

    void show(int id);
};
