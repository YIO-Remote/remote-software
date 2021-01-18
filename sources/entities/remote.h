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
#include <QString>
#include <QVariant>

#include "entity.h"
#include "yio-interface/entities/remoteinterface.h"

class Remote : public Entity, RemoteInterface {
    Q_OBJECT
    Q_INTERFACES(RemoteInterface)

 public:
    Q_PROPERTY(QVariantList commands READ commands NOTIFY commandsChanged)
    Q_PROPERTY(QVariantList channels READ channels NOTIFY channelsChanged)

    QVariantList commands() override { return m_commands; }
    QVariantList channels() override { return m_channels; }
    QVariantMap  settings() override { return m_settings; }

    // transport and media controls
    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void playToggle();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void forward();
    Q_INVOKABLE void backward();
    Q_INVOKABLE void next();
    Q_INVOKABLE void previous();
    Q_INVOKABLE void info();
    Q_INVOKABLE void recordings();
    Q_INVOKABLE void record();
    Q_INVOKABLE void live();

    // navigation
    Q_INVOKABLE void cursorUp();
    Q_INVOKABLE void cursorDown();
    Q_INVOKABLE void cursorLeft();
    Q_INVOKABLE void cursorRight();
    Q_INVOKABLE void cursorOK();
    Q_INVOKABLE void back();
    Q_INVOKABLE void home();
    Q_INVOKABLE void menu();
    Q_INVOKABLE void exit();
    Q_INVOKABLE void app();

    // power commands
    Q_INVOKABLE void powerOn();
    Q_INVOKABLE void powerOff();
    Q_INVOKABLE void powerToggle();

    // tuner commands
    Q_INVOKABLE void channelUp();
    Q_INVOKABLE void channelDown();
    Q_INVOKABLE void channelSearch();
    Q_INVOKABLE void toFavorite();
    Q_INVOKABLE void guide();
    Q_INVOKABLE void channel(int ch);

    Q_INVOKABLE void source();

    // volume commands
    Q_INVOKABLE void volumeUp();
    Q_INVOKABLE void volumeDown();
    Q_INVOKABLE void muteToggle();

    // function commands
    Q_INVOKABLE void red();
    Q_INVOKABLE void green();
    Q_INVOKABLE void yellow();
    Q_INVOKABLE void blue();
    Q_INVOKABLE void orange();

    // custom commands
    Q_INVOKABLE void customCommand(int index);

    void turnOn() override { powerOn(); }
    void turnOff() override { powerOff(); }
    bool supportsOn() override;
    bool isOn() override;

    explicit Remote(QObject* parent = nullptr);
    Remote(const QVariantMap& config, IntegrationInterface* integrationObj, QObject* parent = nullptr);

    static void staticInitialize();

 signals:
    void commandsChanged();
    void channelsChanged();

 public:
    static QString Type;

 private:
    static QMetaEnum s_metaEnumCommands;
    static QMetaEnum s_metaEnumAttr;
    static QMetaEnum s_metaEnumFeatures;
    static QMetaEnum s_metaEnumState;

    QVariantList m_commands;
    QVariantList m_channels;
    QVariantMap  m_settings;
};
