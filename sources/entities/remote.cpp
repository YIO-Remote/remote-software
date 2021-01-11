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

#include "remote.h"

#include <QJsonArray>
#include <QTimer>

#include "../logging.h"
#include "../yioapi.h"

RemoteInterface::~RemoteInterface() {}

QString   Remote::Type = "remote";
QMetaEnum Remote::s_metaEnumCommands;
QMetaEnum Remote::s_metaEnumAttr;
QMetaEnum Remote::s_metaEnumFeatures;
QMetaEnum Remote::s_metaEnumState;

void Remote::staticInitialize() {
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
void Remote::play() {
    command(RemoteDef::C_PLAY, "");
}

void Remote::pause() {
    command(RemoteDef::C_PAUSE, "");
}

void Remote::playToggle() {
    command(RemoteDef::C_PLAYTOGGLE, "");
}

void Remote::stop() {
    command(RemoteDef::C_STOP, "");
}

void Remote::forward() {
    command(RemoteDef::C_FORWARD, "");
}

void Remote::backward() {
    command(RemoteDef::C_BACKWARD, "");
}

void Remote::next() {
    command(RemoteDef::C_NEXT, "");
}

void Remote::previous() {
    command(RemoteDef::C_PREVIOUS, "");
}

void Remote::info() {
    command(RemoteDef::C_INFO, "");
}

void Remote::recordings() {
    command(RemoteDef::C_RECORDINGS, "");
}

void Remote::record() {
    command(RemoteDef::C_RECORD, "");
}

void Remote::live() {
    command(RemoteDef::C_LIVE, "");
}

// navigation
void Remote::cursorUp() {
    command(RemoteDef::C_UP, "");
}

void Remote::cursorDown() {
    command(RemoteDef::C_DOWN, "");
}

void Remote::cursorLeft() {
    command(RemoteDef::C_LEFT, "");
}

void Remote::cursorRight() {
    command(RemoteDef::C_RIGHT, "");
}

void Remote::cursorOK() {
    command(RemoteDef::C_OK, "");
}

void Remote::back() {
    command(RemoteDef::C_BACK, "");
}

void Remote::home() {
    command(RemoteDef::C_HOME, "");
}

void Remote::menu() {
    command(RemoteDef::C_MENU, "");
}

void Remote::exit() {
    command(RemoteDef::C_EXIT, "");
}

void Remote::app() {
    command(RemoteDef::C_APP, "");
}

// power commands
void Remote::powerOn() {
    command(RemoteDef::C_POWER_ON, "");
}

void Remote::powerOff() {
    command(RemoteDef::C_POWER_OFF, "");
}

void Remote::powerToggle() {
    command(RemoteDef::C_POWER_TOGGLE, "");
}

// tuner commands
void Remote::channelUp() {
    command(RemoteDef::C_CHANNEL_UP, "");
}

void Remote::channelDown() {
    command(RemoteDef::C_CHANNEL_DOWN, "");
}

void Remote::channelSearch() {
    command(RemoteDef::C_CHANNEL_SEARCH, "");
}

void Remote::toFavorite() {
    command(RemoteDef::C_FAVORITE, "");
}

void Remote::guide() {
    command(RemoteDef::C_GUIDE, "");
}

void Remote::channel(int ch) {
    qCDebug(lcEntityRemote) << "Switching channel to" << ch;
    int delay = settings().value("delay").toInt();

    QString chStr = QString::number(ch);

    for (int i = 0; i < chStr.length(); i++) {
        QTimer* timer = new QTimer(this);
        timer->setSingleShot(true);

        int digit = chStr[i].digitValue();

        QObject* context = new QObject(this);
        connect(timer, &QTimer::timeout, context, [=]() {
            switch (digit) {
                case 0:
                    command(RemoteDef::C_DIGIT_0, "");
                    break;
                case 1:
                    command(RemoteDef::C_DIGIT_1, "");
                    break;
                case 2:
                    command(RemoteDef::C_DIGIT_2, "");
                    break;
                case 3:
                    command(RemoteDef::C_DIGIT_3, "");
                    break;
                case 4:
                    command(RemoteDef::C_DIGIT_4, "");
                    break;
                case 5:
                    command(RemoteDef::C_DIGIT_5, "");
                    break;
                case 6:
                    command(RemoteDef::C_DIGIT_6, "");
                    break;
                case 7:
                    command(RemoteDef::C_DIGIT_7, "");
                    break;
                case 8:
                    command(RemoteDef::C_DIGIT_8, "");
                    break;
                case 9:
                    command(RemoteDef::C_DIGIT_9, "");
                    break;
            }
            timer->deleteLater();
            context->deleteLater();
        });

        timer->start(delay);
    }
}

void Remote::source() {
    command(RemoteDef::C_SOURCE, "");
}

// volume commands
void Remote::volumeUp() {
    command(RemoteDef::C_VOLUME_UP, "");
}

void Remote::volumeDown() {
    command(RemoteDef::C_VOLUME_DOWN, "");
}

void Remote::muteToggle() {
    command(RemoteDef::C_MUTE_TOGGLE, "");
}

// function commands
void Remote::red() { command(RemoteDef::C_FUNCTION_RED, ""); }
void Remote::blue() { command(RemoteDef::C_FUNCTION_BLUE, ""); }
void Remote::green() { command(RemoteDef::C_FUNCTION_GREEN, ""); }
void Remote::orange() { command(RemoteDef::C_FUNCTION_ORANGE, ""); }
void Remote::yellow() { command(RemoteDef::C_FUNCTION_YELLOW, ""); }

// custom commands
void Remote::customCommand(int index) { clickCustomButton(index); }

bool Remote::supportsOn() {
    if (isSupported(RemoteDef::F_POWER_ON) && isSupported(RemoteDef::F_POWER_OFF)) {
        return true;
    } else {
        return false;
    }
}

bool Remote::isOn() {
    return m_state == RemoteDef::ONLINE;
}

Remote::Remote(QObject* parent) : Entity(Type, QVariantMap(), nullptr, parent) {}

Remote::Remote(const QVariantMap& config, IntegrationInterface* integrationObj, QObject* parent)
    : Entity(Type, config, integrationObj, parent) {
    staticInitialize();

    m_enumAttr = &s_metaEnumAttr;
    m_enumFeatures = &s_metaEnumFeatures;
    m_enumCommands = &s_metaEnumCommands;
    m_enumState = &s_metaEnumState;

    m_specificInterface = qobject_cast<RemoteInterface*>(this);
    initializeSupportedFeatures(config);
    m_commands = config.value("commands").toJsonArray().toVariantList();
    m_settings = config.value("settings").toMap();
    m_channels = config.value("channels").toList();
    emit commandsChanged();
    emit channelsChanged();
}
