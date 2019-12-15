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

#ifndef REMOTE_H
#define REMOTE_H

#include <QObject>
#include <QString>
#include <QVariant>
#include "entity.h"

class Remote : public Entity
{
    Q_OBJECT

public:
    Q_PROPERTY  (QVariantList   commands       READ    commands       NOTIFY commandsChanged)
    QVariantList                commands() const { return m_commands; }

    // transport and media controls
    Q_INVOKABLE void            play();
    Q_INVOKABLE void            pause();
    Q_INVOKABLE void            playToggle();
    Q_INVOKABLE void            stop();
    Q_INVOKABLE void            forward();
    Q_INVOKABLE void            backward();
    Q_INVOKABLE void            next();
    Q_INVOKABLE void            previous();
    Q_INVOKABLE void            info();
    Q_INVOKABLE void            recordings();
    Q_INVOKABLE void            record();
    Q_INVOKABLE void            live();

    // navigation
    Q_INVOKABLE void            cursorUp();
    Q_INVOKABLE void            cursorDown();
    Q_INVOKABLE void            cursorLeft();
    Q_INVOKABLE void            cursorRight();
    Q_INVOKABLE void            cursorOK();
    Q_INVOKABLE void            back();
    Q_INVOKABLE void            home();
    Q_INVOKABLE void            menu();
    Q_INVOKABLE void            exit();
    Q_INVOKABLE void            app();

    // power commands
    Q_INVOKABLE void            powerOn();
    Q_INVOKABLE void            powerOff();
    Q_INVOKABLE void            powerToggle();

    // tuner commands
    Q_INVOKABLE void            channelUp();
    Q_INVOKABLE void            channelDown();
    Q_INVOKABLE void            channelSearch();
    Q_INVOKABLE void            toFavorite();
    Q_INVOKABLE void            guide();

    // volume commands
    Q_INVOKABLE void            volumeUp();
    Q_INVOKABLE void            volumeDown();
    Q_INVOKABLE void            muteToggle();

    void                        turnOn() override               { powerOn (); }
    void                        turnOff() override              { powerOff (); }


    explicit Remote(QObject *parent = nullptr);
    Remote(const QVariantMap& config, QObject* integrationObj, QObject *parent = nullptr);

signals:
    void commandsChanged();

public:
    static QString      Type;

    static QStringList&         AllFeatures()
    {
        static QStringList      s
        {
            // Transport and mediacontrols
            "PLAY", "PAUSE", "PLAYTOGGLE", "STOP", "FORWARD", "BACKWARD", "NEXT", "PREVIOUS", "INFO", "RECORDINGS", "RECORD", "LIVE",

            // digits
            "DIGIT_0", "DIGIT_1","DIGIT_2", "DIGIT_3", "DIGIT_4", "DIGIT_5", "DIGIT_6", "DIGIT_7", "DIGIT_8", "DIGIT_9", "DIGIT_10", "DIGIT_10+", "DIGIT_11", "DIGIT_12", "DIGIT_SEPARATOR", "DIGIT_ENTER,"

            // navigation
            "CURSOR_UP", "CURSOR_DOWN", "CURSOR_LEFT", "CURSOR_RIGHT", "CURSOR_OK", "BACK", "HOME", "MENU", "EXIT", "INFO", "APP", // INFO is repeated twice

            // power
            "POWER_OFF", "POWER_ON", "POWER_TOGGLE",

            //tuner
            "CHANNEL_UP", "CHANNEL_DOWN", "CHANNEL_SEARCH", "FAVORITE", "GUIDE",

            // interactive
            "FUNCTION_RED", "FUNCTION_GREEN", "FUNCTION_YELLOW", "FUNCTION_BLUE", "FUNCTION_ORANGE",

            // video
            "FORMAT_16_9", "FORMAT_4_3", "FORMAT_AUTO",

            // volume
            "VOLUME_UP", "VOLUME_DOWN", "MUTE_TOGGLE",

            // input
            "INPUT_TUNER_1", "INPUT_TUNER_2", "INPUT_TUNER_X", "INPUT_HDMI_1", "INPUT_HDMI_2", "INPUT_HDMI_X", "INPUT_X_1", "INPUT_X_2",

            // output
            "OUTPUT_HDMI_1", "OUTPUT_HDMI_2", "OUTPUT_DVI_1", "OUTPUT_AUDIO_X", "OUTPUT_X",

            // services
            "SERVICE_NETFLIX", "SERVICE_HULU"

        };

        return s;
    }

private:
    QVariantList        m_commands;

};

#endif // REMOTE_H
