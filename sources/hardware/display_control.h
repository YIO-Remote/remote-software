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

#ifndef DISPLAY_CONTROL_H
#define DISPLAY_CONTROL_H

#include <QObject>
#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>

#include <stdio.h>
#include <stdint.h>
#ifdef __arm__
    #include <wiringPi.h>
    #include <mcp23017.h>
#endif
#include <time.h>

class DisplayControl : public QObject
{
    Q_OBJECT

    // define timing
    struct timespec ts = {0, 40L};
    struct timespec ts2 = {0, 100L};
    struct timespec ts3 = {0, 300L};


public:
    Q_INVOKABLE bool setmode(const QString &mode);

    Q_INVOKABLE void setBrightness(int from, int to);

    Q_INVOKABLE void batteryChargingOn();
    Q_INVOKABLE void batteryChargingOff();

    DisplayControl();

    void spi_screenreg_set(int32_t Addr, int32_t Data0, int32_t Data1);
    void setup(void);
};

#endif // DISPLAY_CONTROL_H
