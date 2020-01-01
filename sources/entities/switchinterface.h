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
class SwitchDef : public QObject {
    Q_OBJECT
 public:
    enum Attributes { STATE, POWER };
    Q_ENUM(Attributes)

    enum States { OFF = 0, ON = 1 };
    Q_ENUM(States)

    enum Features { F_POWER };
    Q_ENUM(Features)

    enum Commands { C_OFF, C_ON, C_TOGGLE };
    Q_ENUM(Commands)

    explicit SwitchDef(QObject *parent = nullptr) : QObject(parent) {}
};

/// This interface allows integrations to access the specific attributes
/// You get this interface from the generic entity using getSpecificInterface
class SwitchInterface {
 public:
    virtual ~SwitchInterface();

    virtual int power() = 0;
};

QT_BEGIN_NAMESPACE
#define SwitchInterface_iid "YIO.SwitchInterface"
Q_DECLARE_INTERFACE(SwitchInterface, SwitchInterface_iid)
QT_END_NAMESPACE
