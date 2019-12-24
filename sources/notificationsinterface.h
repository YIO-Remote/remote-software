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

#ifndef NOTIFICATIONSINTERFACE_H
#define NOTIFICATIONSINTERFACE_H

#include <QVariant>

/// This interface is implemented by the Entities object and used by integration DLLs to access the entities
class NotificationsInterface
{
public:
    virtual ~NotificationsInterface  ();

    // add a notification
    virtual void        add      (const bool &type, const QString &text, const QString &actionlabel, void (*f)(QObject*), QObject* param) = 0;
    virtual void        add      (const bool &type, const QString &text) = 0;
    virtual void        add      (const QString &text) = 0;
    virtual void        remove   (const int id) = 0;
    virtual void        remove   (const QString &text) = 0;

};

QT_BEGIN_NAMESPACE
#define NotificationsInterface_iid "YIO.NotificationsInterface"
Q_DECLARE_INTERFACE(NotificationsInterface, NotificationsInterface_iid)
QT_END_NAMESPACE

#endif // NOTIFICATIONSINTERFACE_H
