/******************************************************************************
 *
 * Copyright (C) 2020 Markus Zehnder <business@markuszehnder.ch>
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

#include <QGuiApplication>

#include "device.h"

class DisplayControl : public Device {
    Q_OBJECT

    Q_PROPERTY(int currentBrightness READ currentBrightness NOTIFY currentBrightnessChanged)

 public:
    enum Mode { StandbyOn, StandbyOff };
    Q_ENUM(Mode)

    Q_INVOKABLE virtual bool setMode(Mode mode) = 0;

    Q_INVOKABLE virtual void setBrightness(int from, int to) = 0;
    Q_INVOKABLE virtual void setBrightness(int to)           = 0;

    Q_INVOKABLE virtual int currentBrightness() = 0;
    Q_INVOKABLE virtual int ambientBrightness() = 0;
    Q_INVOKABLE virtual int userBrightness()    = 0;

    virtual void             setAmbientBrightness(int value) = 0;
    Q_INVOKABLE virtual void setUserBrightness(int value)    = 0;

    Q_INVOKABLE virtual int   width()        = 0;
    Q_INVOKABLE virtual int   height()       = 0;
    Q_INVOKABLE virtual qreal pixelDensity() = 0;

 signals:
    void currentBrightnessChanged();

 protected:
    explicit DisplayControl(QString name, QObject* parent = nullptr) : Device(name, parent) {}
};
