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

#include "switch.h"

#include <QQmlApplicationEngine>

SwitchInterface::~SwitchInterface() {}

QString Switch::Type = "switch";

bool Switch::updateAttrByIndex(int attrIndex, const QVariant& value) {
    bool chg = false;
    switch (attrIndex) {
        case SwitchDef::STATE:
            if (value.type() == QVariant::String)
                chg = setStateText(value.toString());
            else
                chg = setState(value.toInt());
            break;
        case SwitchDef::POWER:
            if (m_power != value.toInt()) {
                m_power = value.toInt();
                chg = true;
                emit powerChanged();
            }
            break;
    }
    return chg;
}

void Switch::turnOn() { command(SwitchDef::C_ON, ""); }

void Switch::turnOff() { command(SwitchDef::C_OFF, ""); }

void Switch::toggle() {
    if (state() == SwitchDef::ON)
        turnOff();
    else
        turnOn();
}

Switch::Switch(const QVariantMap& config, IntegrationInterface* integrationObj, QObject* parent)
    : Entity(Type, config, integrationObj, parent), m_power(0) {
    static QMetaEnum metaEnumAttr;
    static QMetaEnum metaEnumFeatures;
    static QMetaEnum metaEnumCommands;
    static QMetaEnum metaEnumState;
    if (!metaEnumAttr.isValid()) {
        int index = SwitchDef::staticMetaObject.indexOfEnumerator("Attributes");
        metaEnumAttr = SwitchDef::staticMetaObject.enumerator(index);
        index = SwitchDef::staticMetaObject.indexOfEnumerator("States");
        metaEnumState = SwitchDef::staticMetaObject.enumerator(index);
        index = SwitchDef::staticMetaObject.indexOfEnumerator("Features");
        metaEnumFeatures = SwitchDef::staticMetaObject.enumerator(index);
        index = SwitchDef::staticMetaObject.indexOfEnumerator("Commands");
        metaEnumCommands = SwitchDef::staticMetaObject.enumerator(index);
        qmlRegisterUncreatableType<SwitchDef>("Entity.Switch", 1, 0, "Switch", "Not creatable as it is an enum type.");
    }
    m_enumAttr = &metaEnumAttr;
    m_enumFeatures = &metaEnumFeatures;
    m_enumCommands = &metaEnumCommands;
    m_enumState = &metaEnumState;
    m_specificInterface = qobject_cast<SwitchInterface*>(this);
    initializeSupportedFeatures(config);
}
