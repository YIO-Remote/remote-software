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
#include <QJsonArray>
#include "remote.h"
#include "../yioapi.h"

RemoteInterface::~RemoteInterface()
{
}

QString   Remote::Type = "remote";
QMetaEnum Remote::s_metaEnumCommands;
QMetaEnum Remote::s_metaEnumAttr;
QMetaEnum Remote::s_metaEnumFeatures;
QMetaEnum Remote::s_metaEnumState;

void Remote::staticInitialize()
{
    if (!s_metaEnumAttr.isValid()) {
        int index = RemoteDef::staticMetaObject.indexOfEnumerator("Attributes");
        s_metaEnumAttr = RemoteDef::staticMetaObject.enumerator(index);
        index = RemoteDef::staticMetaObject.indexOfEnumerator("States");
        s_metaEnumState = RemoteDef::staticMetaObject.enumerator(index);
        index = RemoteDef::staticMetaObject.indexOfEnumerator("Features");
        s_metaEnumFeatures = RemoteDef::staticMetaObject.enumerator(index);
        index = RemoteDef::staticMetaObject.indexOfEnumerator("Commands");
        s_metaEnumCommands = RemoteDef::staticMetaObject.enumerator(index);
        qmlRegisterUncreatableType<RemoteDef>("Entity.Remote", 1, 0, "Remote", "Not creatable as it is an enum type.");
    }
}

// transport and media controls
void Remote::play()
{
    command(RemoteDef::C_PLAY, "");
}

void Remote::pause()
{
    command(RemoteDef::C_PAUSE, "");
}

void Remote::playToggle()
{
    command(RemoteDef::C_PLAYTOGGLE, "");
}

void Remote::stop()
{
    command(RemoteDef::C_STOP, "");
}

void Remote::forward()
{
    command(RemoteDef::C_FORWARD, "");
}

void Remote::backward()
{
    command(RemoteDef::C_BACKWARD, "");
}

void Remote::next()
{
    command(RemoteDef::C_NEXT, "");
}

void Remote::previous()
{
    command(RemoteDef::C_PREVIOUS, "");
}

void Remote::info()
{
    command(RemoteDef::C_INFO, "");
}

void Remote::recordings()
{
    command(RemoteDef::C_RECORDINGS, "");
}

void Remote::record()
{
    command(RemoteDef::C_RECORD, "");
}

void Remote::live()
{
    command(RemoteDef::C_LIVE, "");
}

// navigation
void Remote::cursorUp()
{
    command(RemoteDef::C_CURSOR_UP, "");
}

void Remote::cursorDown()
{
    command(RemoteDef::C_CURSOR_DOWN, "");
}

void Remote::cursorLeft()
{
    command(RemoteDef::C_CURSOR_LEFT, "");
}

void Remote::cursorRight()
{
    command(RemoteDef::C_CURSOR_RIGHT, "");
}

void Remote::cursorOK()
{
    command(RemoteDef::C_CURSOR_OK, "");
}

void Remote::back()
{
    command(RemoteDef::C_BACK, "");
}

void Remote::home()
{
    command(RemoteDef::C_HOME, "");
}

void Remote::menu()
{
    command(RemoteDef::C_MENU, "");
}

void Remote::exit()
{
    command(RemoteDef::C_EXIT, "");
}

void Remote::app()
{
    command(RemoteDef::C_APP, "");
}

// power commands
void Remote::powerOn()
{
    command(RemoteDef::C_POWER_ON, "");
}

void Remote::powerOff()
{
    command(RemoteDef::C_POWER_OFF, "");
}

void Remote::powerToggle()
{
    qCDebug(m_log) << "POWERTOGGLE";
    command(RemoteDef::C_POWER_TOGGLE, "");
}

// tuner commands
void Remote::channelUp()
{
    qCDebug(m_log) << "CHANNEL_UP";
    command(RemoteDef::C_CHANNEL_UP, "");
}

void Remote::channelDown()
{
    qCDebug(m_log) << "CHANNEL_DOWN";
    command(RemoteDef::C_CHANNEL_DOWN, "");
}

void Remote::channelSearch()
{
    command(RemoteDef::C_CHANNEL_SEARCH, "");
}

void Remote::toFavorite()
{
    command(RemoteDef::C_FAVORITE, "");
}

void Remote::guide()
{
    command(RemoteDef::C_GUIDE, "");
}

// volume commands
void Remote::volumeUp()
{
    qCDebug(m_log) << "VOLUME_UP";
    command(RemoteDef::C_VOLUME_UP, "");
}

void Remote::volumeDown()
{
    qCDebug(m_log) << "VOLUME_DOWN";
    command(RemoteDef::C_VOLUME_DOWN, "");
}

void Remote::muteToggle()
{
    command(RemoteDef::C_MUTE_TOGGLE, "");
}

Remote::Remote(QObject *parent) :
    Entity (Type, QVariantMap(), nullptr, parent),
    m_log("REMOTE ENTITY")
{

}

Remote::Remote(const QVariantMap& config, IntegrationInterface* integrationObj, QObject *parent):
    Entity (Type, config, integrationObj, parent),
    m_log("REMOTE ENTITY")
{
    staticInitialize();

    m_enumAttr = &s_metaEnumAttr;
    m_enumFeatures = &s_metaEnumFeatures;
    m_enumCommands = &s_metaEnumCommands;
    m_enumState = &s_metaEnumState;

    m_specificInterface = qobject_cast<RemoteInterface*>(this);
    initializeSupportedFeatures(config);
    m_commands = config.value("commands").toJsonArray().toVariantList();
    emit commandsChanged();
}
