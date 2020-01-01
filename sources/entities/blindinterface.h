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

#include <QObject>

/// This class is a work arround to make the feature enum available in the interface and in the specifc entity class.
/// Qt cannot create metadata describing an enum contained in the interface
/// In the specific entity class it is only needed to generically convert the feature enum to a string and back
class BlindDef : public QObject {
    Q_OBJECT
 public:
    enum Attributes { STATE, POSITION };
    Q_ENUM(Attributes)

    enum States {
        CLOSED = 0,
        OPEN = 1,
        MOVING = 2
    };  // maybe some integration can deliver MOVING, or we simulate by the commands
    Q_ENUM(States)

    enum Features { F_OPEN, F_CLOSE, F_STOP, F_POSITION };
    Q_ENUM(Features)

    enum Commands { C_OPEN, C_CLOSE, C_STOP, C_POSITION };
    Q_ENUM(Commands)

    explicit BlindDef(QObject *parent = nullptr) : QObject(parent) {}
};

/// This interface allows integrations to access the specific attributes
/// You get this interface from the generic entity using getSpecificInterface
class BlindInterface {
 public:
    virtual ~BlindInterface();

    virtual int position() = 0;
};

QT_BEGIN_NAMESPACE
#define BlindInterface_iid "YIO.BlindInterface"
Q_DECLARE_INTERFACE(BlindInterface, BlindInterface_iid)
QT_END_NAMESPACE
