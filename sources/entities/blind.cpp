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
#include <QQmlApplicationEngine>
#include "blind.h"

BlindInterface::~BlindInterface()
{
}

QString Blind::Type = "blind";

bool Blind::updateAttrByIndex (int attrIndex, const QVariant& value)
{
    bool chg = false;
    switch (attrIndex) {
        case BlindDef::STATE:
            if (value.type() == QVariant::String)
                chg = setStateText(value.toString());
            else
                chg = setState(value.toInt());
            break;
        case BlindDef::POSITION:
            if (m_position != value.toInt()) {
                m_position = value.toInt();
                chg = true;
                emit positionChanged();
            }
            break;
        }
    return chg;
}

void Blind::close()
{
    command("CLOSE", "");
}

void Blind::open()
{
    command("OPEN", "");
}

void Blind::stop()
{
    command("STOP", "");
}

void Blind::setPosition(int value)
{
     command("POSITION", value);
}

Blind::Blind(const QVariantMap& config, QObject* integrationObj, QObject *parent):
    Entity (Type, config, integrationObj, parent)
{
    static QMetaEnum metaEnumAttr;
    static QMetaEnum metaEnumState;
    if (!metaEnumAttr.isValid()) {
        int index = BlindDef::staticMetaObject.indexOfEnumerator("Attributes");
        metaEnumAttr = BlindDef::staticMetaObject.enumerator(index);
        index = BlindDef::staticMetaObject.indexOfEnumerator("States");
        metaEnumState = BlindDef::staticMetaObject.enumerator(index);
        qmlRegisterUncreatableType<BlindDef>("Entity.Blind", 1, 0, "Blind", "Not creatable as it is an enum type.");
    }
    m_enumAttr = &metaEnumAttr;
    m_enumState = &metaEnumState;
    m_specificInterface = qobject_cast<BlindInterface*>(this);
}


