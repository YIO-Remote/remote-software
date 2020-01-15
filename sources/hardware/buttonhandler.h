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
#include <QVariant>

#include "../logger.h"
#include "../yioapi.h"
#include "hardwarefactory.h"
#include "interrupthandler.h"

class ButtonHandler : public QObject {
    Q_OBJECT

 public:
    enum Buttons {
        DPAD_UP      = InterruptHandler::DPAD_UP,
        DPAD_DOWN    = InterruptHandler::DPAD_DOWN,
        DPAD_LEFT    = InterruptHandler::DPAD_LEFT,
        DPAD_RIGHT   = InterruptHandler::DPAD_RIGHT,
        DPAD_MIDDLE  = InterruptHandler::DPAD_MIDDLE,
        TOP_RIGHT    = InterruptHandler::TOP_RIGHT,
        TOP_LEFT     = InterruptHandler::TOP_LEFT,
        BOTTOM_RIGHT = InterruptHandler::BOTTOM_RIGHT,
        BOTTOM_LEFT  = InterruptHandler::BOTTOM_LEFT,
        VOLUME_UP    = InterruptHandler::VOLUME_UP,
        VOLUME_DOWN  = InterruptHandler::VOLUME_DOWN,
        CHANNEL_UP   = InterruptHandler::CHANNEL_UP,
        CHANNEL_DOWN = InterruptHandler::CHANNEL_DOWN
    };
    Q_ENUM(Buttons)

    explicit ButtonHandler(InterruptHandler* interruptHandler, YioAPI* api, QObject* parent = nullptr);
    virtual ~ButtonHandler();

    static ButtonHandler* getInstance() { return s_instance; }
    static QObject*       getQMLInstance(QQmlEngine* engine, QJSEngine* scriptEngine);

 signals:
    void buttonPressed(int button);
    void buttonReleased(int button);

 private:
    static ButtonHandler* s_instance;
    QLoggingCategory      m_log;
    InterruptHandler*     m_itnerruptHandler;
    YioAPI*               m_api;

    int m_buttonPressed  = -1;
    int m_buttonReleased = -1;

 private slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void onInterrupt(int event);
    void onYIOAPIPressed(QString button);
    void onYIOAPIReleased(QString button);
};
